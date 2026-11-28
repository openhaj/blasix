/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file fcntl.c
 * @brief Tests for fcntl() (<fcntl.h>).
 * @Created: 2026/11/28 12:15:58 by Moutig
 * @Updated: 2026/11/28 12:16:18 by Moutig
 *
 * fcntl(fildes, cmd, ...):
 *   - F_DUPFD: the lowest descriptor not open that is >= arg, sharing the
 *     open file description, FD_CLOEXEC clear; F_DUPFD_CLOEXEC (2008):
 *     the same with FD_CLOEXEC set; arg negative: EINVAL;
 *   - F_GETFD, F_SETFD: the descriptor flags (FD_CLOEXEC);
 *   - F_GETFL: the file status flags and access mode; F_SETFL sets the
 *     status flags (O_APPEND, O_NONBLOCK), the access mode is not changed;
 *   - F_GETLK, F_SETLK: record locks owned by the process; a lock held by
 *     another process conflicts: F_SETLK fails with EACCES or EAGAIN,
 *     F_GETLK describes it (l_pid its owner); with no conflict F_GETLK sets
 *     l_type to F_UNLCK;
 *   - EBADF: fildes not open.
 *
 * Clauses: POSIX: XSH fcntl().
 */
#include "fcntl/fcntl_cases.h"

/* SEEK_SET for l_whence: XSI in the <fcntl.h> of POSIX.1-2001 (0 everywhere). */
#ifdef SEEK_SET
# define FX_SEEK_SET	SEEK_SET
#else
# define FX_SEEK_SET	0
#endif

/* A temporary file open for reading and writing, already unlinked. */
static int	fx_file(const char *tag, unsigned long n)
{
	char	p[128];
	long	fd;

	if (fc_makeFile(fc_path(p, sizeof(p), tag), n) != 0)
		return (-1);
	fd = lc_sysOpen(p, LC_O_RDWR | LC_O_CLOEXEC, 0);
	(void)lc_sysUnlink(p);
	return ((int)fd);
}

TEST_OPT(prototype, .ref = "XSH fcntl()")
{
	EXPECT_TYPE(&fcntl, int (*)(int, int, ...));
}

/* F_DUPFD: the lowest free descriptor >= arg. */
TEST_OPT(f_dupfd_gives_the_lowest_free_at_least_arg, .ref = "XSH fcntl()")
{
	int	fd = fx_file("fd", 1);
	int	low = fc_lowestFree();
	int	a;
	int	b;

	ASSERT_GE(fd, 0);
	a = fcntl(fd, F_DUPFD, 0);
	EXPECT_EQ(a, low);
	b = fcntl(fd, F_DUPFD, 40);
	EXPECT_EQ(b, 40);
	EXPECT_EQ(fcntl(fd, F_DUPFD, 40), 41);
	(void)lc_sysClose(41);
	(void)lc_sysClose(b);
	(void)lc_sysClose(a);
	(void)lc_sysClose(fd);
}

/* F_DUPFD: the same open file description (one offset), FD_CLOEXEC clear. */
TEST_OPT(f_dupfd_shares_the_description, .ref = "XSH fcntl()")
{
	int	fd = fx_file("fs", 30);
	int	d;

	ASSERT_GE(fd, 0);
	ASSERT_EQ(fc_fdFlags(fd) & FC_FD_CLOEXEC, FC_FD_CLOEXEC);
	d = fcntl(fd, F_DUPFD, 0);
	ASSERT_GE(d, 0);
	EXPECT_EQ(fc_fdFlags(d) & FC_FD_CLOEXEC, 0);
	(void)lc_sysLseek(fd, 17, LC_SEEK_SET);
	EXPECT_EQ(lc_sysLseek(d, 0, LC_SEEK_CUR), 17);
	(void)lc_sysClose(d);
	(void)lc_sysClose(fd);
}

/* F_DUPFD_CLOEXEC: as F_DUPFD, with FD_CLOEXEC set. */
TEST_OPT(f_dupfd_cloexec_sets_fd_cloexec, .since = "POSIX.1-2008", .ref = "XSH fcntl()")
{
#ifdef F_DUPFD_CLOEXEC
	int	fd = fx_file("fc", 1);
	int	d;

	ASSERT_GE(fd, 0);
	ASSERT_EQ(lc_sysFcntl(fd, 2 /* F_SETFD */, 0), 0);
	d = fcntl(fd, F_DUPFD_CLOEXEC, 50);
	EXPECT_EQ(d, 50);
	EXPECT_EQ(fc_fdFlags(d) & FC_FD_CLOEXEC, FC_FD_CLOEXEC);
	(void)lc_sysClose(d);
	(void)lc_sysClose(fd);
#else
	NOT_IMPLEMENTED("F_DUPFD_CLOEXEC");
#endif
}

/* F_DUPFD with a negative arg: EINVAL. */
TEST_OPT(f_dupfd_negative_arg_gives_einval, .ref = "XSH fcntl()")
{
	errno = 0;
	EXPECT_EQ(fcntl(0, F_DUPFD, -1), -1);
	EXPECT_EQ(errno, EINVAL);
}

