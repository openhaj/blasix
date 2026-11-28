/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file read.c
 * @brief Tests for read() (<unistd.h>).
 * @Created: 2026/11/28 09:54:19 by Moutig
 * @Updated: 2026/11/28 09:56:38 by Moutig
 *
 * read(fildes, buf, nbyte) reads up to nbyte bytes from the file into
 * buf, from the file offset, which it advances; it returns the number of
 * bytes read, 0 at the end of the file, -1 and errno on failure. nbyte 0:
 * 0 and no other result (errors may be detected first). A pipe gives what
 * it holds (at most nbyte), 0 when no process has it open for writing,
 * and -1 with EAGAIN when it is empty and O_NONBLOCK is set. Errors:
 * EBADF (not a valid descriptor open for reading), EISDIR (XSI: a
 * directory, if the implementation does not allow reading it).
 *
 * Clauses: POSIX: XSH read().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH read()")
{
	EXPECT_TYPE(&read, ssize_t (*)(int, void *, size_t));
}

TEST_OPT(reads_the_bytes_of_the_file, .ref = "XSH read()")
{
	unsigned char	buf[100];
	int				fd = ui_file("rd", 100, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(read(fd, buf, 100), 100);
	EXPECT_TRUE(ui_matches(buf, 100, 0));
	(void)lc_sysClose(fd);
}

/* Each read starts where the last one ended. */
TEST_OPT(advances_the_file_offset, .ref = "XSH read()")
{
	unsigned char	buf[7];
	int				fd = ui_file("rd", 50, LC_O_RDONLY);
	int				bad = -1;

	ASSERT_NE(fd, -1);
	for (int k = 0; k < 7 && bad < 0; k++)
		if (read(fd, buf, 7) != 7 || !ui_matches(buf, 7, (unsigned long)k * 7))
			bad = k;
	EXPECT_EQ(bad, -1);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 49);
	(void)lc_sysClose(fd);
}

/* Fewer bytes than asked near the end, then 0 at the end of the file. */
TEST_OPT(short_read_then_zero_at_end_of_file, .ref = "XSH read()")
{
	unsigned char	buf[64];
	int				fd = ui_file("rd", 10, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(read(fd, buf, 64), 10);
	EXPECT_TRUE(ui_matches(buf, 10, 0));
	EXPECT_EQ(read(fd, buf, 64), 0);
	EXPECT_EQ(read(fd, buf, 64), 0);
	(void)lc_sysClose(fd);
}

TEST_OPT(empty_file_gives_zero, .ref = "XSH read()")
{
	unsigned char	buf[8];
	int				fd = ui_file("rd", 0, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(read(fd, buf, 8), 0);
	(void)lc_sysClose(fd);
}

/* nbyte 0: 0, nothing read, the offset kept. */
TEST_OPT(zero_count_returns_zero, .ref = "XSH read()")
{
	unsigned char	buf[4] = {0x55, 0x55, 0x55, 0x55};
	int				fd = ui_file("rd", 20, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	(void)lc_sysLseek(fd, 5, LC_SEEK_SET);
	EXPECT_EQ(read(fd, buf, 0), 0);
	EXPECT_EQ(buf[0], 0x55);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 5);
	(void)lc_sysClose(fd);
}

/* A large read of a regular file: the whole file at once, every byte right. */
TEST_OPT(large_read_of_a_regular_file, .ref = "XSH read()")
{
	static unsigned char	buf[(1 << 20) + 77];
	int						fd = ui_file("rd", sizeof(buf), LC_O_RDONLY);
	long					got = 0;
	long					r;

	ASSERT_NE(fd, -1);
	while (got < (long)sizeof(buf) && (r = read(fd, buf + got, sizeof(buf) - (size_t)got)) > 0)
		got += r;
	EXPECT_EQ(got, (long)sizeof(buf));
	EXPECT_TRUE(ui_matches(buf, sizeof(buf), 0));
	(void)lc_sysClose(fd);
}

/* A pipe gives what it holds, never more than nbyte, in order. */
TEST_OPT(pipe_gives_what_it_holds, .ref = "XSH read()")
{
	int				p[2];
	unsigned char	buf[64];
	unsigned char	data[10];

	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	for (int i = 0; i < 10; i++)
		data[i] = ui_pattern((unsigned long)i);
	ASSERT_EQ(lc_sysWrite(p[1], data, 10), 10);
	EXPECT_EQ(read(p[0], buf, 4), 4);
	EXPECT_TRUE(ui_matches(buf, 4, 0));
	EXPECT_EQ(read(p[0], buf, 64), 6);
	EXPECT_TRUE(ui_matches(buf, 6, 4));
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

/* No writer left: the data still in the pipe, then 0 (end of file). */
TEST_OPT(pipe_without_writer_gives_zero, .ref = "XSH read()")
{
	int				p[2];
	unsigned char	buf[8];

	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	ASSERT_EQ(lc_sysWrite(p[1], "abc", 3), 3);
	(void)lc_sysClose(p[1]);
	EXPECT_EQ(read(p[0], buf, 8), 3);
	EXPECT_EQ(read(p[0], buf, 8), 0);
	(void)lc_sysClose(p[0]);
}

/* An empty pipe that has a writer, with O_NONBLOCK: -1 and EAGAIN. */
TEST_OPT(empty_non_blocking_pipe_gives_eagain, .ref = "XSH read() ERRORS")
{
	int				p[2];
	unsigned char	buf[8];

	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	ASSERT_TRUE(ui_nonBlock(p[0]));
	errno = 0;
	EXPECT_EQ(read(p[0], buf, 8), -1);
	EXPECT_EQ(errno, EAGAIN);
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

/* Not a descriptor open for reading: -1 and EBADF. */
TEST_OPT(bad_descriptor_gives_ebadf, .ref = "XSH read() ERRORS")
{
	unsigned char	buf[8];
	int				w = ui_file("rd", 8, LC_O_WRONLY);
	int				c = ui_file("rd", 8, LC_O_RDONLY);

	ASSERT_NE(w, -1);
	ASSERT_NE(c, -1);
	(void)lc_sysClose(c);
	errno = 0;
	EXPECT_EQ(read(-1, buf, 8), -1);
	EXPECT_EQ(errno, EBADF);
	errno = 0;
	EXPECT_EQ(read(c, buf, 8), -1);
	EXPECT_EQ(errno, EBADF);
	errno = 0;
	EXPECT_EQ(read(w, buf, 8), -1);
	EXPECT_EQ(errno, EBADF);
	(void)lc_sysClose(w);
}

/* XSI: a directory gives EISDIR, unless the implementation lets it be read. */
TEST_OPT(directory_gives_eisdir, .opt = "XSI", .ref = "XSH read() ERRORS")
{
	unsigned char	buf[64];
	long			fd = lc_sysOpen("/tmp", LC_O_RDONLY | LC_O_CLOEXEC, 0);
	ssize_t			r;

	if (fd < 0)
		SKIP("cannot open /tmp");
	errno = 0;
	r = read((int)fd, buf, sizeof(buf));
	if (r >= 0)
		LOG("the implementation lets read() read a directory");
	else
		EXPECT_EQ(errno, EISDIR);
	(void)lc_sysClose((int)fd);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	unsigned char	buf[4];
	int				fd = ui_file("rd", 8, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(read(fd, buf, 4), 4);
	EXPECT_EQ((read)(fd, buf, 4), 4);
	EXPECT_TRUE(ui_matches(buf, 4, 4));
	(void)lc_sysClose(fd);
}
