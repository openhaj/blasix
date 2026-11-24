/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file exec.c
 * @brief The process pool that runs the tests.
 * @Created: 2026/11/24 08:15:05 by Moutig
 * @Updated: 2026/11/24 08:21:18 by Moutig
 *
 * Each test runs as `bin --run FUNC NAME` in a fresh process (its own
 * process group), with:
 *   - fd 1 and 2 -> one pipe (captured output, shown on failure);
 *   - fd 3       -> another pipe (framework events).
 * A poll loop multiplexes up to -j children, kills the group of a
 * test that outlives its timeout, and reaps with wait4 to get the
 * CPU time and the peak RSS.
 *
 * The status of a test is decided by finish() from three sources: the
 * events on fd 3 ("@end" pass / fail / skip, the checks), how the
 * process ended (exit status, signal), and the test's expected outcome
 * (.expect = LC_EXIT(n) / LC_SIGNAL(s)).
 */

#include "runner/lcrun.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/** Captured stdout + stderr kept per test (the rest is dropped). */
#define LC_OUT_MAX	(16 * 1024)

/** Events kept per test (a runaway LOG loop cannot exhaust memory). */
#define LC_EV_MAX	(1 << 20)

/**
 * @brief One running test: its process, its two pipes, what they sent.
 *
 * A slot is free when pid == 0.
 */
typedef struct s_slot
{
	t_test	*t;			/* Test being run. */
	pid_t	pid;		/* Child (also its process group id), 0 = free slot. */
	int		evFd;		/* Read end of the fd-3 pipe, -1 once closed. */
	int		outFd;		/* Read end of the stdout + stderr pipe, -1 once closed. */
	char	*ev;		/* raw event bytes */
	size_t	evLen;		/* Bytes in ev. */
	char	*out;		/* Captured output. */
	size_t	outLen;		/* Bytes in out. */
	double	start;		/* nowMs() at fork. */
	int		limit;		/* timeout in seconds */
	int		timedOut;	/* Killed for its timeout. */
	int		ended;		/* "@end" seen */
	int		endSt;		/* status from @end */
}	t_slot;

/** Width of the progress bar, in characters. */
#define LC_BAR	30

const char	*statusName(t_status st)
{
	static const char	*names[ST_COUNT] = {
		"pass", "fail", "skip", "crash", "timeout", "error", "unavailable"
	};

	return (names[st]);
}

/**
 * @brief Close @p fd in the children spawned later (FD_CLOEXEC).
 *
 * @param fd File descriptor of the parent.
 */
static void	setCloexec(int fd)
{
	fcntl(fd, F_SETFD, FD_CLOEXEC);
}

/**
 * @brief Start one test: `bin --run FUNC NAME` in a new process group.
 *
 * In the child: fd 1 and 2 go to the output pipe, fd 3 to the event
 * pipe, every other descriptor is closed, then the binary is executed
 * (status 127 if it cannot be). The process group lets runAll() kill
 * whatever the test itself forked. The parent keeps the read ends,
 * non-blocking and close-on-exec.
 *
 * @param s Free slot, filled on success.
 * @param t Test to run.
 * @return 0, or -1 if a pipe or the process cannot be created.
 */
static int	spawn(t_slot *s, t_test *t)
{
	int		ev[2];
	int		out[2];
	pid_t	pid;

	if (pipe(ev) < 0 || pipe(out) < 0)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0)
	{
		setpgid(0, 0);
		dup2(out[1], 1);
		dup2(out[1], 2);
		dup2(ev[1], 3);
		for (int fd = 4; fd < 256; fd++) {
			close(fd);
		}
		execl(t->unit->bin, t->unit->bin, "--run", t->func, t->name, (char *)NULL);
		_exit(127);
	}
	setpgid(pid, pid);
	close(ev[1]);
	close(out[1]);
	setCloexec(ev[0]);
	setCloexec(out[0]);
	fcntl(ev[0], F_SETFL, O_NONBLOCK);
	fcntl(out[0], F_SETFL, O_NONBLOCK);
	memset(s, 0, sizeof(*s));
	s->t = t;
	s->pid = pid;
	s->evFd = ev[0];
	s->outFd = out[0];
	s->start = nowMs();
	return (0);
}

