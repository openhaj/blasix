/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file realloc.c
 * @brief Tests for realloc() (<stdlib.h>).
 * @Created: 2026/11/25 13:26:02 by Moutig
 * @Updated: 2026/11/25 14:04:15 by Moutig
 *
 * realloc() deallocates the old object and returns a pointer to a new
 * object of the given size, with the same contents up to the lesser of
 * the two sizes (the bytes past the old size are indeterminate, C23:
 * unspecified); the pointer may equal the old one. A null ptr: as
 * malloc(). When the new object cannot be allocated, the old one is not
 * deallocated and keeps its value, and a null pointer is returned. The
 * new object has the guarantees of every allocation (7.24.3p1).
 *
 * A size of zero with a non-null ptr changes with the edition and is not
 * tested in ISO C: C99, C11 leave it to 7.22.3p1, C17 makes the
 * deallocation of the old object implementation-defined (DR 400), C11
 * makes it obsolescent (7.31.12p2), C23 undefined (7.24.3.7p3). POSIX
 * says more: 2001 to 2017, the object is freed and the result is a null
 * pointer or a unique pointer free() accepts; 2024 ([OB]), either a null
 * pointer with errno EINVAL, or a pointer to allocated space with the
 * old object freed. POSIX (CX): a failure for lack of memory sets errno
 * to ENOMEM.
 *
 * Stress: blocks of ended POSIX threads resized by other threads.
 *
 * Clauses: C99 7.20.3.4; C11, C17 7.22.3.5; C23 7.24.3.7. POSIX: XSH
 * realloc().
 */
#include "stdlib/stdlib_procs.h"

TEST_OPT(prototype, .ref = "C11 7.22.3.5p1")
{
	EXPECT_TYPE(&realloc, void *(*)(void *, size_t));
}

/* A null pointer: like malloc(), for every size class. */
TEST_OPT(null_pointer_behaves_like_malloc, .ref = "C11 7.22.3.5p3")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && !bad; i += (i < 1100 ? 3 : 1))
	{
		size_t	n = sl_sizes(i);
		void	*p = realloc(NULL, n);

		if (p == NULL)
			bad = n;
		else
		{
			sl_fill(p, n, (unsigned)i);
			if (!sl_check(p, n, (unsigned)i))
				bad = n;
			free(p);
		}
	}
	EXPECT_UEQ(bad, 0);
}

/*
 * Resize a block of @p from bytes (filled with the pattern of @p id) to
 * @p to bytes, check the kept prefix, then write the whole new object.
 *
 * @return 1 if every check held.
 */
static int	resizeKeeps(size_t from, size_t to, unsigned id)
{
	unsigned char	*p = malloc(from);
	unsigned char	*q;
	int				ok;

	if (p == NULL)
		return (0);
	sl_fill(p, from, id);
	q = realloc(p, to);
	if (q == NULL)
	{
		free(p);
		return (0);
	}
	ok = sl_checkPrefix(q, from < to ? from : to, from, id);
	sl_fill(q, to, id + 1);
	ok = ok && sl_check(q, to, id + 1);
	free(q);
	return (ok);
}

/* The new size of a block of @p n bytes, case @p k (0 when done). */
static size_t	targetSize(size_t n, int k)
{
	switch (k)
	{
		case 0:
			return (n + 1);
		case 1:
			return (n * 2);
		case 2:
			return (n + 4096);
		case 3:
			return (n * 9 + 7);
		case 4:
			return (((size_t)1 << 20) + 3);
		case 5:
			return (n > 1 ? n / 2 : 1);
		case 6:
			return (n > 1 ? n - 1 : 1);
		case 7:
			return (n > 100 ? 100 : 1);
		case 8:
			return (n);
		default:
			return (0);
	}
}

/*
 * Every size class up to 16 MiB, grown and shrunk across the classes (one
 * byte more, twice, one page more, ninefold, 1 MiB; half, one byte less,
 * 100 bytes; the same size): the contents up to the lesser size are kept.
 */
