/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file getpagesize.c
 * @brief Tests for getpagesize() (<unistd.h>, extension).
 * @Created: 2026/11/28 11:05:15 by Moutig
 * @Updated: 2026/11/28 11:05:38 by Moutig
 *
 * getpagesize() returns the number of bytes in a memory page, the unit of
 * mmap() and of the page alignment; it cannot fail. In no standard
 * (EXT=1; LEGACY in SUSv2, gone from POSIX.1-2001). Reference: BSD
 * getpagesize(3), glibc manual. The page size is the kernel's (AT_PAGESZ
 * of /proc/self/auxv).
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "BSD getpagesize(3)")
{
	EXPECT_TYPE(&getpagesize, int (*)(void));
}

TEST_OPT(is_the_system_page_size, .ref = "BSD getpagesize(3)")
{
	unsigned long	v[2];
	long			fd = lc_sysOpen("/proc/self/auxv", LC_O_RDONLY | LC_O_CLOEXEC, 0);
	long			page = 0;

	if (fd < 0)
		SKIP("no /proc/self/auxv");
	while (page == 0 && lc_sysRead((int)fd, v, sizeof(v)) == (long)sizeof(v) && v[0] != 0)
		if (v[0] == 6)
			page = (long)v[1];
	(void)lc_sysClose((int)fd);
	EXPECT_EQ(getpagesize(), page);
}

/* A power of two, the alignment of the mappings the kernel gives. */
TEST_OPT(is_a_power_of_two_that_aligns_mappings, .ref = "BSD getpagesize(3)")
{
	int		p = getpagesize();
	void	*m = lc_sysMmap(3 * (unsigned long)(p > 0 ? p : 1));

	EXPECT_GT(p, 0);
	EXPECT_EQ(p & (p - 1), 0);
	ASSERT_NOTNULL(m);
	EXPECT_UEQ((unsigned long)m % (unsigned long)p, 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(getpagesize(), (getpagesize)());
}