/**
 * @brief Read everything available on a non-blocking pipe.
 *
 * Bytes past @p max are read and dropped, so a chatty test cannot fill
 * the pipe and block.
 *
 * @param fd  Read end of a pipe.
 * @param buf Heap buffer, grown and kept NUL-terminated.
 * @param len Bytes in @p buf.
 * @param max Bytes kept at most.
 * @return 0 on end of file (the child closed its end), 1 otherwise.
 */
static int	drain(int fd, char **buf, size_t *len, size_t max)
{
	char	tmp[4096];
	ssize_t	n;

	for (;;)
	{
		n = read(fd, tmp, sizeof(tmp));
		if (n > 0)
		{
			size_t	keep = (*len + (size_t)n > max) ? max - *len : (size_t)n;

			if (keep > 0)
			{
				*buf = xrealloc(*buf, *len + keep + 1);
				memcpy(*buf + *len, tmp, keep);
				*len += keep;
				(*buf)[*len] = '\0';
			}
			continue ;
		}
		if (n == 0)
			return (0);
		if (errno == EINTR)
			continue ;
		return (1);
	}
}

/**
 * @brief Undo the field escaping of the protocol, in place (\\t \\n \\\\).
 *
 * @param s Field.
 */
static void	unescape(char *s)
{
	char	*d = s;

	for (; *s; s++)
	{
		if (*s == '\\' && s[1])
		{
			s++;
			*d++ = (*s == 't') ? '\t' : (*s == 'n') ? '\n' : *s;
		}
		else
			*d++ = *s;
	}
	*d = '\0';
}

/**
 * @brief Turn the fd-3 event stream into the test's counters and messages.
 *
 * "@fail", "@skip" and "@log" become lines of t->msgs; "@end" gives
 * the status, the number of checks and of failures. Unknown lines are
 * ignored. Consumes s->ev (strtok_r).
 *
 * @param s Slot of a finished test.
 */
static void	parseEvents(t_slot *s)
{
	t_test	*t = s->t;
	char	*line;
	char	*save;
	char	buf[4096];

	if (s->ev == NULL)
		return ;
	for (line = strtok_r(s->ev, "\n", &save); line; line = strtok_r(NULL, "\n", &save))
	{
		char	*f[4] = {0};
		char	*p = line;
		int		n = 0;

		while (n < 4)
		{
			f[n++] = p;
			p = strchr(p, '\t');
			if (p == NULL)
				break ;
			*p++ = '\0';
		}
		for (int i = 1; i < n; i++)
			unescape(f[i]);
		if (!strcmp(f[0], "@fail") && n >= 4)
			snprintf(buf, sizeof(buf), "%s:%s: %s\n", f[1], f[2], f[3]);
		else if (!strcmp(f[0], "@skip") && n >= 2)
			snprintf(buf, sizeof(buf), "skipped: %s\n", f[1]);
		else if (!strcmp(f[0], "@log") && n >= 2)
			snprintf(buf, sizeof(buf), "log: %s\n", f[1]);
		else if (!strcmp(f[0], "@end") && n >= 4)
		{
			s->ended = 1;
			s->endSt = !strcmp(f[1], "pass") ? ST_PASS
				: !strcmp(f[1], "skip") ? ST_SKIP : ST_FAIL;
			t->asserts = atoi(f[2]);
			t->failed = atoi(f[3]);
			continue ;
		}
		else
			continue ;
		strAppend(&t->msgs, buf, strlen(buf));
	}
}

/**
 * @brief Add a runner message (timeout, signal, ...) to the test's messages.
 *
 * @param t   Test.
 * @param msg Line, newline included.
 */
static void	note(t_test *t, const char *msg)
{
	strAppend(&t->msgs, msg, strlen(msg));
}

