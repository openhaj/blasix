/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file lc.c
 * @brief Test-side engine: checks, output protocol, main().
 * @Created: 2026/11/23 04:42:37 by Moutig
 * @Updated: 2026/11/25 08:41:04 by Moutig
 *
 * Linked into every test binary, compiled against the libc under
 * test but calling none of its functions (see sys.c).
 *
 * Command line of a test binary:
 *
 *     bin                       run every test in-process, human output
 *     bin --list                describe the tests (one line each, fd 1)
 *     bin --run FUNC NAME       run one test
 *
 * When fd 3 is open (lcrun sets it to a pipe), --run writes machine
 * events there; the test's own stdout / stderr stay free for the libc
 * under test. Protocol, one event per line, tab-separated, fields
 * escaped (\t \n \\):
 *
 *     @test  CAT FUNC NAME FILE LINE TIMEOUT EXPECT SINCE UNTIL OPT REF  (--list)
 *     @fail  FILE LINE MESSAGE
 *     @skip  REASON
 *     @unimpl WHAT                (NOT_IMPLEMENTED: the libc lacks it)
 *     @log   MESSAGE
 *     @end   pass|fail|skip|unimplemented ASSERTS FAILED
 *
 * Without fd 3 (a binary run by hand, e.g. under a debugger) the same
 * information is printed for humans on stderr.
 *
 * Output goes through a small buffer of this file (gBuf) flushed with
 * raw write(2): no stdio, no malloc. Numbers, pointers and quoted
 * strings are formatted here too.
 */

#define LC_FRAMEWORK
#include "framework/lc.h"
#include "framework/sys.h"

/*
 * Bounds of the "lc_tests" section, provided by the linker. Weak: a
 * binary without any test links too (both are then NULL).
 */
extern const struct lc_test	__start_lc_tests[] __attribute__((weak));
extern const struct lc_test	__stop_lc_tests[] __attribute__((weak));

/**
 * @brief Write the coverage counters of this process (cov.c).
 *
 * Only defined in coverage builds (COV=1): the weak reference is NULL
 * otherwise and main() does not call it.
 */
void	lc_covDump(void) __attribute__((weak));

/**
 * @brief Dump the coverage counters when signal @p sig kills the test
 *        (cov.c).
 *
 * Coverage builds only, like lc_covDump(). Called for a test that
 * expects LC_SIGNAL(sig), before its body runs.
 *
 * @param sig Signal number.
 */
void	lc_covArmSignal(int sig) __attribute__((weak));

/* ----- State ----- */

static int					gProto;		/* 1: events on fd 3 for lcrun */
static int					gFd = 2;	/* where reports go */
static int					gEscape;	/* escape \t \n \\ while writing a field */

/** Result of the current test so far. */
struct lc_counts
{
	int	asserts;	/* checks run */
	int	failed;		/* checks failed */
	int	skipped;	/* SKIP was called */
	int	unimpl;		/* NOT_IMPLEMENTED was called */
};

static struct lc_counts		gLocal;
/* &gLocal, or a shared page once the test forks with lc_child() */
static struct lc_counts		*gCnt = &gLocal;

/* ----- Output (own buffer, raw write) ----- */

static char		gBuf[4096];	/* pending output */
static unsigned	gLen;		/* bytes used in gBuf */

/**
 * @brief Write the pending output to gFd and empty the buffer.
 *
 * Loops on short writes; stops on an error (nothing else can be done
 * with a report that cannot be written).
 */
static void	lcFlush(void)
{
	unsigned	off;
	long		r;

	off = 0;
	while (off < gLen)
	{
		r = lc_sysWrite(gFd, gBuf + off, gLen - off);
		if (r <= 0)
			break ;
		off += (unsigned)r;
	}
	gLen = 0;
}

/**
 * @brief Append one byte to the output buffer, as is.
 *
 * @param c Byte to append (the buffer is flushed first when full).
 */
static void	lcRawc(char c)
{
	if (gLen == sizeof(gBuf))
		lcFlush();
	gBuf[gLen++] = c;
}

/**
 * @brief Append one character, escaped when writing a protocol field.
 *
 * With gEscape set, tab, newline and backslash become \\t, \\n and \\\\ so
 * that a field never breaks the tab-separated, one-event-per-line
 * protocol.
 *
 * @param c Character to append.
 */
