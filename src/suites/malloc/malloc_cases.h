/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file malloc_cases.h
 * @brief Shared helpers of the <malloc.h> units (extensions).
 * @Created: 2026/11/25 14:27:53 by Moutig
 * @Updated: 2026/11/25 14:28:18 by Moutig
 *
 * The units of src/suites/malloc/ test extensions (EXT=1), with the
 * helpers of the <stdlib.h> allocation units (stdlib/stdlib_cases.h:
 * sizes, patterns, alignment) and:
 *
 *   - ml_pageSize: the system page size (AT_PAGESZ of /proc/self/auxv,
 *     without the libc under test), for valloc() and pvalloc().
 *
 * <malloc.h> is under test, with <stdlib.h> (malloc(), free()) to make
 * and release the blocks.
 */

#ifndef MALLOC_CASES_H
# define MALLOC_CASES_H

# include "stdlib/stdlib_cases.h"
# include "framework/sys.h"
# include <malloc.h>

/** AT_PAGESZ, the auxiliary vector entry of the page size. */
# define ML_AT_PAGESZ	6

/*
 * The page size, from the auxiliary vector of the process (pairs of
 * unsigned long: type, value).
 *
 * @return The page size, or 0 when /proc/self/auxv cannot be read.
 */
static inline size_t	ml_pageSize(void)
{
	unsigned long	v[2];
	long			fd = lc_sysOpen("/proc/self/auxv", LC_O_RDONLY | LC_O_CLOEXEC, 0);
	size_t			page = 0;

	if (fd < 0)
		return (0);
	while (page == 0 && lc_sysRead((int)fd, v, sizeof(v)) == (long)sizeof(v) && v[0] != 0)
		if (v[0] == ML_AT_PAGESZ)
			page = v[1];
	(void)lc_sysClose((int)fd);
	return (page);
}

/* The page size of a case, or stop it. */
# define ML_PAGE(page)				\
	size_t	page = ml_pageSize();	\
	if (page == 0)					\
		SKIP("no page size (/proc/self/auxv)")

#endif
