/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file open.c
 * @brief Tests for open() (<fcntl.h>).
 * @Created: 2026/11/28 12:04:24 by Moutig
 * @Updated: 2026/11/28 12:11:05 by Moutig
 *
 * open(path, oflag, ...) opens a file description for path and returns
 * the lowest numbered descriptor not open, at offset 0, with FD_CLOEXEC
 * clear unless O_CLOEXEC. Exactly one access mode: O_RDONLY, O_WRONLY,
 * O_RDWR. O_CREAT: a missing file is created as a regular file with the
 * mode bits of the third argument, cleared by the umask; an existing one
 * is kept. O_CREAT | O_EXCL: fails with EEXIST when path exists, even as
 * a symbolic link (not followed). O_TRUNC: a regular file opened for
 * writing is truncated to 0. O_APPEND: each write goes to the end.
 * POSIX.1-2008: O_CLOEXEC, O_DIRECTORY (ENOTDIR for another file),
 * O_NOFOLLOW (ELOOP on a final symbolic link). Errors: ENOENT (no such
 * file without O_CREAT, or an empty path), ENOTDIR (a prefix is not a
 * directory), EISDIR (a directory opened for writing).
 *
 * Clauses: POSIX: XSH open().
 */
#include "fcntl/fcntl_cases.h"

TEST_OPT(prototype, .ref = "XSH open()")
{
	EXPECT_TYPE(&open, int (*)(const char *, int, ...));
}

/* An existing file: the lowest free descriptor, at offset 0, reading what the file holds. */
TEST_OPT(gives_the_lowest_free_descriptor_at_offset_0, .ref = "XSH open()")
{
	char			p[128];
	unsigned char	b[40];
	int				want;
	int				fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "op"), 40), 0);
	want = fc_lowestFree();
	fd = open(p, O_RDONLY);
	EXPECT_EQ(fd, want);
	ASSERT_GE(fd, 0);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 0);
	EXPECT_EQ(lc_sysRead(fd, b, sizeof(b)), 40);
	EXPECT_EQ(b[0], fc_pattern(0));
	EXPECT_EQ(b[39], fc_pattern(39));
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* The access mode is that of the open file description: read only, write only, both. */
TEST_OPT(access_modes, .ref = "XSH open()")
{
	char	p[128];
	char	c = 'x';
	int		r;
	int		w;
	int		rw;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "om"), 4), 0);
	r = open(p, O_RDONLY);
	w = open(p, O_WRONLY);
	rw = open(p, O_RDWR);
	ASSERT_TRUE(r >= 0 && w >= 0 && rw >= 0);
	EXPECT_EQ(fc_flFlags(r) & 3, LC_O_RDONLY);
	EXPECT_EQ(fc_flFlags(w) & 3, LC_O_WRONLY);
	EXPECT_EQ(fc_flFlags(rw) & 3, LC_O_RDWR);
	EXPECT_LT(lc_sysWrite(r, &c, 1), 0);
	EXPECT_LT(lc_sysRead(w, &c, 1), 0);
	EXPECT_EQ(lc_sysRead(rw, &c, 1), 1);
	EXPECT_EQ(lc_sysWrite(rw, &c, 1), 1);
	(void)lc_sysClose(r);
	(void)lc_sysClose(w);
	(void)lc_sysClose(rw);
	(void)lc_sysUnlink(p);
}

/* O_CREAT: a regular file, empty, with the mode given less the umask bits. */
TEST_OPT(o_creat_makes_a_file_with_the_mode_less_the_umask, .ref = "XSH open()")
{
	char	p[128];
	int		old = fc_umask(027);
	int		fd = open(fc_path(p, sizeof(p), "oc"), O_WRONLY | O_CREAT, 0666);
	fc_st	st;

	(void)fc_umask(old);
	ASSERT_GE(fd, 0);
	ASSERT_EQ(fc_stat(-1, p, 1, &st), 0);
	EXPECT_EQ(st.mode & FC_IFMT, FC_IFREG);
	EXPECT_EQ(st.mode & 07777, 0640);
	EXPECT_EQ(st.size, 0);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* O_CREAT on an existing file: the file, its content and its mode are kept. */
TEST_OPT(o_creat_keeps_an_existing_file, .ref = "XSH open()")
{
	char	p[128];
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "ok"), 10), 0);
	fd = open(p, O_RDWR | O_CREAT, 0777);
	ASSERT_GE(fd, 0);
	EXPECT_EQ(fc_size(fd), 10);
	EXPECT_EQ(fc_perm(p), 0600);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* O_CREAT | O_EXCL on an existing file: EEXIST. */