static void	lcc(char c)
{
	if (gEscape && (c == '\t' || c == '\n' || c == '\\'))
	{
		lcRawc('\\');
		c = (c == '\t') ? 't' : (c == '\n') ? 'n' : '\\';
	}
	lcRawc(c);
}

/**
 * @brief Append a string.
 *
 * @param s NUL-terminated string; NULL is written as "(null)".
 */
static void	lcs(const char *s)
{
	if (s == 0)
		s = "(null)";
	while (*s)
		lcc(*s++);
}

/**
 * @brief Append an unsigned integer in decimal.
 *
 * @param v Value.
 */
static void	lcu(unsigned long long v)
{
	char	tmp[24];
	int		i;

	i = 0;
	do
		tmp[i++] = (char)('0' + v % 10);
	while ((v /= 10) != 0);
	while (i > 0)
		lcc(tmp[--i]);
}

/**
 * @brief Append a signed integer in decimal.
 *
 * LLONG_MIN is handled without overflow: -(v + 1) + 1 is computed in
 * unsigned arithmetic.
 *
 * @param v Value.
 */
static void	lci(long long v)
{
	if (v < 0)
	{
		lcc('-');
		lcu((unsigned long long)(-(v + 1)) + 1);
	}
	else
		lcu((unsigned long long)v);
}

/**
 * @brief Append the low @p digits hexadecimal digits of @p v.
 *
 * @param v      Value.
 * @param digits Number of digits, leading zeros included.
 */
static void	lcx(unsigned long long v, int digits)
{
	static const char	hex[] = "0123456789abcdef";
	int					i;

	for (i = digits - 1; i >= 0; i--)
		lcc(hex[(v >> (i * 4)) & 0xf]);
}

/**
 * @brief Append a pointer: "NULL" or 0x followed by every hex digit.
 *
 * @param p Pointer.
 */
static void	lcPtr(const void *p)
{
	if (p == 0)
	{
		lcs("NULL");
		return ;
	}
	lcs("0x");
	lcx((unsigned long long)(__UINTPTR_TYPE__)p, (int)sizeof(void *) * 2);
}

/**
 * @brief Append a string quoted, escaped and truncated, for failure messages.
 *
 * Quotes and backslashes are escaped, non-printable bytes are written
 * as \\xHH, and only the first 80 bytes are shown (then "...").
 *
 * @param s String, or NULL (written as NULL, without quotes).
 */
static void	lcQuoted(const char *s)
{
	int	n;

	if (s == 0)
	{
		lcs("NULL");
		return ;
	}
	lcc('"');
	for (n = 0; s[n] && n < 80; n++)
	{
		unsigned char	c = (unsigned char)s[n];

		if (c == '"' || c == '\\')
		{
			lcc('\\');
			lcc((char)c);
		}
		else if (c >= 0x20 && c < 0x7f)
			lcc((char)c);
		else
		{
			lcs("\\x");
			lcx(c, 2);
		}
	}
	lcc('"');
	if (s[n])
		lcs("...");
}

/* ----- Small string helpers (no libc) ----- */

lc_size_t	lc_strlen(const char *s)
{
	lc_size_t	n;

	n = 0;
	while (s[n])
		n++;
	return (n);
}

void	lc_fill(void *p, int c, lc_size_t n)
{
	volatile unsigned char	*d = p;

	while (n--)
		*d++ = (unsigned char)c;
}

/*
 * 64 KiB: a multiple of every page size Linux uses (4, 16, 64 KiB), so
 * the guard is page aligned without asking the kernel for the size.
 */
#define LC_GUARD	65536UL

void	*lc_guarded(lc_size_t n)
{
	unsigned long	len;
	unsigned char	*base;

	len = (n + LC_GUARD - 1) / LC_GUARD * LC_GUARD;
	base = lc_sysMmap(len + LC_GUARD);
	if (!base || lc_sysMprotect(base + len, LC_GUARD, 0) != 0)
		return (0);
	return (base + len - n);
}

int	lc_unlink(const char *path)
{
	return ((int)lc_sysUnlink(path));
}

void	lc_exitNow(int status)
{
	/* Raw exit_group skips the destructors: dump here. */
	if (lc_covDump)
		lc_covDump();
	lc_sysExit(status);
}

