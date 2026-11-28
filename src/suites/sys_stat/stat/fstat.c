/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file fstat.c
 * @brief Tests for fstat() (<sys/stat.h>).
 * @Created: 2026/11/28 12:24:30 by Moutig
 * @Updated: 2026/11/28 12:25:06 by Moutig
 *
 * fstat(fildes, buf) fills *buf with the information on the open file of
 * fildes: st_mode (file type and mode bits), st_ino and st_dev (the file
 * identity: the same for two descriptors of one file, different for two
 * files), st_nlink, st_uid, st_gid, st_size (for a regular file, its size
 * in bytes) and the timestamps; it returns 0. Error: EBADF (fildes not
 * open). Every value is checked against what statx(2) says, raw, and the
 * size against the bytes just written.
 *
 * Clauses: POSIX: XSH fstat(), XBD <sys/stat.h>.
 */
#include "sys_stat/sys_stat_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH fstat()")
{
	EXPECT_TYPE(&fstat, int (*)(int, struct stat *));
}

/* A regular file: 0, S_ISREG, and st_size the number of bytes written. */
TEST_OPT(regular_file_type_and_size, .ref = "XSH fstat()")
{
	char		p[128];
	struct stat	s;
	int			fd = ss_makeFile(ss_path(p, sizeof(p), "fs"), 123);

	(void)lc_sysUnlink(p);
	ASSERT_GE(fd, 0);
	EXPECT_EQ(fstat(fd, &s), 0);
	EXPECT_NE(S_ISREG(s.st_mode), 0);
	EXPECT_EQ((long)s.st_size, 123);
	(void)lc_sysClose(fd);
}

/* The size follows what write() has just added, and what truncation removed. */
TEST_OPT(size_follows_the_writes, .ref = "XSH fstat()")
{
	char		p[128];
	struct stat	s;
	int			fd = ss_makeFile(ss_path(p, sizeof(p), "fw"), 0);

	(void)lc_sysUnlink(p);
	ASSERT_GE(fd, 0);
	ASSERT_EQ(fstat(fd, &s), 0);
	EXPECT_EQ((long)s.st_size, 0);
	ASSERT_EQ(lc_sysWrite(fd, "0123456789", 10), 10);
	ASSERT_EQ(fstat(fd, &s), 0);
	EXPECT_EQ((long)s.st_size, 10);
	ASSERT_EQ(lc_sysLseek(fd, 4096, LC_SEEK_SET), 4096);
	ASSERT_EQ(lc_sysWrite(fd, "x", 1), 1);
	ASSERT_EQ(fstat(fd, &s), 0);
	EXPECT_EQ((long)s.st_size, 4097);
	(void)lc_sysClose(fd);
}

/* Mode, inode, links, owner, size, mtime: as the kernel gives them. */
TEST_OPT(same_as_the_kernel, .ref = "XSH fstat()")
{
	char		p[128];
	struct stat	s;
	ss_st		r;
	int			fd = ss_makeFile(ss_path(p, sizeof(p), "fk"), 77);

	ASSERT_GE(fd, 0);
	ASSERT_EQ(fstat(fd, &s), 0);
	ASSERT_EQ(ss_raw(fd, 0, 0, &r), 0);
	EXPECT_UEQ((unsigned long)s.st_mode, r.mode);
	EXPECT_UEQ((unsigned long long)s.st_ino, r.ino);
	EXPECT_UEQ((unsigned long)s.st_nlink, r.nlink);
	EXPECT_UEQ((unsigned long)s.st_uid, r.uid);
	EXPECT_UEQ((unsigned long)s.st_gid, r.gid);
	EXPECT_UEQ((unsigned long long)s.st_size, r.size);
	EXPECT_EQ((long long)s.st_mtime, r.mtimeSec);
	(void)lc_sysUnlink(p);
	(void)lc_sysClose(fd);
}

/* The mode bits are those the file was created with. */
TEST_OPT(mode_bits_of_the_file, .ref = "XSH fstat()")
{
	char		p[128];
	struct stat	s;
	int			fd = ss_makeFile(ss_path(p, sizeof(p), "fm"), 1);

	(void)lc_sysUnlink(p);
	ASSERT_GE(fd, 0);
	ASSERT_EQ(fstat(fd, &s), 0);
	EXPECT_EQ((long)(s.st_mode & 07777), 0600);
	(void)lc_sysClose(fd);
}