TEST_OPT(o_excl_on_an_existing_file_gives_eexist, .ref = "XSH open()")
{
	char	p[128];

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "ox"), 3), 0);
	errno = 0;
	EXPECT_EQ(open(p, O_WRONLY | O_CREAT | O_EXCL, 0600), -1);
	EXPECT_EQ(errno, EEXIST);
	EXPECT_EQ(fc_perm(p), 0600);
	(void)lc_sysUnlink(p);
}

/* O_CREAT | O_EXCL: a symbolic link is not followed, even a dangling one. */
TEST_OPT(o_excl_does_not_follow_a_symbolic_link, .ref = "XSH open()")
{
	char	p[128];
	char	t[128];

	(void)fc_path(t, sizeof(t), "oxt");
	ASSERT_EQ(lc_sysSymlink(t, fc_path(p, sizeof(p), "oxl")), 0);
	errno = 0;
	EXPECT_EQ(open(p, O_WRONLY | O_CREAT | O_EXCL, 0600), -1);
	EXPECT_EQ(errno, EEXIST);
	EXPECT_EQ(fc_perm(t), -1);
	(void)lc_sysUnlink(p);
	(void)lc_sysUnlink(t);
}

/* O_CREAT | O_EXCL on a missing file: created. */
TEST_OPT(o_excl_on_a_missing_file_creates_it, .ref = "XSH open()")
{
	char	p[128];
	int		fd = open(fc_path(p, sizeof(p), "oy"), O_WRONLY | O_CREAT | O_EXCL, 0600);

	ASSERT_GE(fd, 0);
	EXPECT_EQ(fc_perm(p), 0600);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* O_TRUNC: a regular file opened for writing is emptied. */
TEST_OPT(o_trunc_empties_the_file, .ref = "XSH open()")
{
	char	p[128];
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "ot"), 100), 0);
	fd = open(p, O_RDWR | O_TRUNC);
	ASSERT_GE(fd, 0);
	EXPECT_EQ(fc_size(fd), 0);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* O_APPEND: every write goes to the end of the file, wherever the offset is. */
TEST_OPT(o_append_writes_at_the_end, .ref = "XSH open()")
{
	char	p[128];
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "oa"), 20), 0);
	fd = open(p, O_WRONLY | O_APPEND);
	ASSERT_GE(fd, 0);
	EXPECT_NE(fc_flFlags(fd) & LC_O_APPEND, 0);
	(void)lc_sysLseek(fd, 0, LC_SEEK_SET);
	EXPECT_EQ(lc_sysWrite(fd, "abc", 3), 3);
	EXPECT_EQ(fc_size(fd), 23);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* O_NONBLOCK is a file status flag of the description. */
TEST_OPT(o_nonblock_is_a_file_status_flag, .ref = "XSH open()")
{
	char	p[128];
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "on"), 1), 0);
	fd = open(p, O_RDONLY | O_NONBLOCK);
	ASSERT_GE(fd, 0);
	EXPECT_NE(fc_flFlags(fd) & LC_O_NONBLOCK, 0);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* Without O_CLOEXEC, FD_CLOEXEC is clear. */
TEST_OPT(fd_cloexec_is_clear_by_default, .ref = "XSH open()")
{
	char	p[128];
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "oe"), 1), 0);
	fd = open(p, O_RDONLY);
	ASSERT_GE(fd, 0);
	EXPECT_EQ(fc_fdFlags(fd) & FC_FD_CLOEXEC, 0);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* O_CLOEXEC: FD_CLOEXEC is set on the new descriptor. */