/**
 * @brief Are two strings equal? (strcmp() == 0 without the libc).
 *
 * @param a First string.
 * @param b Second string.
 * @return 1 if equal, 0 otherwise.
 */
static int	lcStreq(const char *a, const char *b)
{
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return (*a == *b);
}

char	*lc_tmpPath(char *buf, lc_size_t size, const char *tag)
{
	static const char	pre[] = "/tmp/lc-";
	char				num[24];
	lc_size_t			i;
	lc_size_t			j;
	long				pid;
	int					k;

	i = 0;
	for (j = 0; pre[j] && i + 1 < size; j++)
		buf[i++] = pre[j];
	pid = lc_sysGetpid();
	k = 0;
	do
		num[k++] = (char)('0' + pid % 10);
	while ((pid /= 10) != 0);
	while (k > 0 && i + 1 < size)
		buf[i++] = num[--k];
	if (i + 1 < size)
		buf[i++] = '-';
	for (j = 0; tag[j] && i + 1 < size; j++)
		buf[i++] = tag[j];
	buf[i] = '\0';
	return (buf);
}

/** First descriptor the capture may use for its own files. */
#define LC_CAPTURE_MINFD	10

/**
 * @brief Move descriptor @p fd to a number >= LC_CAPTURE_MINFD.
 *
 * @param fd Descriptor (closed here).
 * @return The new descriptor (close-on-exec), or -errno.
 */
static long	lcHighFd(long fd)
{
	long	r;

	r = lc_sysFcntl((int)fd, LC_F_DUPFD_CLOEXEC, LC_CAPTURE_MINFD);
	lc_sysClose((int)fd);
	return (r);
}

int	lc_captureStart(lc_capture *c, int fd)
{
	char	path[64];
	long	r;

	c->fd = fd;
	c->saved = -1;
	r = lc_sysFcntl(fd, LC_F_DUPFD_CLOEXEC, LC_CAPTURE_MINFD);
	if (r >= 0)
		c->saved = (int)r;
	else if (r != -9)	/* -EBADF: fd was closed, nothing to save */
		return ((int)r);
	lc_tmpPath(path, sizeof(path), "capture");
	r = lc_sysOpen(path, LC_O_RDWR | LC_O_CREAT | LC_O_TRUNC | LC_O_CLOEXEC,
			0600);
	if (r >= 0)
	{
		lc_sysUnlink(path);
		r = lcHighFd(r);
	}
	if (r >= 0)
	{
		c->file = (int)r;
		r = lc_sysDup2(c->file, fd);
		if (r >= 0)
			return (0);
		lc_sysClose(c->file);
	}
	if (c->saved >= 0)
		lc_sysClose(c->saved);
	return ((int)r);
}

long	lc_captureEnd(lc_capture *c, char *buf, lc_size_t size)
{
	long		len;
	long		r;
	lc_size_t	got;

	if (c->saved >= 0)
	{
		lc_sysDup2(c->saved, c->fd);
		lc_sysClose(c->saved);
	}
	else
		lc_sysClose(c->fd);
	len = lc_sysLseek(c->file, 0, LC_SEEK_END);
	got = 0;
	if (len >= 0 && size > 0)
	{
		r = lc_sysLseek(c->file, 0, LC_SEEK_SET);
		while (r >= 0 && got + 1 < size)
		{
			r = lc_sysRead(c->file, buf + got, size - 1 - got);
			if (r <= 0)
				break ;
			got += (lc_size_t)r;
		}
	}
	if (size > 0)
		buf[got] = '\0';
	lc_sysClose(c->file);
	return (len);
}

/**
 * @brief Move the check counters to memory shared with the children.
 *
 * Once per process; the counts so far are kept.
 *
 * @return 0, or -ENOMEM.
 */
static int	lcShareCounts(void)
{
	struct lc_counts	*shared;

	if (gCnt != &gLocal)
		return (0);
	shared = lc_sysMmapShared(sizeof(*shared));
	if (!shared)
		return (-12);
	*shared = gLocal;
	gCnt = shared;
	return (0);
}

