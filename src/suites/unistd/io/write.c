/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file write.c
 * @brief Tests for write() (<unistd.h>).
 * @Created: 2026/11/28 09:54:29 by Moutig
 * @Updated: 2026/11/28 09:57:01 by Moutig
 *
 * write(fildes, buf, nbyte) writes up to nbyte bytes from buf to the file
 * at its offset (at its end with O_APPEND), advances the offset, and
 * returns the number of bytes written, never more than nbyte; -1 and errno
 * on failure. nbyte 0 on a regular file: 0 and no other result. Errors:
 * EBADF (not a valid descriptor open for writing), EPIPE (a pipe that no
 * process has open for reading; SIGPIPE is also sent), EAGAIN (a full pipe
 * with O_NONBLOCK).
 *
 * The contents are read back raw (unistd_cases.h).
 *
 * Clauses: POSIX: XSH write().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH write()")
{
	EXPECT_TYPE(&write, ssize_t (*)(int, const void *, size_t));
}

/* Fill buf[0 .. n) with the pattern from 'from'. */
static void	fillPattern(unsigned char *buf, unsigned long n, unsigned long from)
{
	for (unsigned long i = 0; i < n; i++)
		buf[i] = ui_pattern(from + i);
}

TEST_OPT(writes_the_bytes, .ref = "XSH write()")
{
	unsigned char	buf[300];
	unsigned char	back[300];
	int				fd = ui_file("wr", 0, LC_O_RDWR);

	ASSERT_NE(fd, -1);
	fillPattern(buf, 300, 0);
	EXPECT_EQ(write(fd, buf, 300), 300);
	EXPECT_EQ(ui_rawSize(fd), 300);
	EXPECT_EQ(ui_rawContents(fd, 0, back, 300), 300);
	EXPECT_TRUE(ui_matches(back, 300, 0));
	(void)lc_sysClose(fd);
}

/* Successive writes follow each other; a write in the middle overwrites. */
TEST_OPT(advances_the_file_offset, .ref = "XSH write()")
{
	unsigned char	buf[40];
	unsigned char	back[40];
	int				fd = ui_file("wr", 0, LC_O_RDWR);

	ASSERT_NE(fd, -1);
	fillPattern(buf, 40, 0);
	for (int k = 0; k < 4; k++)
		EXPECT_EQ(write(fd, buf + k * 10, 10), 10);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 40);
	(void)lc_sysLseek(fd, 5, LC_SEEK_SET);
	EXPECT_EQ(write(fd, "XYZ", 3), 3);
	EXPECT_EQ(ui_rawSize(fd), 40);
	EXPECT_EQ(ui_rawContents(fd, 0, back, 40), 40);
	EXPECT_TRUE(ui_matches(back, 5, 0) && back[5] == 'X' && back[7] == 'Z'
		&& ui_matches(back + 8, 32, 8));
	(void)lc_sysClose(fd);
}

/* O_APPEND: every write goes to the end, wherever the offset was. */
TEST_OPT(append_mode_writes_at_the_end, .ref = "XSH write()")
{
	unsigned char	back[23];
	int				fd = ui_file("wr", 20, LC_O_RDWR | LC_O_APPEND);

	ASSERT_NE(fd, -1);
	(void)lc_sysLseek(fd, 0, LC_SEEK_SET);
	EXPECT_EQ(write(fd, "abc", 3), 3);
	EXPECT_EQ(ui_rawSize(fd), 23);
	EXPECT_EQ(ui_rawContents(fd, 0, back, 23), 23);
	EXPECT_TRUE(ui_matches(back, 20, 0) && back[20] == 'a' && back[22] == 'c');
	(void)lc_sysClose(fd);
}

/* nbyte 0 on a regular file: 0, the file and the offset unchanged. */
TEST_OPT(zero_count_returns_zero, .ref = "XSH write()")
{
	int	fd = ui_file("wr", 12, LC_O_RDWR);

	ASSERT_NE(fd, -1);
	(void)lc_sysLseek(fd, 4, LC_SEEK_SET);
	EXPECT_EQ(write(fd, "x", 0), 0);
	EXPECT_EQ(ui_rawSize(fd), 12);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 4);
	(void)lc_sysClose(fd);
}