TEST_OPT(contents_kept_up_to_the_lesser_size, .ref = "C11 7.22.3.5p2")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && sl_sizes(i) <= ((size_t)16 << 20) + 1 && !bad;
		i += (i < 1100 ? 11 : 1))
		for (int k = 0; targetSize(sl_sizes(i), k) && !bad; k++)
			if (!resizeKeeps(sl_sizes(i), targetSize(sl_sizes(i), k), (unsigned)i))
				bad = sl_sizes(i);
	EXPECT_UEQ(bad, 0);
}

/* One block grown a byte at a time to 5000 bytes, then shrunk the same way. */
TEST_OPT(grown_and_shrunk_byte_by_byte, .ref = "C11 7.22.3.5p2")
{
	unsigned char	*p = malloc(1);
	size_t			bad = 0;

	ASSERT_NOTNULL(p);
	p[0] = sl_byte(3, 0);
	for (size_t n = 2; n <= 5000 && !bad; n++)
	{
		unsigned char	*q = realloc(p, n);

		ASSERT_NOTNULL(q);
		p = q;
		if (!sl_check(p, n - 1, 3))
			bad = n;
		p[n - 1] = sl_byte(3, n - 1);
	}
	for (size_t n = 4999; n >= 1 && !bad; n--)
	{
		unsigned char	*q = realloc(p, n);

		ASSERT_NOTNULL(q);
		p = q;
		if (!sl_check(p, n, 3))
			bad = n;
	}
	free(p);
	EXPECT_UEQ(bad, 0);
}

/* Small to medium to huge and back, the first bytes kept at each step. */
TEST_OPT(through_the_size_classes_and_back, .ref = "C11 7.22.3.5p2")
{
	static const size_t	steps[] = {16, 1000, 9000, 70000, 600000, (size_t)40 << 20,
		300000, 20000, 500, 8};
	unsigned char		*p = malloc(16);
	size_t				kept = 16;
	int					bad = -1;

	ASSERT_NOTNULL(p);
	sl_fill(p, 16, 9);
	for (int k = 1; k < (int)(sizeof(steps) / sizeof(steps[0])) && bad < 0; k++)
	{
		unsigned char	*q = realloc(p, steps[k]);

		ASSERT_NOTNULL(q);
		p = q;
		if (steps[k] < kept)
			kept = steps[k];
		if (!sl_check(p, kept, 9))
			bad = k;
	}
	free(p);
	EXPECT_EQ(bad, -1);
}

/* C99 to C17 (and POSIX): the new object is aligned for every fundamental type. */
TEST_OPT(aligned_for_every_fundamental_type, .until = "C23", .ref = "C11 7.22.3p1")
{
	const char	*bad = NULL;

	for (int i = 0; sl_sizes(i) && !bad; i += (i < 1100 ? 5 : 1))
	{
		void	*p = malloc(8);
		void	*q;

		ASSERT_NOTNULL(p);
		q = realloc(p, sl_sizes(i));
		ASSERT_NOTNULL(q);
		bad = sl_misalignedType(q, sl_sizes(i), 0);
		free(q);
	}
	if (bad)
		LOG(bad);
	EXPECT_NULL(bad);
}

/* C23: aligned for every fundamental type that fits in the new size. */
TEST_OPT(aligned_for_every_type_that_fits, .since = "C23", .ref = "C23 7.24.3p1")
{
	const char	*bad = NULL;

	for (int i = 0; sl_sizes(i) && !bad; i += (i < 1100 ? 5 : 1))
	{
		void	*p = malloc(8);
		void	*q;

		ASSERT_NOTNULL(p);
		q = realloc(p, sl_sizes(i));
		ASSERT_NOTNULL(q);
		bad = sl_misalignedType(q, sl_sizes(i), 1);
		free(q);
	}
	if (bad)
		LOG(bad);
	EXPECT_NULL(bad);
}