int	lc_child(void (*body)(void *), void *arg)
{
	long	parent;
	long	pid;
	long	r;
	int		st;

	if (lcShareCounts() != 0)
		return (-12);
	lcFlush();		/* or the child writes the pending output again */
	parent = lc_sysGetpid();
	pid = lc_sysFork();
	if (pid < 0)
		return ((int)pid);
	if (pid == 0)
	{
		/* no orphan if the test is killed for its timeout */
		lc_sysPrctl(LC_PR_SET_PDEATHSIG, LC_SIGKILL);
		if (lc_sysGetppid() != parent)
			lc_sysExit(1);
		body(arg);
		lcFlush();
		lc_exitNow(0);
	}
	do
		r = lc_sysWait4(pid, &st, 0);
	while (r == -4);	/* -EINTR */
	if (r < 0)
		return ((int)r);
	if ((st & 0x7f) == 0)
		return (LC_EXIT((st >> 8) & 0xff));
	return (LC_SIGNAL(st & 0x7f));
}

/**
 * @brief Read a clock in nanoseconds.
 *
 * @param clk LC_CLOCK_REALTIME or LC_CLOCK_MONOTONIC.
 * @return Nanoseconds, or 0 if the clock cannot be read.
 */
static long long	lcClockNs(int clk)
{
	struct lc_timespec	ts;

	if (lc_sysClockGettime(clk, &ts) != 0)
		return (0);
	return (ts.sec * LC_NS_PER_SEC + ts.nsec);
}

long long	lc_now(void)
{
	return (lcClockNs(LC_CLOCK_MONOTONIC));
}

long long	lc_nowReal(void)
{
	return (lcClockNs(LC_CLOCK_REALTIME));
}

void	lc_sleep(long long ns)
{
	struct lc_timespec	end;
	long long			t;

	if (ns <= 0)
		return ;
	t = lc_now() + ns;
	end.sec = (long)(t / LC_NS_PER_SEC);
	end.nsec = (long)(t % LC_NS_PER_SEC);
	while (lc_sysClockNanosleep(LC_CLOCK_MONOTONIC, LC_TIMER_ABSTIME,
			&end, 0) == -4)	/* -EINTR: same deadline, sleep again */
		;
}

/* ----- Failure reporting ----- */

/**
 * @brief Count a failure and write the start of its report.
 *
 * Protocol: "@fail\tFILE\tLINE\t" then the message, escaped. Human
 * output: "    FAIL file:line: ". The caller appends the values and ends
 * the line with lcFailEnd().
 *
 * @param file Source file of the check.
 * @param line Source line of the check.
 * @param text Text of the macro, written first in the message.
 */
static void	lcFailBegin(const char *file, int line, const char *text)
{
	gCnt->failed++;
	if (gProto)
	{
		gEscape = 0;
		lcs("@fail\t");
		gEscape = 1;
		lcs(file);
		gEscape = 0;
		lcc('\t');
		gEscape = 1;
		lci(line);
		gEscape = 0;
		lcc('\t');
		gEscape = 1;
	}
	else
	{
		lcs("    FAIL ");
		lcs(file);
		lcc(':');
		lci(line);
		lcs(": ");
	}
	lcs(text);
	lcs(": ");
}

/**
 * @brief End the report of a failure and flush it.
 *
 * Flushed at once: if the test crashes right after, the failure is
 * already with lcrun.
 */
static void	lcFailEnd(void)
{
	gEscape = 0;
	lcc('\n');
	lcFlush();
}

int	lc_check(int ok, const char *file, int line, const char *text)
{
	gCnt->asserts++;
	if (ok)
		return (1);
	lcFailBegin(file, line, text);
	lcs("failed");
	lcFailEnd();
	return (0);
}

int		lc_checkInt(int ok,
					const char	*file,	int			line,	const char	*text,
					long long	a,		long long	b,		const char	*op)
{
	gCnt->asserts++;
	if (ok)
		return (1);
	lcFailBegin(file, line, text);
	lci(a);
	lcc(' ');
	lcs(op);
	lcc(' ');
	lci(b);
	lcs(" is false");
	lcFailEnd();
	return (0);
}

int		lc_checkUint(int ok,
					 const char			*file,	int					line,	const char	*text,
					 unsigned long long	a,		unsigned long long	b,		const char	*op)
{
	gCnt->asserts++;
	if (ok)
		return (1);
	lcFailBegin(file, line, text);
	lcu(a);
	lcc(' ');
	lcs(op);
	lcc(' ');
	lcu(b);
	lcs(" is false");
	lcFailEnd();
	return (0);
}

