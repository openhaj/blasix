/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file standards.c
 * @brief The 8 standards, the option groups and the target.
 * @Created: 2026/11/24 08:16:02 by Moutig
 * @Updated: 2026/11/24 08:22:12 by Moutig
 *
 * Standards. The 4 ISO C editions and the 4 POSIX.1 editions form two
 * ladders; a POSIX edition is built on an ISO C edition (its "C base").
 * A target (t_target) is one standard, the option groups turned on and
 * whether extensions are included; it gives the compiler flags of the
 * tests (-std=, _POSIX_C_SOURCE, _XOPEN_SOURCE, want macros).
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/**
 * The 8 standards, ISO C ladder first: id, name, ISO C level (of the C
 * base for POSIX), POSIX level, _POSIX_C_SOURCE, _XOPEN_SOURCE, -std=.
 * C23 compiles with -std=c2x, accepted by GCC and clang before C23 had
 * its final name.
 */
static const t_std	gStds[] = {
	{"c99", "C99", 1, 0, 0, 0, "c99"},
	{"c11", "C11", 2, 0, 0, 0, "c11"},
	{"c17", "C17", 3, 0, 0, 0, "c17"},
	{"c23", "C23", 4, 0, 0, 0, "c2x"},
	{"posix-2001", "POSIX.1-2001", 1, 1, 200112L, 600, "c99"},
	{"posix-2008", "POSIX.1-2008", 1, 2, 200809L, 700, "c99"},
	{"posix-2017", "POSIX.1-2017", 1, 3, 200809L, 700, "c99"},
	{"posix-2024", "POSIX.1-2024", 3, 4, 202405L, 800, "c17"},
};

/** Number of standards. */
#define N_STDS	(sizeof(gStds) / sizeof(gStds[0]))


/**
 * Every option group a spec line or a test may name: name, `from`
 * (ISO C edition where the feature becomes conditional; NULL for a
 * POSIX option), macro to define when selected, description.
 * CX (extension to ISO C) and OB (obsolescent) are margin codes too,
 * but not options: CX is base POSIX, OB is the ob= tag.
 */
static const t_optdef	gOpts[] = {
	/* ISO C conditional features. */
	{"COMPLEX", "C11", NULL, "Complex arithmetic <complex.h> (__STDC_NO_COMPLEX__)"},
	{"THREADS", "C11", NULL, "Threads <threads.h> (__STDC_NO_THREADS__)"},
	{"ATOMICS", "C11", NULL, "Atomics <stdatomic.h> (__STDC_NO_ATOMICS__)"},
	{"LIB_EXT1", "C11", "__STDC_WANT_LIB_EXT1__=1",
		"Bounds-checking interfaces, Annex K (__STDC_LIB_EXT1__)"},
	{"IEC_60559", "C99", NULL, "IEC 60559 floating point, Annex F (__STDC_IEC_559__)"},
	{"DFP", "C23", NULL, "Decimal floating point (__STDC_IEC_60559_DFP__)"},
	{"IEC_60559_TYPES", "C23", "__STDC_WANT_IEC_60559_TYPES_EXT__=1",
		"Interchange and extended types, Annex H (__STDC_IEC_60559_TYPES__)"},
	/* POSIX margin codes (XBD 1.7.1) that mark library interfaces. */
	{"ADV", NULL, NULL, "Advisory Information"},
	{"AIO", NULL, NULL, "Asynchronous Input and Output"},
	{"BAR", NULL, NULL, "Barriers"},
	{"CPT", NULL, NULL, "Process CPU-Time Clocks"},
	{"CS", NULL, NULL, "Clock Selection"},
	{"DC", NULL, NULL, "Device Control"},
	{"FSC", NULL, NULL, "File Synchronization"},
	{"IP6", NULL, NULL, "IPV6"},
	{"MC1", NULL, NULL, "Advisory Information and Memory Mapped Files or Shared Memory Objects"},
	{"MC2", NULL, NULL, "Memory Mapped Files, Shared Memory Objects, or Memory Protection"},
	{"MC3", NULL, NULL, "Memory Mapped Files, Shared Memory Objects, or Typed Memory Objects"},
	{"MF", NULL, NULL, "Memory Mapped Files"},
	{"ML", NULL, NULL, "Process Memory Locking"},
	{"MLR", NULL, NULL, "Range Memory Locking"},
	{"MON", NULL, NULL, "Monotonic Clock"},
	{"MPR", NULL, NULL, "Memory Protection"},
	{"MSG", NULL, NULL, "Message Passing"},
	{"MX", NULL, NULL, "IEC 60559 Floating-Point"},
	{"MXC", NULL, NULL, "IEC 60559 Complex Floating-Point"},
	{"MXX", NULL, NULL, "IEC 60559 Floating-Point Extension"},
	{"PIO", NULL, NULL, "Prioritized Input and Output"},
	{"PS", NULL, NULL, "Process Scheduling"},
	{"RPI", NULL, NULL, "Robust Mutex Priority Inheritance"},
	{"RPP", NULL, NULL, "Robust Mutex Priority Protection"},
	{"RS", NULL, NULL, "Raw Sockets"},
	{"RTS", NULL, NULL, "Realtime Signals Extension"},
	{"SEM", NULL, NULL, "Semaphores"},
	{"SHM", NULL, NULL, "Shared Memory Objects"},
	{"SIO", NULL, NULL, "Synchronized Input and Output"},
	{"SPI", NULL, NULL, "Spin Locks"},
	{"SPN", NULL, NULL, "Spawn"},
	{"SS", NULL, NULL, "Process Sporadic Server"},
	{"TCT", NULL, NULL, "Thread CPU-Time Clocks"},
	{"TEF", NULL, NULL, "Trace Event Filter"},
	{"THR", NULL, NULL, "Threads"},
	{"TMO", NULL, NULL, "Timeouts"},
	{"TMR", NULL, NULL, "Timers"},
	{"TPI", NULL, NULL, "Non-Robust Mutex Priority Inheritance"},
	{"TPP", NULL, NULL, "Non-Robust Mutex Priority Protection"},
	{"TPS", NULL, NULL, "Thread Execution Scheduling"},
	{"TRC", NULL, NULL, "Trace"},
	{"TRI", NULL, NULL, "Trace Inherit"},
	{"TRL", NULL, NULL, "Trace Log"},
	{"TSA", NULL, NULL, "Thread Stack Address Attribute"},
	{"TSF", NULL, NULL, "Thread-Safe Functions"},
	{"TSH", NULL, NULL, "Thread Process-Shared Synchronization"},
	{"TSP", NULL, NULL, "Thread Sporadic Server"},
	{"TSS", NULL, NULL, "Thread Stack Size Attribute"},
	{"TYM", NULL, NULL, "Typed Memory Objects"},
	{"XSI", NULL, NULL, "X/Open System Interfaces"},
	/* XSI option groups (XBD 2.1.5.2) that mark library interfaces. */
	{"CRYPT", NULL, NULL, "X/Open Encryption Option Group (with XSI): crypt, encrypt, setkey"},
	{"XSR", NULL, NULL, "XSI STREAMS"},
};

