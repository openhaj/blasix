/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file lseek.c
 * @brief Tests for lseek() (<unistd.h>).
 * @Created: 2026/11/28 10:07:24 by Moutig
 * @Updated: 2026/11/28 10:07:50 by Moutig
 *
 * lseek(fildes, offset, whence) sets the file offset of the open file
 * description: to offset (SEEK_SET), to the current offset plus offset
 * (SEEK_CUR), to the size of the file plus offset (SEEK_END), and returns
 * it, measured from the start of the file. The offset may go past the
 * end: the size is not changed, and a later write leaves a gap that reads
 * as zeros. Errors: EBADF, EINVAL (whence not valid, or a negative
 * resulting offset; the offset is then unchanged), ESPIPE (a pipe).
 * POSIX.1-2024: SEEK_DATA and SEEK_HOLE (every seekable file has a
 * virtual hole at its end; ENXIO past the last byte).
 *
 * Clauses: POSIX: XSH lseek().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH lseek()")
{
	EXPECT_TYPE(&lseek, off_t (*)(int, off_t, int));
}

/* SEEK_SET: the offset given; the next read starts there. */
TEST_OPT(seek_set_goes_to_the_offset, .ref = "XSH lseek()")
{
	unsigned char	b[4];
	int				fd = ui_file("ls", 100, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(lseek(fd, 37, SEEK_SET), 37);
	EXPECT_EQ(lc_sysRead(fd, b, 4), 4);
	EXPECT_TRUE(ui_matches(b, 4, 37));
	EXPECT_EQ(lseek(fd, 0, SEEK_SET), 0);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 0);
	(void)lc_sysClose(fd);
}

/* SEEK_CUR: relative to the current offset, forward, backward, or 0 (where am I?). */
TEST_OPT(seek_cur_is_relative, .ref = "XSH lseek()")
{
	int	fd = ui_file("ls", 100, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	ASSERT_EQ(lc_sysLseek(fd, 50, LC_SEEK_SET), 50);
	EXPECT_EQ(lseek(fd, 10, SEEK_CUR), 60);
	EXPECT_EQ(lseek(fd, -25, SEEK_CUR), 35);
	EXPECT_EQ(lseek(fd, 0, SEEK_CUR), 35);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 35);
	(void)lc_sysClose(fd);
}

/* SEEK_END: relative to the size of the file. */
TEST_OPT(seek_end_is_relative_to_the_size, .ref = "XSH lseek()")
{
	unsigned char	b[3];
	int				fd = ui_file("ls", 100, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(lseek(fd, 0, SEEK_END), 100);
	EXPECT_EQ(lseek(fd, -3, SEEK_END), 97);
	EXPECT_EQ(lc_sysRead(fd, b, 3), 3);
	EXPECT_TRUE(ui_matches(b, 3, 97));
	(void)lc_sysClose(fd);
}

/* Past the end: allowed, the size kept; a write there leaves a gap of zeros. */
TEST_OPT(past_the_end_leaves_a_gap_of_zeros, .ref = "XSH lseek()")
{
	unsigned char	b[30];
	int				fd = ui_file("ls", 10, LC_O_RDWR);
	int				zeros = 1;

	ASSERT_NE(fd, -1);
	EXPECT_EQ(lseek(fd, 30, SEEK_SET), 30);
	EXPECT_EQ(ui_rawSize(fd), 10);
	EXPECT_EQ(lseek(fd, 5, SEEK_END), 15);
	EXPECT_EQ(ui_rawSize(fd), 10);
	ASSERT_EQ(lc_sysWrite(fd, "end", 3), 3);
	EXPECT_EQ(ui_rawSize(fd), 18);
	EXPECT_EQ(ui_rawContents(fd, 0, b, 18), 18);
	for (int i = 10; i < 15; i++)
		zeros = zeros && b[i] == 0;
	EXPECT_TRUE(ui_matches(b, 10, 0) && zeros && b[15] == 'e');
	(void)lc_sysClose(fd);
}

/* A large offset (8 GiB with a 64-bit off_t): set and returned exactly. */
TEST_OPT(large_offset, .ref = "XSH lseek()")
{
	int		fd = ui_file("ls", 1, LC_O_RDWR);
	off_t	big = (off_t)1 << (sizeof(off_t) >= 8 ? 33 : 30);

	ASSERT_NE(fd, -1);
	EXPECT_TRUE(lseek(fd, big, SEEK_SET) == big);
	EXPECT_TRUE(lseek(fd, 7, SEEK_CUR) == big + 7);
	EXPECT_EQ(ui_rawSize(fd), 1);
	(void)lc_sysClose(fd);
}

/* Descriptors of the same open file description share the offset. */
TEST_OPT(offset_belongs_to_the_open_file_description, .ref = "XSH lseek()")
{
	int		fd = ui_file("ls", 100, LC_O_RDONLY);
	long	dup;

	ASSERT_NE(fd, -1);
	dup = lc_sysFcntl(fd, LC_F_DUPFD_CLOEXEC, 0);
	ASSERT_TRUE(dup >= 0);
	EXPECT_EQ(lseek(fd, 42, SEEK_SET), 42);
	EXPECT_EQ(lc_sysLseek((int)dup, 0, LC_SEEK_CUR), 42);
	EXPECT_EQ(lseek((int)dup, 8, SEEK_CUR), 50);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 50);
	(void)lc_sysClose((int)dup);
	(void)lc_sysClose(fd);
}