TEST_OPT(o_cloexec_sets_fd_cloexec, .since = "POSIX.1-2008", .ref = "XSH open()")
{
#ifdef O_CLOEXEC
	char	p[128];
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "of"), 1), 0);
	fd = open(p, O_RDONLY | O_CLOEXEC);
	ASSERT_GE(fd, 0);
	EXPECT_NE(fc_fdFlags(fd) & FC_FD_CLOEXEC, 0);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
#else
	NOT_IMPLEMENTED("O_CLOEXEC");
#endif
}

/* O_DIRECTORY: a directory opens, another file gives ENOTDIR. */
TEST_OPT(o_directory_needs_a_directory, .since = "POSIX.1-2008", .ref = "XSH open()")
{
#ifdef O_DIRECTORY
	char	p[128];
	int		fd;

	fd = open("/", O_RDONLY | O_DIRECTORY);
	EXPECT_GE(fd, 0);
	(void)lc_sysClose(fd);
	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "od"), 1), 0);
	errno = 0;
	EXPECT_EQ(open(p, O_RDONLY | O_DIRECTORY), -1);
	EXPECT_EQ(errno, ENOTDIR);
	(void)lc_sysUnlink(p);
#else
	NOT_IMPLEMENTED("O_DIRECTORY");
#endif
}

/* O_NOFOLLOW: a symbolic link as the last component gives ELOOP. */
TEST_OPT(o_nofollow_on_a_symbolic_link_gives_eloop, .since = "POSIX.1-2008", .ref = "XSH open()")
{
#ifdef O_NOFOLLOW
	char	p[128];
	char	t[128];

	ASSERT_EQ(fc_makeFile(fc_path(t, sizeof(t), "ont"), 1), 0);
	ASSERT_EQ(lc_sysSymlink(t, fc_path(p, sizeof(p), "onl")), 0);
	errno = 0;
	EXPECT_EQ(open(p, O_RDONLY | O_NOFOLLOW), -1);
	EXPECT_EQ(errno, ELOOP);
	(void)lc_sysUnlink(p);
	(void)lc_sysUnlink(t);
#else
	NOT_IMPLEMENTED("O_NOFOLLOW");
#endif
}

/* A missing file without O_CREAT, or an empty path: ENOENT. */
TEST_OPT(missing_file_gives_enoent, .ref = "XSH open()")
{
	char	p[128];

	errno = 0;
	EXPECT_EQ(open(fc_path(p, sizeof(p), "oz"), O_RDONLY), -1);
	EXPECT_EQ(errno, ENOENT);
	errno = 0;
	EXPECT_EQ(open("", O_RDONLY), -1);
	EXPECT_EQ(errno, ENOENT);
}

/* O_CREAT in a missing directory: ENOENT; under a file: ENOTDIR. */
TEST_OPT(bad_prefix_gives_enoent_or_enotdir, .ref = "XSH open()")
{
	char	p[128];
	char	q[160];
	int		n;

	(void)fc_path(p, sizeof(p), "op2");
	for (n = 0; p[n]; n++)
		q[n] = p[n];
	q[n] = '/';
	q[n + 1] = 'f';
	q[n + 2] = 0;
	errno = 0;
	EXPECT_EQ(open(q, O_WRONLY | O_CREAT, 0600), -1);
	EXPECT_EQ(errno, ENOENT);
	ASSERT_EQ(fc_makeFile(p, 1), 0);
	errno = 0;
	EXPECT_EQ(open(q, O_WRONLY | O_CREAT, 0600), -1);
	EXPECT_EQ(errno, ENOTDIR);
	(void)lc_sysUnlink(p);
}

/* A directory opened for writing: EISDIR. */
TEST_OPT(directory_for_writing_gives_eisdir, .ref = "XSH open()")
{
	char	p[128];

	ASSERT_EQ(lc_sysMkdir(fc_path(p, sizeof(p), "ow"), 0700), 0);
	errno = 0;
	EXPECT_EQ(open(p, O_WRONLY), -1);
	EXPECT_EQ(errno, EISDIR);
	errno = 0;
	EXPECT_EQ(open(p, O_RDWR), -1);
	EXPECT_EQ(errno, EISDIR);
	(void)lc_sysRmdir(p);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(open("", O_RDONLY), (open)("", O_RDONLY));
}