/** Number of option groups. */
#define N_OPTS	(sizeof(gOpts) / sizeof(gOpts[0]))

const t_std	*stdFind(const char *s)
{
	for (size_t i = 0; i < N_STDS; i++)
		if (!strcasecmp(s, gStds[i].id) || !strcasecmp(s, gStds[i].name))
			return (&gStds[i]);
	return (NULL);
}

const t_std	*stdAt(size_t i)
{
	return (i < N_STDS ? &gStds[i] : NULL);
}

int	stdValid(const char *v)
{
	return (!strcmp(v, "EXT") || stdFind(v) != NULL);
}

void	stdPrintAll(FILE *fp)
{
	for (size_t i = 0; i < N_STDS; i++)
		fprintf(fp, "  %-11s %-13s %s\n", gStds[i].id, gStds[i].name,
			gStds[i].posix ? "POSIX (its C base + system interfaces)" : "ISO C");
}

int	stdIncludes(const t_target *t, const char *v)
{
	const t_std	*s;

	if (v == NULL || *v == '\0')
		return (1);
	if (!strcmp(v, "EXT"))
		return (t->ext);
	s = stdFind(v);
	if (s == NULL)
		return (0);
	if (s->posix == 0)
		return (s->c <= t->std->c);
	return (t->std->posix != 0 && s->posix <= t->std->posix);
}

int	stdIncludesAny(const t_target *t, const char *list)
{
	char	buf[256];
	char	*save;

	if (list == NULL)
		return (0);
	snprintf(buf, sizeof(buf), "%s", list);
	for (char *v = strtok_r(buf, ",", &save); v; v = strtok_r(NULL, ",", &save))
		if (stdIncludes(t, v))
			return (1);
	return (0);
}

const t_optdef	*optFind(const char *name)
{
	for (size_t i = 0; i < N_OPTS; i++)
		if (!strcasecmp(gOpts[i].name, name))
			return (&gOpts[i]);
	return (NULL);
}

const t_optdef	*optAt(size_t i)
{
	return (i < N_OPTS ? &gOpts[i] : NULL);
}

int	targetHasOpt(const t_target *t, const char *opt)
{
	for (char **o = t->opts; o && *o; o++)
		if (!strcasecmp(*o, opt))
			return (1);
	return (0);
}

int	targetCheck(const t_target *t)
{
	for (char **o = t->opts; o && *o; o++)
	{
		const t_optdef	*d = optFind(*o);

		if (d == NULL)
		{
			fprintf(stderr, "lcrun: unknown option group '%s' (make options)\n", *o);
			return (-1);
		}
		if (d->from == NULL && t->std->posix == 0)
		{
			fprintf(stderr, "lcrun: %s is a POSIX option, %s is an ISO C target\n",
				d->name, t->std->name);
			return (-1);
		}
	}
	return (0);
}

/**
 * @brief Append a formatted string to @p buf (truncated to @p size).
 *
 * @param buf  NUL-terminated buffer.
 * @param size Size of @p buf.
 * @param fmt  Format with one %s.
 * @param s    String.
 */
static void	appendStr(char *buf, size_t size, const char *fmt, const char *s)
{
	size_t	len = strlen(buf);

	snprintf(buf + len, size - len, fmt, s);
}

void	targetCflags(const t_target *t, char *buf, size_t size)
{
	size_t	len;

	snprintf(buf, size, "-std=%s", t->std->cstd);
	if (t->std->posix)
	{
		len = strlen(buf);
		snprintf(buf + len, size - len, " -D_POSIX_C_SOURCE=%ldL", t->std->posixMacro);
		if (targetHasOpt(t, "XSI"))
		{
			len = strlen(buf);
			snprintf(buf + len, size - len, " -D_XOPEN_SOURCE=%d", t->std->xopen);
		}
	}
	for (char **o = t->opts; o && *o; o++)
	{
		const t_optdef	*d = optFind(*o);

		if (d && d->want)
			appendStr(buf, size, " -D%s", d->want);
	}
}

void	targetDescribe(const t_target *t, char *buf, size_t size)
{
	snprintf(buf, size, "%s", t->std->name);
	for (char **o = t->opts; o && *o; o++)
		appendStr(buf, size, " + %s", *o);
	if (t->ext)
		appendStr(buf, size, "%s", " + extensions");
}