/* st_nlink: the number of links; 0 once the last name is removed. */
TEST_OPT(link_count, .ref = "XBD <sys/stat.h>")
{
	char		p[128];
	struct stat	s;
	int			fd = ss_makeFile(ss_path(p, sizeof(p), "fn"), 1);

	ASSERT_GE(fd, 0);
	ASSERT_EQ(fstat(fd, &s), 0);
	EXPECT_EQ((long)s.st_nlink, 1);
	(void)lc_sysUnlink(p);
	ASSERT_EQ(fstat(fd, &s), 0);
	EXPECT_EQ((long)s.st_nlink, 0);
	(void)lc_sysClose(fd);
}

/* st_dev and st_ino: one identity for one file, another for another file. */
TEST_OPT(file_identity, .ref = "XBD <sys/stat.h>")
{
	char		p[128];
	char		q[128];
	struct stat	a;
	struct stat	b;
	struct stat	c;
	int			f = ss_makeFile(ss_path(p, sizeof(p), "fi"), 1);
	int			g = (int)lc_sysOpen(p, LC_O_RDONLY | LC_O_CLOEXEC, 0);
	int			h = ss_makeFile(ss_path(q, sizeof(q), "fj"), 1);

	ASSERT_TRUE(f >= 0 && g >= 0 && h >= 0);
	ASSERT_EQ(fstat(f, &a), 0);
	ASSERT_EQ(fstat(g, &b), 0);
	ASSERT_EQ(fstat(h, &c), 0);
	EXPECT_TRUE(a.st_dev == b.st_dev && a.st_ino == b.st_ino);
	EXPECT_FALSE(a.st_dev == c.st_dev && a.st_ino == c.st_ino);
	(void)lc_sysUnlink(p);
	(void)lc_sysUnlink(q);
	(void)lc_sysClose(f);
	(void)lc_sysClose(g);
	(void)lc_sysClose(h);
}

/* Other file types: a directory, a pipe, a character device. */
TEST_OPT(other_file_types, .ref = "XSH fstat()")
{
	struct stat	s;
	int			fds[2];
	int			d = (int)lc_sysOpen("/", LC_O_RDONLY | LC_O_CLOEXEC, 0);
	int			n = (int)lc_sysOpen("/dev/null", LC_O_RDONLY | LC_O_CLOEXEC, 0);

	ASSERT_TRUE(d >= 0 && n >= 0);
	ASSERT_EQ(lc_sysPipe(fds, LC_O_CLOEXEC), 0);
	EXPECT_TRUE(fstat(d, &s) == 0 && S_ISDIR(s.st_mode));
	EXPECT_TRUE(fstat(fds[0], &s) == 0 && S_ISFIFO(s.st_mode));
	EXPECT_TRUE(fstat(n, &s) == 0 && S_ISCHR(s.st_mode));
	(void)lc_sysClose(d);
	(void)lc_sysClose(n);
	(void)lc_sysClose(fds[0]);
	(void)lc_sysClose(fds[1]);
}

/* st_mtim: the modification time with its nanoseconds, as the kernel gives it. */
TEST_OPT(modification_time_with_nanoseconds, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
	/* st_mtime is the macro st_mtim.tv_sec from 2008: st_mtim is there too. */
#ifdef st_mtime
	char		p[128];
	struct stat	s;
	ss_st		r;
	int			fd = ss_makeFile(ss_path(p, sizeof(p), "ft"), 5);

	(void)lc_sysUnlink(p);
	ASSERT_GE(fd, 0);
	ASSERT_EQ(fstat(fd, &s), 0);
	ASSERT_EQ(ss_raw(fd, 0, 0, &r), 0);
	EXPECT_EQ((long long)s.st_mtim.tv_sec, r.mtimeSec);
	EXPECT_EQ((long)s.st_mtim.tv_nsec, (long)r.mtimeNsec);
	(void)lc_sysClose(fd);
#else
	NOT_IMPLEMENTED("st_mtim (st_mtime is not a macro)");
#endif
}

/* A descriptor that is not open: -1 and EBADF. */
TEST_OPT(closed_descriptor_gives_ebadf, .ref = "XSH fstat()")
{
	struct stat	s;

	errno = 0;
	EXPECT_EQ(fstat(-1, &s), -1);
	EXPECT_EQ(errno, EBADF);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	struct stat	s;

	EXPECT_EQ(fstat(-1, &s), (fstat)(-1, &s));
}