/* F_SETFD and F_GETFD: FD_CLOEXEC set and cleared, and seen raw. */
TEST_OPT(f_setfd_and_f_getfd, .ref = "XSH fcntl()")
{
	int	fd = fx_file("fg", 1);

	ASSERT_GE(fd, 0);
	EXPECT_EQ(fcntl(fd, F_SETFD, 0), 0);
	EXPECT_EQ(fcntl(fd, F_GETFD) & FD_CLOEXEC, 0);
	EXPECT_EQ(fc_fdFlags(fd) & FC_FD_CLOEXEC, 0);
	EXPECT_NE(fcntl(fd, F_SETFD, FD_CLOEXEC), -1);
	EXPECT_EQ(fcntl(fd, F_GETFD) & FD_CLOEXEC, FD_CLOEXEC);
	EXPECT_EQ(fc_fdFlags(fd) & FC_FD_CLOEXEC, FC_FD_CLOEXEC);
	(void)lc_sysClose(fd);
}

/* F_GETFL: the access mode (under O_ACCMODE) and the status flags of the description. */
TEST_OPT(f_getfl_gives_the_access_mode_and_flags, .ref = "XSH fcntl()")
{
	int	fd = fx_file("fl", 1);
	int	fl;

	ASSERT_GE(fd, 0);
	fl = fcntl(fd, F_GETFL);
	EXPECT_EQ(fl & O_ACCMODE, O_RDWR);
	EXPECT_EQ(fl & O_APPEND, 0);
	EXPECT_EQ(fl & O_NONBLOCK, 0);
	ASSERT_EQ(lc_sysFcntl(fd, LC_F_SETFL, LC_O_APPEND), 0);
	EXPECT_NE(fcntl(fd, F_GETFL) & O_APPEND, 0);
	(void)lc_sysClose(fd);
}

/* F_SETFL: O_APPEND and O_NONBLOCK are set, the access mode is kept. */
TEST_OPT(f_setfl_sets_status_flags_not_the_access_mode, .ref = "XSH fcntl()")
{
	int	fd = fx_file("ff", 1);
	long	raw;

	ASSERT_GE(fd, 0);
	EXPECT_NE(fcntl(fd, F_SETFL, O_APPEND | O_NONBLOCK | O_WRONLY), -1);
	raw = fc_flFlags(fd);
	EXPECT_NE(raw & LC_O_APPEND, 0);
	EXPECT_NE(raw & LC_O_NONBLOCK, 0);
	EXPECT_EQ(raw & 3, LC_O_RDWR);
	EXPECT_NE(fcntl(fd, F_SETFL, 0), -1);
	EXPECT_EQ(fc_flFlags(fd) & (LC_O_APPEND | LC_O_NONBLOCK), 0);
	(void)lc_sysClose(fd);
}

/* F_GETLK with no conflicting lock: l_type becomes F_UNLCK. */
TEST_OPT(f_getlk_without_conflict_gives_f_unlck, .ref = "XSH fcntl()")
{
	int				fd = fx_file("lk", 10);
	struct flock	l;

	ASSERT_GE(fd, 0);
	l.l_type = F_WRLCK;
	l.l_whence = FX_SEEK_SET;
	l.l_start = 0;
	l.l_len = 0;
	l.l_pid = 0;
	EXPECT_NE(fcntl(fd, F_GETLK, &l), -1);
	EXPECT_EQ(l.l_type, F_UNLCK);
	(void)lc_sysClose(fd);
}

static int	g_lockFd = -1;
static long	g_lockOwner;

/* In another process: the lock of the parent conflicts. */
static void	fx_otherProcess(void *arg)
{
	struct flock	l;

	(void)arg;
	l.l_type = F_WRLCK;
	l.l_whence = FX_SEEK_SET;
	l.l_start = 2;
	l.l_len = 3;
	l.l_pid = 0;
	errno = 0;
	EXPECT_EQ(fcntl(g_lockFd, F_SETLK, &l), -1);
	EXPECT_TRUE(errno == EACCES || errno == EAGAIN);
	l.l_type = F_RDLCK;
	EXPECT_NE(fcntl(g_lockFd, F_GETLK, &l), -1);
	EXPECT_EQ(l.l_type, F_WRLCK);
	EXPECT_EQ((long)l.l_pid, g_lockOwner);
	l.l_type = F_WRLCK;
	l.l_start = 6;
	l.l_len = 1;
	EXPECT_NE(fcntl(g_lockFd, F_SETLK, &l), -1);
}

/* F_SETLK: a write lock held here makes another process fail (EACCES or EAGAIN) and is seen by its F_GETLK. */
TEST_OPT(f_setlk_conflicts_with_another_process, .ref = "XSH fcntl()")
{
	struct flock	l;

	g_lockFd = fx_file("ls", 10);
	g_lockOwner = lc_sysGetpid();
	ASSERT_GE(g_lockFd, 0);
	l.l_type = F_WRLCK;
	l.l_whence = FX_SEEK_SET;
	l.l_start = 0;
	l.l_len = 5;
	l.l_pid = 0;
	ASSERT_NE(fcntl(g_lockFd, F_SETLK, &l), -1);
	EXPECT_EQ(lc_child(fx_otherProcess, 0), LC_EXIT(0));
	l.l_type = F_UNLCK;
	EXPECT_NE(fcntl(g_lockFd, F_SETLK, &l), -1);
	(void)lc_sysClose(g_lockFd);
}

/* A descriptor that is not open: EBADF. */
TEST_OPT(closed_descriptor_gives_ebadf, .ref = "XSH fcntl()")
{
	int	fd = fc_lowestFree() + 100;

	errno = 0;
	EXPECT_EQ(fcntl(fd, F_GETFD), -1);
	EXPECT_EQ(errno, EBADF);
	errno = 0;
	EXPECT_EQ(fcntl(-1, F_GETFL), -1);
	EXPECT_EQ(errno, EBADF);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(fcntl(-1, F_GETFD), (fcntl)(-1, F_GETFD));
}
