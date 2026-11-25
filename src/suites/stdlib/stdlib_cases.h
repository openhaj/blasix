/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file stdlib_cases.h
 * @brief Shared helpers and cases of the <stdlib.h> units.
 * @Created: 2026/11/25 13:05:29 by Moutig
 * @Updated: 2026/11/25 13:08:28 by Moutig
 *
 * What the units of src/suites/stdlib/ need around the function they
 * test:
 *
 *   - SL_ALIGNOF, sl_misalignedType: the alignment of the objects an
 *     allocation must be able to hold (7.24.3p1: "any type of object with
 *     a fundamental alignment requirement", and from C23 only those whose
 *     size fits);
 *   - sl_sizes: request sizes across the size classes of an allocator
 *     (every small size, then around each power of two up to 64 MiB);
 *   - sl_fill, sl_check: a byte pattern tied to the block and the offset,
 *     to see a block written by someone else or not written at all;
 *   - sl_disjoint: two allocations do not overlap.
 *
 * Only <stdlib.h> is under test. <stddef.h> (max_align_t, C11 on),
 * <stdint.h> and <errno.h> only prepare the data.
 */

#ifndef STDLIB_CASES_H
# define STDLIB_CASES_H

# include "framework/lc.h"
# include <stdlib.h>
# include <stddef.h>
# include <stdint.h>
# include <errno.h>

/* ----- Alignment ----- */

# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define SL_ALIGNOF(T)		_Alignof(T)
# else
#  define SL_ALIGNOF(T)		__alignof__(T)
# endif

/** A type with a fundamental alignment, its size and its alignment. */
typedef struct s_slType
{
	const char	*name;
	size_t		size;
	size_t		align;
}	t_slType;

# define SL_TYPE(T)	{#T, sizeof(T), SL_ALIGNOF(T)}

/* The scalar types, and max_align_t (the strictest) from C11 on. */
static const t_slType	g_slTypes[] = {
	SL_TYPE(short), SL_TYPE(int), SL_TYPE(long), SL_TYPE(long long),
	SL_TYPE(float), SL_TYPE(double), SL_TYPE(long double),
	SL_TYPE(void *), SL_TYPE(void (*)(void)), SL_TYPE(intmax_t),
# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
	SL_TYPE(max_align_t),
# endif
};

/*
 * The first type whose alignment @p p does not have, or NULL.
 *
 * @param fitOnly 1: only the types of at most @p n bytes (C23 7.24.3p1,
 *                "and size less than or equal to the size requested");
 *                0: every type (C99 to C17, POSIX).
 */
static inline const char	*sl_misalignedType(const void *p, size_t n, int fitOnly)
{
	for (size_t i = 0; i < sizeof(g_slTypes) / sizeof(g_slTypes[0]); i++)
		if ((!fitOnly || g_slTypes[i].size <= n)
			&& (uintptr_t)p % g_slTypes[i].align != 0)
			return (g_slTypes[i].name);
	return (NULL);
}

/* ----- Request sizes ----- */

/*
 * The i-th request size, 0 when there are no more: 1 ... 1100 one by one,
 * then each power of two from 2 KiB to 64 MiB, minus one, exact and plus
 * one (where allocators change their size class or go to the system).
 */
static inline size_t	sl_sizes(int i)
{
	int	k;

	if (i < 1100)
		return ((size_t)i + 1);
	i -= 1100;
	k = 11 + i / 3;
	if (k > 26)
		return (0);
	return (((size_t)1 << k) + (size_t)(i % 3) - 1);
}

/* ----- Contents ----- */

/* Byte @p i of the pattern of block @p id. */
static inline unsigned char	sl_byte(unsigned id, size_t i)
{
	return ((unsigned char)(i * 131u + (i >> 8) + id * 29u + 1u));
}

/*
 * Write the pattern of @p id in the @p n bytes of @p p: every byte up to
 * 4 KiB, then one per 512 bytes and the last one (large blocks).
 */
static inline void	sl_fill(void *p, size_t n, unsigned id)
{
	unsigned char	*b = p;

	for (size_t i = 0; i < n; i += (i < 4096 ? 1 : 512))
		b[i] = sl_byte(id, i);
	if (n)
		b[n - 1] = sl_byte(id, n - 1);
}

/* Do the bytes sl_fill() wrote still hold the pattern of @p id? */
static inline int	sl_check(const void *p, size_t n, unsigned id)
{
	const unsigned char	*b = p;

	for (size_t i = 0; i < n; i += (i < 4096 ? 1 : 512))
		if (b[i] != sl_byte(id, i))
			return (0);
	return (n == 0 || b[n - 1] == sl_byte(id, n - 1));
}

/* Do [a, a + n) and [b, b + m) not overlap? */
static inline int	sl_disjoint(const void *a, size_t n, const void *b, size_t m)
{
	uintptr_t	x = (uintptr_t)a;
	uintptr_t	y = (uintptr_t)b;

	return (x + n <= y || y + m <= x);
}

#endif
