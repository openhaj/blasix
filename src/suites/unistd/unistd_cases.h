/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file unistd_cases.h
 * @brief Shared helpers of the <unistd.h> units.
 * @Created: 2026/11/28 09:54:38 by Moutig
 * @Updated: 2026/11/28 09:56:15 by Moutig
 *
 * The files, pipes and descriptors a case works on are made and checked
 * with raw syscalls (framework/sys.h), never with the function under
 * test:
 *
 *   - ui_file: a temporary file holding given bytes, open with given
 *     flags (unlinked at once: it goes away with the process);
 *   - ui_pattern, ui_matches: the bytes of a file, without the libc;
 *   - ui_rawSize, ui_rawContents: what the file holds, read back raw;
 *   - ui_noReader, ui_nonBlock: a pipe without reader, a non-blocking fd;
 *   - UI_SIGPIPE, UI_SIG_DFL, UI_SIG_IGN: the Linux values (x86_64, aarch64).
 *
 * Only <unistd.h> is under test; <errno.h> prepares the data.
 */

#ifndef UNISTD_CASES_H
# define UNISTD_CASES_H

# include "framework/lc.h"
# include "framework/sys.h"
# include <unistd.h>
# include <errno.h>

/** SIGPIPE, and the "default" and "ignore" dispositions of sigaction(2). */
# define UI_SIGPIPE	13
# define UI_SIG_DFL	((void (*)(int))0)
# define UI_SIG_IGN	((void (*)(int))1)

/** Byte i of the content of the files the cases make. */
static inline unsigned char	ui_pattern(unsigned long i)
{
	return ((unsigned char)(i * 151u + (i >> 9) + 7u));
}

/* Do the n bytes of p hold ui_pattern(from ...)? */
static inline int	ui_matches(const void *p, unsigned long n, unsigned long from)
{
	const unsigned char	*b = p;

	for (unsigned long i = 0; i < n; i++)
		if (b[i] != ui_pattern(from + i))
			return (0);
	return (1);
}

/*
 * A new temporary file holding size bytes of the pattern, open with
 * flags (LC_O_RDWR, LC_O_RDONLY, ...), at offset 0, already unlinked.
 *
 * @return The descriptor, or -1.
 */
static inline int	ui_file(const char *tag, unsigned long size, int flags)
{
	static unsigned char	chunk[4096];
	char					path[128];
	long					w;
	long					fd;

	(void)lc_tmpPath(path, sizeof(path), tag);
	w = lc_sysOpen(path, LC_O_RDWR | LC_O_CREAT | LC_O_TRUNC | LC_O_CLOEXEC, 0600);
	if (w < 0)
		return (-1);
	for (unsigned long done = 0; done < size; )
	{
		unsigned long	n = size - done < sizeof(chunk) ? size - done : sizeof(chunk);

		for (unsigned long i = 0; i < n; i++)
			chunk[i] = ui_pattern(done + i);
		if (lc_sysWrite((int)w, chunk, n) != (long)n)
			return (-1);
		done += n;
	}
	fd = lc_sysOpen(path, flags | LC_O_CLOEXEC, 0);
	(void)lc_sysClose((int)w);
	(void)lc_sysUnlink(path);
	return ((int)fd);
}

/* The size of the file of fd (its end), without moving its offset for good. */
static inline long	ui_rawSize(int fd)
{
	long	at = lc_sysLseek(fd, 0, LC_SEEK_CUR);
	long	end = lc_sysLseek(fd, 0, LC_SEEK_END);

	(void)lc_sysLseek(fd, at, LC_SEEK_SET);
	return (end);
}

/* Read n bytes at offset off of the file of fd into buf, raw. */
static inline long	ui_rawContents(int fd, long off, void *buf, unsigned long n)
{
	long	at = lc_sysLseek(fd, 0, LC_SEEK_CUR);
	long	got = 0;
	long	r;

	(void)lc_sysLseek(fd, off, LC_SEEK_SET);
	while ((unsigned long)got < n && (r = lc_sysRead(fd, (char *)buf + got, n - (unsigned long)got)) > 0)
		got += r;
	(void)lc_sysLseek(fd, at, LC_SEEK_SET);
	return (got);
}

/* Set O_NONBLOCK on fd. */
static inline int	ui_nonBlock(int fd)
{
	long	fl = lc_sysFcntl(fd, LC_F_GETFL, 0);

	return (fl >= 0 && lc_sysFcntl(fd, LC_F_SETFL, fl | LC_O_NONBLOCK) == 0);
}

#endif