/* 4 MiB to a regular file: the bytes written are the ones given, all of them. */
TEST_OPT(large_write_to_a_regular_file, .ref = "XSH write()")
{
	static unsigned char	buf[(4 << 20) + 13];
	static unsigned char	back[sizeof(buf)];
	int						fd = ui_file("wr", 0, LC_O_RDWR);
	long					done = 0;
	ssize_t					r = 0;

	ASSERT_NE(fd, -1);
	fillPattern(buf, sizeof(buf), 0);
	while (done < (long)sizeof(buf) && (r = write(fd, buf + done, sizeof(buf) - (size_t)done)) > 0)
	{
		EXPECT_TRUE(r <= (ssize_t)(sizeof(buf) - (size_t)done));
		done += r;
	}
	EXPECT_EQ(done, (long)sizeof(buf));
	EXPECT_EQ(ui_rawContents(fd, 0, back, sizeof(back)), (long)sizeof(back));
	EXPECT_TRUE(ui_matches(back, sizeof(back), 0));
	(void)lc_sysClose(fd);
}

/* Bytes written to a pipe come out of it in order. */
TEST_OPT(pipe_keeps_the_order, .ref = "XSH write()")
{
	int				p[2];
	unsigned char	buf[30];
	unsigned char	back[30];

	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	fillPattern(buf, 30, 0);
	EXPECT_EQ(write(p[1], buf, 10), 10);
	EXPECT_EQ(write(p[1], buf + 10, 20), 20);
	EXPECT_EQ(lc_sysRead(p[0], back, 30), 30);
	EXPECT_TRUE(ui_matches(back, 30, 0));
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

/* Not a descriptor open for writing: -1 and EBADF. */
TEST_OPT(bad_descriptor_gives_ebadf, .ref = "XSH write() ERRORS")
{
	int	r = ui_file("wr", 8, LC_O_RDONLY);
	int	c = ui_file("wr", 8, LC_O_RDWR);

	ASSERT_NE(r, -1);
	ASSERT_NE(c, -1);
	(void)lc_sysClose(c);
	errno = 0;
	EXPECT_EQ(write(-1, "x", 1), -1);
	EXPECT_EQ(errno, EBADF);
	errno = 0;
	EXPECT_EQ(write(c, "x", 1), -1);
	EXPECT_EQ(errno, EBADF);
	errno = 0;
	EXPECT_EQ(write(r, "x", 1), -1);
	EXPECT_EQ(errno, EBADF);
	EXPECT_EQ(ui_rawSize(r), 8);
	(void)lc_sysClose(r);
}

/* A pipe without reader, SIGPIPE ignored: -1 and EPIPE. */
static void	writeNoReaderIgnored(void *arg)
{
	int	p[2];

	(void)arg;
	if (lc_sysPipe(p, 0) != 0 || lc_sysSigaction(UI_SIGPIPE, UI_SIG_IGN, 0) != 0)
		lc_exitNow(90);
	(void)lc_sysClose(p[0]);
	errno = 0;
	if (write(p[1], "x", 1) != -1)
		lc_exitNow(1);
	lc_exitNow(errno == EPIPE ? 0 : 2);
}

TEST_OPT(no_reader_gives_epipe, .ref = "XSH write() ERRORS")
{
	EXPECT_EQ(lc_child(writeNoReaderIgnored, NULL), LC_EXIT(0));
}

/* A pipe without reader, SIGPIPE in its default action: the process dies of it. */
static void	writeNoReader(void *arg)
{
	int	p[2];

	(void)arg;
	if (lc_sysPipe(p, 0) != 0 || lc_sysSigaction(UI_SIGPIPE, UI_SIG_DFL, 0) != 0)
		lc_exitNow(90);
	(void)lc_sysClose(p[0]);
	(void)write(p[1], "x", 1);
	lc_exitNow(3);
}

TEST_OPT(no_reader_sends_sigpipe, .ref = "XSH write() ERRORS")
{
	EXPECT_EQ(lc_child(writeNoReader, NULL), LC_SIGNAL(UI_SIGPIPE));
}

/* A full pipe with O_NONBLOCK: a write that cannot go through gives -1 and EAGAIN. */
TEST_OPT(full_non_blocking_pipe_gives_eagain, .ref = "XSH write() ERRORS")
{
	static unsigned char	chunk[4096];
	int						p[2];
	ssize_t					r = 0;
	long					total = 0;

	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	ASSERT_TRUE(ui_nonBlock(p[1]));
	for (int i = 0; i < 4096 && r >= 0; i++)
	{
		errno = 0;
		r = write(p[1], chunk, sizeof(chunk));
		if (r > 0)
			total += r;
	}
	EXPECT_EQ(r, -1);
	EXPECT_EQ(errno, EAGAIN);
	EXPECT_GT(total, 0);
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	int	fd = ui_file("wr", 0, LC_O_RDWR);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(write(fd, "ab", 2), 2);
	EXPECT_EQ((write)(fd, "cd", 2), 2);
	EXPECT_EQ(ui_rawSize(fd), 4);
	(void)lc_sysClose(fd);
}
