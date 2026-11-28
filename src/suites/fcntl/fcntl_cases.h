/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */
/**
 * @file fcntl_cases.h
 * @brief Shared helpers of the <fcntl.h> units.
 * @Created: 2026/11/28 11:13:34 by Moutig
 * @Updated: 2026/11/28 12:04:06 by Moutig
 *
 * The files and directories a case works on are made and checked with
 * raw syscalls (framework/sys.h), never with the function under test:
 *
 *   - fc_path: a fresh temporary path (nothing there yet);
 *   - fc_makeFile: a file at a path holding n bytes of fc_pattern;
 *   - fc_stat: mode, size and link count of a path or descriptor
 *     (statx(2));
 *   - fc_fdFlags, fc_flFlags: F_GETFD and F_GETFL of a descriptor, raw;
 *   - fc_lowestFree: the lowest descriptor not open (what open() and
 *     F_DUPFD must give);
 *   - fc_umask: set the creation mask, raw, and give the old one.
 *
 * The modes are numeric (0644): from POSIX.1-2008 the S_I* bits have
 * these values, and in POSIX.1-2001 they are XSI in <fcntl.h>. The Linux
 * values of the flags read back raw are the LC_O_* of framework/sys.h.
 * Only <fcntl.h> is under test; <errno.h> prepares the data.
 */

#ifndef FCNTL_CASES_H
# define FCNTL_CASES_H

# include "framework/lc.h"
# include "framework/sys.h"
# include <fcntl.h>
# include <errno.h>

/** File type and permission bits of the Linux st_mode. */
# define FC_IFMT		0170000
# define FC_IFREG		0100000
# define FC_IFDIR		0040000
# define FC_IFLNK		0120000
/** FD_CLOEXEC on Linux, read back raw by F_GETFD. */
# define FC_FD_CLOEXEC	1

/** Byte i of the content of the files the cases make. */
static inline unsigned char	fc_pattern(unsigned long i)
{
	return ((unsigned char)(i * 151u + (i >> 9) + 7u));
}

/* A temporary path for tag, with nothing there (a leftover is removed). */
static inline char	*fc_path(char *buf, unsigned long size, const char *tag)
{
	(void)lc_tmpPath(buf, size, tag);
	(void)lc_sysUnlink(buf);
	(void)lc_sysRmdir(buf);
	return (buf);
}

/* Make path a regular file of mode 0600 holding n bytes of fc_pattern. */
static inline int	fc_makeFile(const char *path, unsigned long n)
{
	unsigned char	b[256];
	long			fd = lc_sysOpen(path, LC_O_WRONLY | LC_O_CREAT | LC_O_TRUNC | LC_O_CLOEXEC, 0600);

	if (fd < 0)
		return (-1);
	for (unsigned long done = 0; done < n; )
	{
		unsigned long	k = n - done < sizeof(b) ? n - done : sizeof(b);

		for (unsigned long i = 0; i < k; i++)
			b[i] = fc_pattern(done + i);
		if (lc_sysWrite((int)fd, b, k) != (long)k)
			return ((void)lc_sysClose((int)fd), -1);
		done += k;
	}
	return ((int)lc_sysClose((int)fd));
}

/** What fc_stat reads. */
typedef struct fc_st
{
	unsigned int		mode;
	unsigned long long	size;
	unsigned int		nlink;
}	fc_st;

/*
 * The mode, size and link count of path (fd < 0) or of fd (path NULL),
 * without following a final symbolic link when nofollow.
 * @return 0, or -1.
 */
static inline int	fc_stat(int fd, const char *path, int nofollow, fc_st *st)
{
	union
	{
		unsigned long long	u64[32];
		unsigned int		u32[64];
		unsigned short		u16[128];
	}		raw;
	long	r;

	if (path)
		r = lc_sysStatx(LC_AT_FDCWD, path,
				nofollow ? LC_AT_SYMLINK_NOFOLLOW : 0, raw.u64);
	else
		r = lc_sysStatx(fd, "", LC_AT_EMPTY_PATH, raw.u64);
	if (r != 0)
		return (-1);
	st->mode = raw.u16[LC_STX_MODE / 2];
	st->size = raw.u64[LC_STX_SIZE / 8];
	st->nlink = raw.u32[LC_STX_NLINK / 4];
	return (0);
}

/* The permission bits (07777) of path, or -1. */
static inline int	fc_perm(const char *path)
{
	fc_st	st;

	return (fc_stat(-1, path, 0, &st) == 0 ? (int)(st.mode & 07777) : -1);
}

/* The size of the file of fd, or -1. */
static inline long	fc_size(int fd)
{
	fc_st	st;

	return (fc_stat(fd, 0, 0, &st) == 0 ? (long)st.size : -1);
}

/* F_GETFD and F_GETFL of fd, raw (-errno on error). */
static inline long	fc_fdFlags(int fd)
{
	return (lc_sysFcntl(fd, LC_F_GETFD, 0));
}

static inline long	fc_flFlags(int fd)
{
	return (lc_sysFcntl(fd, LC_F_GETFL, 0));
}

/* The lowest descriptor not open, from 0. */
static inline int	fc_lowestFree(void)
{
	int	fd = 0;

	while (lc_sysFcntl(fd, LC_F_GETFD, 0) >= 0)
		fd++;
	return (fd);
}

/* Set the file mode creation mask, raw; the old one is returned. */
static inline int	fc_umask(int mask)
{
	return ((int)lc_sysUmask(mask));
}

#endif