/**
 * @brief Decide the final status from the events, exit status and timeout.
 *
 * In order: killed for its timeout; died from a signal (pass if it was
 * the expected one); expected exit status; expected signal that did not
 * come; no "@end" (error: the process ended inside the test); "@end"
 * pass but a non-zero exit (error); otherwise the status of "@end".
 * Also records the times, the peak RSS and the captured output, and
 * counts the run for -r.
 *
 * @param s   Slot of the reaped child.
 * @param wst Wait status.
 * @param ru  Resource usage of the child.
 */
static void	finish(t_slot *s, int wst, const struct rusage *ru)
{
	t_test	*t = s->t;
	int		wantSig = (t->expect & 0x200) ? (t->expect & 0xff) : 0;
	int		wantExit = (t->expect & 0x100) ? (t->expect & 0xff) : -1;
	char	buf[256];

	free(t->msgs);
	t->msgs = NULL;
	t->asserts = 0;
	t->failed = 0;
	parseEvents(s);
	t->ms = nowMs() - s->start;
	t->cpuMs = (double)(ru->ru_utime.tv_sec + ru->ru_stime.tv_sec) * 1e3
		+ (double)(ru->ru_utime.tv_usec + ru->ru_stime.tv_usec) / 1e3;
	t->rssKb = ru->ru_maxrss;
	t->sig = WIFSIGNALED(wst) ? WTERMSIG(wst) : 0;
	t->code = WIFEXITED(wst) ? WEXITSTATUS(wst) : -1;
	free(t->out);
	t->out = s->out;
	s->out = NULL;
	if (s->timedOut)
	{
		t->st = ST_TIMEOUT;
		snprintf(buf, sizeof(buf), "killed after %d s\n", s->limit);
		note(t, buf);
	}
	else if (t->sig)
	{
		if (t->sig == wantSig)
			t->st = (t->failed ? ST_FAIL : ST_PASS);
		else
		{
			t->st = ST_CRASH;
			snprintf(buf, sizeof(buf), "killed by signal %d (%s)\n",
				t->sig, strsignal(t->sig));
			note(t, buf);
		}
	}
	else if (wantExit >= 0)
	{
		t->st = (!s->ended && t->code == wantExit && !t->failed) ? ST_PASS : ST_FAIL;
		if (t->st == ST_FAIL)
		{
			snprintf(buf, sizeof(buf), "expected exit status %d, got %s %d\n",
				wantExit, s->ended ? "a normal return then" : "status", t->code);
			note(t, buf);
		}
	}
	else if (wantSig)
	{
		t->st = ST_FAIL;
		snprintf(buf, sizeof(buf), "expected signal %d (%s), exited with %d\n",
			wantSig, strsignal(wantSig), t->code);
		note(t, buf);
	}
	else if (!s->ended)
	{
		t->st = ST_ERROR;
		snprintf(buf, sizeof(buf), t->code == 127
			? "could not exec the test binary\n"
			: "process exited with status %d before the end of the test\n",
			t->code);
		note(t, buf);
	}
	else if (t->code != 0 && s->endSt == ST_PASS)
	{
		t->st = ST_ERROR;
		snprintf(buf, sizeof(buf), "test passed but the process exited with %d\n",
			t->code);
		note(t, buf);
	}
	else
		t->st = (t_status)s->endSt;
	t->runs++;
	if (t->st == ST_PASS)
		t->passes++;
	free(s->ev);
	s->ev = NULL;
}

/**
 * @brief Draw the progress bar of the run on a terminal (nothing with -q
 *        or when stdout is not a terminal):
 *
 *     testing [##########....................]  523/1376  2 failed
 *
 * Redrawn at most every 50 ms (and for the last test), then erased.
 *
 * @param r      Run.
 * @param done   Tests finished.
 * @param total  Tests to run.
 * @param failed Finished tests that did not pass (fail, crash, ...).
 */