/*
 * Blocks resized among live neighbours: the new objects stay disjoint
 * from the others, and the neighbours keep their contents.
 */
TEST_OPT(resized_blocks_stay_disjoint, .ref = "C11 7.22.3p1")
{
	static unsigned char	*p[200];
	static size_t			n[200];
	int						bad = -1;

	for (int i = 0; i < 200; i++)
	{
		n[i] = (size_t)(i * 37 % 300) + 1;
		p[i] = malloc(n[i]);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int round = 0; round < 4; round++)
		for (int i = round % 2; i < 200; i += 2)
		{
			size_t			m = round < 2 ? n[i] * 3 + 50 : n[i] / 2 + 1;
			unsigned char	*q = realloc(p[i], m);

			ASSERT_NOTNULL(q);
			if (bad < 0 && !sl_check(q, m < n[i] ? m : n[i], (unsigned)i))
				bad = i;
			p[i] = q;
			n[i] = m;
			sl_fill(p[i], n[i], (unsigned)i);
		}
	for (int i = 0; i < 200 && bad < 0; i++)
	{
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
		for (int j = 0; j < i && bad < 0; j++)
			if (!sl_disjoint(p[i], n[i], p[j], n[j]))
				bad = 1000 + i;
	}
	for (int i = 0; i < 200; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

/* No space for the new object: a null pointer, the old object intact and still owned. */
TEST_OPT(failure_keeps_the_old_object, .ref = "C11 7.22.3.5p3-4")
{
	static const size_t	huge[] = {SIZE_MAX, SIZE_MAX - 4095, SIZE_MAX / 2 + 1};
	int					bad = -1;

	for (int k = 0; k < 3; k++)
		for (int s = 0; s < 2; s++)
		{
			size_t			n = s ? (size_t)2 << 20 : 40;
			unsigned char	*p = malloc(n);

			ASSERT_NOTNULL(p);
			sl_fill(p, n, (unsigned)k);
			if (realloc(p, huge[k]) != NULL)
				bad = k * 2 + s;
			else if (!sl_check(p, n, (unsigned)k))
				bad = k * 2 + s;
			else
			{
				unsigned char	*q = realloc(p, n + 1);

				ASSERT_NOTNULL(q);
				if (!sl_check(q, n, (unsigned)k))
					bad = k * 2 + s;
				p = q;
			}
			free(p);
		}
	EXPECT_EQ(bad, -1);
	EXPECT_NULL(realloc(NULL, SIZE_MAX));
}

/* POSIX (CX): not enough memory sets errno to ENOMEM. */
TEST_OPT(failure_sets_enomem, .since = "POSIX.1-2001", .ref = "XSH realloc() ERRORS")
{
	void	*p = malloc(64);

	ASSERT_NOTNULL(p);
	errno = 0;
	EXPECT_NULL(realloc(p, SIZE_MAX));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(realloc(p, SIZE_MAX / 2 + 1));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(realloc(NULL, SIZE_MAX));
	EXPECT_EQ(errno, ENOMEM);
	free(p);
}

/*
 * POSIX.1-2001 to 2017: size 0 frees the object; the result is a null
 * pointer or a unique pointer that free() accepts.
 */
TEST_OPT(zero_size_frees_the_object, .since = "POSIX.1-2001", .until = "POSIX.1-2024",
	.ref = "XSH realloc()")
{
	void	*p = malloc(100);
	void	*o = malloc(1);
	void	*q;

	ASSERT_NOTNULL(p);
	ASSERT_NOTNULL(o);
	q = realloc(p, 0);
	if (q == NULL)
		LOG("realloc(p, 0) returns a null pointer");
	EXPECT_TRUE(q == NULL || q != o);
	free(q);
	free(o);
}

/*
 * POSIX.1-2024 ([OB]): size 0 gives either a null pointer with errno set
 * to EINVAL, or a pointer to allocated space (the old object freed).
 */
TEST_OPT(zero_size_is_null_with_einval_or_a_new_object, .since = "POSIX.1-2024",
	.ref = "XSH realloc() RETURN VALUE (Issue 8)")
{
	void	*p = malloc(100);
	void	*q;

	ASSERT_NOTNULL(p);
	errno = 0;
	q = realloc(p, 0);
	if (q == NULL)
	{
		EXPECT_EQ(errno, EINVAL);
	}
	free(q);
}

/* ----- Stress: threads (stdlib_procs.h) ----- */

# define R_PER	300

/** One thread: blocks it allocates, or resizes. */
typedef struct s_rWorker
{
	unsigned char	*p[R_PER];
	size_t			n[R_PER];
	size_t			filled[R_PER];
	unsigned		id;
	int				bad;
}	t_rWorker;

/* Allocate R_PER blocks with the thread's pattern, exit keeping them. */
static void	*makeWorker(void *arg)
{
	t_rWorker	*w = arg;

	for (int i = 0; i < R_PER; i++)
	{
		w->n[i] = sl_sizes((i * 29 + (int)w->id) % 1115);
		w->filled[i] = w->n[i];
		w->p[i] = malloc(w->n[i]);
		if (w->p[i] == NULL)
			w->bad = i + 1;
		else
			sl_fill(w->p[i], w->n[i], w->id * R_PER + (unsigned)i);
	}
	return (NULL);
}

/* Resize blocks another thread made (grow, then shrink); the prefix kept. */
static void	*resizeWorker(void *arg)
{
	t_rWorker	*w = arg;

	for (int i = 0; i < R_PER && !w->bad; i++)
	{
		size_t			big = w->n[i] * 3 + 100;
		size_t			small = w->n[i] / 2 + 1;
		unsigned char	*q = realloc(w->p[i], big);

		if (q == NULL || !sl_check(q, w->n[i], w->id * R_PER + (unsigned)i))
		{
			w->bad = i + 1;
			break ;
		}
		w->p[i] = q;
		q = realloc(q, small);
		if (q == NULL || !sl_checkPrefix(q, small, w->n[i], w->id * R_PER + (unsigned)i))
		{
			w->bad = i + 1;
			break ;
		}
		w->p[i] = q;
		w->n[i] = small;
	}
	return (NULL);
}

/*
 * Blocks allocated by threads that ended are resized by other threads,
 * then by the main thread: the contents are kept every time.
 */
TEST_OPT(resize_blocks_of_other_threads, .since = "POSIX.1-2008", .ref = "C11 7.22.3.5p2")
{
	static t_rWorker	w[4];
	int					bad = -1;

	for (int t = 0; t < 4; t++)
	{
		w[t].id = (unsigned)t;
		w[t].bad = 0;
	}
	SL_THREADS(4, makeWorker, w);
	SL_THREADS(4, resizeWorker, w);
	for (int t = 0; t < 4; t++)
	{
		EXPECT_EQ(w[t].bad, 0);
		for (int i = 0; i < R_PER && bad < 0 && !w[t].bad; i++)
		{
			unsigned char	*q = realloc(w[t].p[i], w[t].n[i] + 5000);

			ASSERT_NOTNULL(q);
			w[t].p[i] = q;
			if (!sl_checkPrefix(q, w[t].n[i], w[t].filled[i], w[t].id * R_PER + (unsigned)i))
				bad = t * R_PER + i;
		}
	}
	for (int t = 0; t < 4; t++)
		for (int i = 0; i < R_PER; i++)
			free(w[t].p[i]);
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	unsigned char	*a = malloc(4);
	unsigned char	*b = malloc(4);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	sl_fill(a, 4, 1);
	sl_fill(b, 4, 1);
	a = realloc(a, 4000);
	b = (realloc)(b, 4000);
	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	EXPECT_TRUE(sl_check(a, 4, 1) && sl_check(b, 4, 1));
	free(a);
	free(b);
}
