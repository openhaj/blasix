/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file cov.c
 * @brief Coverage dump without the libgcov runtime (COV=1 only).
 * @Created: 2026/11/23 04:42:37 by Moutig
 * @Updated: 2026/11/24 00:38:00 by Moutig
 *
 * The system libgcov is built against glibc (__fprintf_chk,
 * __errno_location, ...) and cannot link with another libc. Instead:
 *
 *   - the libc under test is compiled with
 *     -fprofile-arcs -ftest-coverage -fprofile-info-section=gcov_info,
 *     which puts a pointer to every object's gcov_info in the
 *     "gcov_info" section and does not register any constructor;
 *   - at the end of each test, lc_covDump() walks that section and
 *     writes the .gcda files itself (raw syscalls) through
 *     __gcov_info_to_gcda, the only libgcov member linked (it needs
 *     abort, mmap and strlen from the libc);
 *   - each process writes under LC_COV_DIR/<pid>/<original path>,
 *     scripts/cov-report.sh merges them with gcov-tool.
 *
 * Tests that end in exit() or a signal do not dump.
 *
 * __gcov_info_to_gcda calls back into this file: onFilename() once per
 * object (opens its .gcda), onDump() for each chunk of data, and
 * onAllocate() for its scratch memory.
 */

#define LC_FRAMEWORK
#include "framework/lc.h"
#include "framework/sys.h"

#ifndef LC_COV_DIR
# error "LC_COV_DIR is set by the Makefile in coverage builds"
# define LC_COV_DIR "default"
#endif

/** Opaque: only libgcov knows its layout. */
struct gcov_info;

/*
 * Bounds of the "gcov_info" section (pointers to the gcov_info of every
 * instrumented object). Weak: empty when nothing is instrumented.
 */
extern const struct gcov_info *const	__start_gcov_info[] __attribute__((weak));
extern const struct gcov_info *const	__stop_gcov_info[] __attribute__((weak));

/**
 * @brief Serialize one gcov_info as .gcda data (libgcov, GCC >= 12).
 *
 * @param info     Counters of one object file.
 * @param filename Called once with the path of the .gcda.
 * @param dump     Called with each chunk of the .gcda contents.
 * @param allocate Called for the memory libgcov needs.
 * @param arg      Passed back to the three callbacks.
 */
void	__gcov_info_to_gcda(const struct gcov_info *info,
			void (*filename)(const char *, void *),
			void (*dump)(const void *, unsigned, void *),
			void *(*allocate)(unsigned, void *),
			void *arg);

/*
 * Merge functions are referenced by every gcov_info but only called
 * when libgcov reads back an existing .gcda, which never happens here.
 * Defining them keeps libgcov's _gcov.o (and its glibc dependencies)
 * out of the link.
 */
extern void	__gcov_merge_add(void *counters, unsigned n);

void	__gcov_merge_add(void *counters, unsigned n)
{
	(void)counters;
	(void)n;
}

/** "LC_COV_DIR/<pid>": directory of this process's .gcda files. */
static char	gPrefix[256];

/**
 * @brief Build gPrefix for the current process.
 *
 * One directory per process, so that tests running in parallel never
 * write the same .gcda; cov-report.sh merges them afterwards.
 */
static void	covPrefix(void)
{
	char	tag[24];
	long	pid = lc_sysGetpid();
	int		k = 0;

	do
		tag[k++] = (char)('0' + pid % 10);
	while ((pid /= 10) != 0);
	lc_size_t	i = 0;
	for (const char *s = LC_COV_DIR; *s && i < sizeof(gPrefix) - 32; s++)
		gPrefix[i++] = *s;
	gPrefix[i++] = '/';
	while (k > 0)
		gPrefix[i++] = tag[--k];
	gPrefix[i] = '\0';
}

/**
 * @brief Create every parent directory of @p path (mkdir -p of dirname).
 *
 * Existing directories are fine (EEXIST is ignored).
 *
 * @param path Path of a file; modified while walking, restored after.
 */
static void	mkParents(char *path)
{
	for (char *p = path + 1; *p; p++)
	{
		if (*p != '/')
			continue ;
		*p = '\0';
		lc_sysMkdir(path, 0755);
		*p = '/';
	}
}

/**
 * @brief libgcov callback: open the .gcda of one object.
 *
 * The .gcda goes to gPrefix followed by its original absolute path, so
 * cov-report.sh can put it back next to the object's .gcno.
 *
 * @param name Original path of the .gcda (as compiled in the object).
 * @param arg  Pointer to a long: receives the file descriptor, or a
 *             negative value if the file cannot be opened.
 */
static void	onFilename(const char *name, void *arg)
{
	char		path[1024];
	lc_size_t	i = 0;
	long		*fd = arg;

	for (const char *s = gPrefix; *s && i < sizeof(path) - 1; s++)
		path[i++] = *s;
	if (name[0] != '/' && i < sizeof(path) - 1)
		path[i++] = '/';
	for (const char *s = name; *s && i < sizeof(path) - 1; s++)
		path[i++] = *s;
	path[i] = '\0';
	mkParents(path);
	*fd = lc_sysOpen(path, LC_O_WRONLY | LC_O_CREAT | LC_O_TRUNC, 0644);
}

/**
 * @brief libgcov callback: append data to the open .gcda.
 *
 * @param data Bytes to write.
 * @param len  Number of bytes.
 * @param arg  Pointer to the long file descriptor set by onFilename().
 */
static void	onDump(const void *data, unsigned len, void *arg)
{
	long	fd = *(long *)arg;

	if (fd >= 0)
		lc_sysWrite((int)fd, data, len);
}

/**
 * @brief libgcov callback: memory for libgcov (never freed: the process
 *        is about to end).
 *
 * @param len Size in bytes.
 * @param arg Unused.
 * @return Anonymous mapping of @p len bytes, or NULL.
 */
static void	*onAllocate(unsigned len, void *arg)
{
	(void)arg;
	return (lc_sysMmap(len));
}

void	lc_covDump(void)
{
	long	fd;

	covPrefix();
	for (const struct gcov_info *const *p = __start_gcov_info;
		p < __stop_gcov_info; p++)
	{
		fd = -1;
		__gcov_info_to_gcda(*p, onFilename, onDump, onAllocate, &fd);
		if (fd >= 0)
			lc_sysClose((int)fd);
	}
}