int		lc_checkPtr(int ok,
					const char	*file,	int			line, const char *text,
					const void	*a,		const void	*b)
{
	gCnt->asserts++;
	if (ok)
		return (1);
	lcFailBegin(file, line, text);
	lcPtr(a);
	lcs(" vs ");
	lcPtr(b);
	lcFailEnd();
	return (0);
}

int		lc_checkStr(int wantEqual,
					const char	*file,	int			line, const char *text,
					const char	*a,		const char	*b)
{
	int			eq;
	lc_size_t	i;

	gCnt->asserts++;
	if (a == 0 || b == 0)
		eq = (a == b);
	else
		eq = lcStreq(a, b);
	if (eq == wantEqual)
		return (1);
	lcFailBegin(file, line, text);
	lcQuoted(a);
	lcs(wantEqual ? " vs " : " equals ");
	lcQuoted(b);
	if (wantEqual && a != 0 && b != 0)
	{
		for (i = 0; a[i] && a[i] == b[i]; i++)
			;
		lcs(" (first difference at index ");
		lcu(i);
		lcc(')');
	}
	lcFailEnd();
	return (0);
}

int		lc_checkMem(const char	*file,	int			line,	const char	*text,
					const void	*a,		const void	*b,		lc_size_t	n)
{
	const unsigned char	*pa = a;
	const unsigned char	*pb = b;
	lc_size_t			i;

	gCnt->asserts++;
	if (pa == 0 || pb == 0)
	{
		if (pa == pb)
			return (1);
		lcFailBegin(file, line, text);
		lcs("NULL pointer");
		lcFailEnd();
		return (0);
	}
	for (i = 0; i < n && pa[i] == pb[i]; i++)
		;
	if (i == n)
		return (1);
	lcFailBegin(file, line, text);
	lcs("differ at byte ");
	lcu(i);
	lcs(": 0x");
	lcx(pa[i], 2);
	lcs(" vs 0x");
	lcx(pb[i], 2);
	lcFailEnd();
	return (0);
}

void	lc_skip(const char *reason)
{
	gCnt->skipped = 1;
	if (gProto)
	{
		lcs("@skip\t");
		gEscape = 1;
		lcs(reason);
		gEscape = 0;
	}
	else
	{
		lcs("    SKIP ");
		lcs(reason);
	}
	lcc('\n');
	lcFlush();
}

void	lc_unimplemented(const char *what)
{
	gCnt->unimpl = 1;
	if (gProto)
	{
		lcs("@unimpl\t");
		gEscape = 1;
		lcs(what);
		gEscape = 0;
	}
	else
	{
		lcs("    N/I  ");
		lcs(what);
	}
	lcc('\n');
	lcFlush();
}

void	lc_log(const char *msg)
{
	if (gProto)
	{
		lcs("@log\t");
		gEscape = 1;
		lcs(msg);
		gEscape = 0;
	}
	else
	{
		lcs("    LOG  ");
		lcs(msg);
	}
	lcc('\n');
	lcFlush();
}

/* ----- Running ----- */

/**
 * @brief Append an escaped protocol field followed by a tab.
 *
 * @param s Field value.
 */
static void	lcField(const char *s)
{
	gEscape = 1;
	lcs(s);
	gEscape = 0;
	lcc('\t');
}

/**
 * @brief Run one test body and report its result.
 *
 * Resets the per-test counters, calls the body, then writes "@end" (for
 * lcrun) or a PASS / FAIL / SKIP / N/I line (for humans). A body that exits
 * or dies never reaches the report: lcrun judges it by the process
 * status against the test's `expect`.
 *
 * @param t Test to run.
 * @return 1 if a check failed, 0 otherwise (pass, skip, not implemented).
 */
