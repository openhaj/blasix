/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file main.c
 * @brief lcrun: entry point, one command per invocation.
 * @Created: 2026/11/24 08:25:41 by Moutig
 * @Updated: 2026/11/24 08:26:17 by Moutig
 *
 * Parses the command line (cli.c), loads the spec database (spec/), then
 * runs the command: a spec tool (tools/), or a run: discover the units
 * and their tests (run/discover.c), run them (run/exec.c), report
 * (report/). The exit status says whether everything passed.
 */

#include "runner/lcrun.h"

#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief Entry point: parse, load the spec, then do the command.
 *
 * @param argc Argument count.
 * @param argv Arguments.
 * @return 0 when everything passed (or the command succeeded), 1 when a
 *         test did not pass, a .since / .until / .opt is unknown, a
 *         translation unit that must be refused compiled, or (strict mode)
 *         a header leaks a name it must hide, 2 on a usage or system error.
 */
int	main(int argc, char **argv)
{
	t_run		r;
	t_mode		mode;
	const char	*probeOut = NULL;
	int			bad;

	if (argc == 4 && !strcmp(argv[1], "--compare"))
		return (compareTsv(argv[2], argv[3], isatty(1)));
	if (argc == 2 && !strcmp(argv[1], "--standards"))
		return (stdPrintAll(stdout), 0);
	if (argc == 2 && !strcmp(argv[1], "--options"))
		return (toolOptions());
	memset(&r, 0, sizeof(r));
	if (parseOpts(&r.o, argc, argv, &mode, &probeOut) < 0)
		return (2);
	if (specLoad(&r.spec, r.o.specDir) < 0)
	{
		fprintf(stderr, "lcrun: %s: %s\n", r.o.specDir, strerror(errno));
		return (2);
	}
	if (targetCheck(&r.o.target) < 0)
		return (2);
	if (mode == MODE_CHECK)
		return (toolCheck(&r));
	if (mode == MODE_MATRIX)
		return (toolMatrix(&r));
	if (mode == MODE_TODO)
		return (toolTodo(&r));
	if (mode == MODE_LIST)
		return (toolList(&r));
	if (mode == MODE_PROBES)
		return (toolProbes(&r, probeOut));
	if (mode == MODE_MAKE)
	{
		discoverUnits(&r, 0);
		return (toolMake(&r));
	}
	discover(&r);
	if (r.o.listOnly)
	{
		for (size_t i = 0; i < r.nTests; i++)
			printf("%s/%s/%s\t%s:%d%s%s\n", r.tests[i].cat, r.tests[i].func,
				r.tests[i].name, r.tests[i].file, r.tests[i].line,
				r.tests[i].ref ? "\t" : "", r.tests[i].ref ? r.tests[i].ref : "");
		return (0);
	}
	r.wallMs = nowMs();
	runAll(&r);
	r.wallMs = nowMs() - r.wallMs;
	reportConsole(&r);
	if (r.o.tsvOut && reportTsv(&r, r.o.tsvOut) < 0)
		perror(r.o.tsvOut);
	if (r.o.jsonOut && reportJson(&r, r.o.jsonOut) < 0)
		perror(r.o.jsonOut);
	if (r.nBadVersion)
		fprintf(stderr, "lcrun: %zu test%s not run: unknown .since / .until / .opt "
			"(see above)\n", r.nBadVersion, r.nBadVersion == 1 ? "" : "s");
	bad = r.nBadVersion > 0;
	for (size_t i = 0; i < r.nTests; i++)
		if (r.tests[i].st != ST_PASS && r.tests[i].st != ST_SKIP)
			bad = 1;
	for (size_t i = 0; i < r.spec.n; i++)
		if (r.hidden[i] == HID_LEAKED && inList(r.o.cats, r.spec.v[i].cat)
			&& inList(r.o.funcs, r.spec.v[i].name))
			bad = 1;
	for (size_t i = 0; i < r.nRejects; i++)
		if (r.rejects[i].st == REJ_ACCEPTED && inList(r.o.cats, r.rejects[i].cat))
			bad = 1;
	return (bad);
}