static void	progress(const t_run *r, size_t done, size_t total, size_t failed)
{
	static double	last;
	double			now = nowMs();
	char			bar[LC_BAR + 1];
	size_t			fill;
	int				digits = snprintf(NULL, 0, "%zu", total);

	if (r->o.quiet || !r->o.color || total == 0)
		return ;
	if (done < total && now - last < 50)
		return ;
	last = now;
	fill = done * LC_BAR / total;
	for (size_t i = 0; i < LC_BAR; i++)
		bar[i] = i < fill ? '#' : '.';
	bar[LC_BAR] = '\0';
	fprintf(stderr, "\r\033[2K  testing [%s] %*zu/%zu", bar, digits, done, total);
	if (failed)
		fprintf(stderr, "  \033[31m%zu failed\033[0m", failed);
	if (done == total)
		fprintf(stderr, "\r\033[2K");
}

void	runAll(t_run *r)
{
	t_slot			*slots;
	struct pollfd	*pfd;
	size_t			next;
	size_t			done;
	size_t			total;
	size_t			failed;
	int				active;

	signal(SIGPIPE, SIG_IGN);
	slots = xmalloc((size_t)r->o.jobs * sizeof(*slots));
	pfd = xmalloc((size_t)r->o.jobs * 2 * sizeof(*pfd));
	for (int i = 0; i < r->o.jobs; i++)
		slots[i].pid = 0;
	total = r->nTests * (size_t)r->o.repeat;
	next = 0;
	done = 0;
	failed = 0;
	active = 0;
	progress(r, 0, total, 0);
	while (done < total)
	{
		/* fill free slots */
		for (int i = 0; i < r->o.jobs && next < total; i++)
		{
			if (slots[i].pid != 0)
				continue ;
			if (spawn(&slots[i], &r->tests[next % r->nTests]) < 0)
			{
				perror("lcrun: spawn");
				exit(2);
			}
			next++;
			active++;
		}
		/* wait for output */
		int	n = 0;
		for (int i = 0; i < r->o.jobs; i++)
		{
			if (slots[i].pid == 0)
				continue ;
			if (slots[i].evFd >= 0)
				pfd[n++] = (struct pollfd){.fd = slots[i].evFd, .events = POLLIN};
			if (slots[i].outFd >= 0)
				pfd[n++] = (struct pollfd){.fd = slots[i].outFd, .events = POLLIN};
		}
		if (n > 0)
			poll(pfd, (nfds_t)n, 20);
		/* drain, timeouts, reap */
		for (int i = 0; i < r->o.jobs; i++)
		{
			t_slot			*s = &slots[i];
			int				wst;
			struct rusage	ru;
			int				limit;
			pid_t			w;

			if (s->pid == 0)
				continue ;
			if (s->evFd >= 0 && drain(s->evFd, &s->ev, &s->evLen, LC_EV_MAX) == 0)
			{
				close(s->evFd);
				s->evFd = -1;
			}
			if (s->outFd >= 0 && drain(s->outFd, &s->out, &s->outLen, LC_OUT_MAX) == 0)
			{
				close(s->outFd);
				s->outFd = -1;
			}
			limit = s->t->timeout ? s->t->timeout : r->o.timeout;
			s->limit = limit;
			if (!s->timedOut && nowMs() - s->start > limit * 1e3)
			{
				s->timedOut = 1;
				kill(-s->pid, SIGKILL);
			}
			w = wait4(s->pid, &wst, WNOHANG, &ru);
			if (w != s->pid)
				continue ;
			/* the child is gone: collect what is left, kill leftovers */
			if (s->evFd >= 0)
				drain(s->evFd, &s->ev, &s->evLen, LC_EV_MAX);
			if (s->outFd >= 0)
				drain(s->outFd, &s->out, &s->outLen, LC_OUT_MAX);
			kill(-s->pid, SIGKILL);
			if (s->evFd >= 0)
				close(s->evFd);
			if (s->outFd >= 0)
				close(s->outFd);
			finish(s, wst, &ru);
			failed += (s->t->st != ST_PASS && s->t->st != ST_SKIP);
			s->pid = 0;
			active--;
			done++;
			progress(r, done, total, failed);
		}
	}
	(void)active;
	free(slots);
	free(pfd);
}