/* A negative resulting offset: -1, EINVAL, the offset unchanged. */
TEST_OPT(negative_offset_gives_einval, .ref = "XSH lseek() ERRORS")
{
	int	fd = ui_file("ls", 20, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	ASSERT_EQ(lc_sysLseek(fd, 5, LC_SEEK_SET), 5);
	errno = 0;
	EXPECT_EQ(lseek(fd, -1, SEEK_SET), -1);
	EXPECT_EQ(errno, EINVAL);
	errno = 0;
	EXPECT_EQ(lseek(fd, -6, SEEK_CUR), -1);
	EXPECT_EQ(errno, EINVAL);
	errno = 0;
	EXPECT_EQ(lseek(fd, -21, SEEK_END), -1);
	EXPECT_EQ(errno, EINVAL);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 5);
	(void)lc_sysClose(fd);
}

/* A whence that is none of the SEEK_* values: -1 and EINVAL. */
TEST_OPT(invalid_whence_gives_einval, .ref = "XSH lseek() ERRORS")
{
	int	fd = ui_file("ls", 20, LC_O_RDONLY);
	int	bad = 12345;

	ASSERT_NE(fd, -1);
	errno = 0;
	EXPECT_EQ(lseek(fd, 0, bad), -1);
	EXPECT_EQ(errno, EINVAL);
	errno = 0;
	EXPECT_EQ(lseek(fd, 0, -1), -1);
	EXPECT_EQ(errno, EINVAL);
	(void)lc_sysClose(fd);
}

TEST_OPT(pipe_gives_espipe, .ref = "XSH lseek() ERRORS")
{
	int	p[2];

	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	errno = 0;
	EXPECT_EQ(lseek(p[0], 0, SEEK_CUR), -1);
	EXPECT_EQ(errno, ESPIPE);
	errno = 0;
	EXPECT_EQ(lseek(p[1], 0, SEEK_SET), -1);
	EXPECT_EQ(errno, ESPIPE);
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

TEST_OPT(bad_descriptor_gives_ebadf, .ref = "XSH lseek() ERRORS")
{
	int	c = ui_file("ls", 4, LC_O_RDONLY);

	ASSERT_NE(c, -1);
	(void)lc_sysClose(c);
	errno = 0;
	EXPECT_EQ(lseek(-1, 0, SEEK_SET), -1);
	EXPECT_EQ(errno, EBADF);
	errno = 0;
	EXPECT_EQ(lseek(c, 0, SEEK_SET), -1);
	EXPECT_EQ(errno, EBADF);
}

/*
 * POSIX.1-2024: SEEK_DATA finds data where there is some; SEEK_HOLE finds
 * the virtual hole at the end of a file without holes; both fail with
 * ENXIO from the end of the file on.
 */
TEST_OPT(seek_data_and_seek_hole, .since = "POSIX.1-2024", .ref = "XSH lseek() (Issue 8)")
{
#if defined(SEEK_DATA) && defined(SEEK_HOLE)
	int	fd = ui_file("ls", 5000, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(lseek(fd, 0, SEEK_DATA), 0);
	EXPECT_EQ(lseek(fd, 100, SEEK_DATA), 100);
	EXPECT_EQ(lseek(fd, 0, SEEK_HOLE), 5000);
	errno = 0;
	EXPECT_EQ(lseek(fd, 5000, SEEK_DATA), -1);
	EXPECT_EQ(errno, ENXIO);
	errno = 0;
	EXPECT_EQ(lseek(fd, 6000, SEEK_HOLE), -1);
	EXPECT_EQ(errno, ENXIO);
	(void)lc_sysClose(fd);
#else
	NOT_IMPLEMENTED("SEEK_DATA / SEEK_HOLE (<unistd.h>)");
#endif
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	int	fd = ui_file("ls", 10, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(lseek(fd, 3, SEEK_SET), 3);
	EXPECT_EQ((lseek)(fd, 2, SEEK_CUR), 5);
	(void)lc_sysClose(fd);
}