static int	lcRunOne(const struct lc_test *t)
{
	const char	*status;

	gCnt->asserts = 0;
	gCnt->failed = 0;
	gCnt->skipped = 0;
	gCnt->unimpl = 0;
	t->fn();
	status = gCnt->failed ? "fail" : gCnt->unimpl ? "unimplemented" : gCnt->skipped ? "skip" : "pass";
	if (gProto)
	{
		lcs("@end\t");
		lcs(status);
		lcc('\t');
		lci(gCnt->asserts);
		lcc('\t');
		lci(gCnt->failed);
		lcc('\n');
	}
	else
	{
		lcs(gCnt->failed ? "  FAIL  " : gCnt->unimpl ? "  N/I   " : gCnt->skipped ? "  SKIP  " : "  PASS  ");
		lcs(t->category);
		lcc('/');
		lcs(t->function);
		lcc('/');
		lcs(t->name);
		lcs(" (");
		lci(gCnt->asserts);
		lcs(" checks)\n");
	}
	lcFlush();
	return (gCnt->failed != 0);
}

/**
 * @brief --list: one "@test" line per test of the binary, on fd 1.
 *
 * Absent optional fields are written as "-". REF is last, so it may be
 * the only field holding arbitrary text.
 */
static void	lcList(void)
{
	const struct lc_test	*t;

	gFd = 1;
	for (t = __start_lc_tests; t < __stop_lc_tests; t++)
	{
		lcs("@test\t");
		lcField(t->category);
		lcField(t->function);
		lcField(t->name);
		lcField(t->file);
		lci(t->line);
		lcc('\t');
		lci(t->timeout);
		lcc('\t');
		lci(t->expect);
		lcc('\t');
		lcField(t->since ? t->since : "-");
		lcField(t->until ? t->until : "-");
		lcField(t->opt ? t->opt : "-");
		gEscape = 1;
		lcs(t->ref ? t->ref : "-");
		gEscape = 0;
		lcc('\n');
	}
	lcFlush();
}

/**
 * @brief Run every test in this process, one after the other (no lcrun).
 *
 * For debugging only: no isolation, no timeout, no since / until / opt
 * filtering. Tests that expect an exit or a signal are skipped, since
 * they would end the process.
 *
 * @return 1 if a test failed, 0 otherwise.
 */
static int	lcRunAll(void)
{
	const struct lc_test	*t;
	int						total;
	int						failed;

	total = 0;
	failed = 0;
	for (t = __start_lc_tests; t < __stop_lc_tests; t++)
	{
		if (t->expect != LC_NORMAL)
		{
			lcs("  SKIP  ");
			lcs(t->name);
			lcs(" (expects an exit or a signal: run it through lcrun)\n");
			continue ;
		}
		total++;
		failed += lcRunOne(t);
	}
	lci(total);
	lcs(" tests, ");
	lci(failed);
	lcs(" failed\n");
	lcFlush();
	return (failed != 0);
}

/**
 * @brief Entry point of every test binary (see the command line above).
 *
 * fd 3 open at startup means "run by lcrun": events go there in the
 * machine protocol. After a test run with --run, the coverage counters
 * are written when the binary was built with COV=1 (see cov.c for the
 * tests that end in exit() or a signal).
 *
 * @param argc Argument count.
 * @param argv Arguments.
 * @return 0 when every test run passed or was skipped, 1 if one failed,
 *         2 for a usage error or an unknown test.
 */
int	main(int argc, char **argv)
{
	const struct lc_test	*t;

	gProto = (lc_sysFcntl(3, LC_F_GETFD, 0) >= 0);
	if (argc >= 2 && lcStreq(argv[1], "--list"))
	{
		lcList();
		return (0);
	}
	if (argc >= 4 && lcStreq(argv[1], "--run"))
	{
		if (gProto)
			gFd = 3;
		for (t = __start_lc_tests; t < __stop_lc_tests; t++)
		{
			int	failed;

			if (!lcStreq(t->function, argv[2]) || !lcStreq(t->name, argv[3]))
				continue ;
			if (lc_covArmSignal && (t->expect & ~0xff) == LC_SIGNAL(0))
				lc_covArmSignal(t->expect & 0xff);
			failed = lcRunOne(t);
			if (lc_covDump)
				lc_covDump();
			return (failed);
		}
		lcs("blasix: no test ");
		lcs(argv[2]);
		lcc('/');
		lcs(argv[3]);
		lcc('\n');
		lcFlush();
		return (2);
	}
	if (argc >= 2)
	{
		lcs("usage: ");
		lcs(argv[0]);
		lcs(" [--list | --run FUNCTION NAME]\n");
		lcFlush();
		return (2);
	}
	gProto = 0;
	return (lcRunAll());
}
