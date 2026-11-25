/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file free.c
 * @brief Tests for free() (<stdlib.h>).
 * @Created: 2026/11/25 12:57:36 by Moutig
 * @Updated: 2026/11/25 14:04:00 by Moutig
 *
 * free() deallocates the space ptr points to, which becomes available for
 * further allocation; a null pointer causes no action. Any other argument
 * that does not come from an allocation function, or a space already
 * deallocated, is undefined (not tested). It returns no value.
 * POSIX.1-2024 (CX): free() does not modify errno for a null pointer or a
 * live pointer returned as if by malloc().
 *
 * Only malloc() provides the blocks here: the pointers of calloc(),
 * realloc(), aligned_alloc() ... are freed in their own units.
 *
 * Stress: frees from other threads, blocks that outlive their thread
 * (POSIX threads, C11 7.22.3p1-2, XSH 2.9.1), and space freed then
 * allocated again after a pause (an allocator may give it back to the
 * system in between).
 *
 * Clauses: C99 7.20.3.2; C11, C17 7.22.3.3; C23 7.24.3.3. POSIX: XSH
 * free().
 */
#include "stdlib/stdlib_procs.h"

TEST_OPT(prototype, .ref = "C11 7.22.3.3p1")
{
	EXPECT_TYPE(&free, void (*)(void *));
}

TEST_OPT(null_pointer_does_nothing, .ref = "C11 7.22.3.3p2")
{
	void	*p = malloc(32);

	ASSERT_NOTNULL(p);
	sl_fill(p, 32, 7);
	free(NULL);
	free((void *)0);
	EXPECT_TRUE(sl_check(p, 32, 7));
	free(p);
}

/* Blocks of every size class can be freed (each one once). */
TEST_OPT(frees_every_size_class, .ref = "C11 7.22.3.3p2")
{
	int	n = 0;

	for (int i = 0; sl_sizes(i); i++)
	{
		void	*p = malloc(sl_sizes(i));

		ASSERT_NOTNULL(p);
		sl_fill(p, sl_sizes(i), (unsigned)i);
		free(p);
		n++;
	}
	EXPECT_GT(n, 1100);
}

/* What malloc(0) returns, null or not, can be passed to free(). */
TEST_OPT(frees_a_zero_size_allocation, .ref = "C11 7.22.3p1")
{
	void	*p = malloc(0);
	void	*q;

	free(p);
	q = malloc(8);
	EXPECT_NOTNULL(q);
	free(q);
}

/*
 * Freeing some blocks leaves the others intact: in allocation order, in
 * reverse order and one in two, with blocks of mixed sizes.
 */
TEST_OPT(other_blocks_are_untouched, .ref = "C11 7.22.3.3p2")
{
	static void		*p[600];
	static size_t	n[600];
	int				bad = -1;

	for (int order = 0; order < 3 && bad < 0; order++)
	{
		for (int i = 0; i < 600; i++)
		{
			n[i] = sl_sizes((i * 7 + order) % 1127);
			p[i] = malloc(n[i]);
			ASSERT_NOTNULL(p[i]);
			sl_fill(p[i], n[i], (unsigned)i);
		}
		for (int k = 0; k < 600; k++)
		{
			int	i = order == 0 ? k : order == 1 ? 599 - k : (k * 2) % 600 + (k >= 300);

			free(p[i]);
			p[i] = NULL;
			for (int j = 0; j < 600 && bad < 0 && k % 50 == 0; j++)
				if (p[j] && !sl_check(p[j], n[j], (unsigned)j))
					bad = order * 1000 + j;
		}
	}
	EXPECT_EQ(bad, -1);
}

/* The freed space can be allocated again, over and over. */
TEST_OPT(space_can_be_allocated_again, .ref = "C11 7.22.3.3p2")
{
	int	bad = -1;

	for (int round = 0; round < 2000 && bad < 0; round++)
	{
		size_t	n = round % 2 ? (size_t)1 << 20 : 100 + (size_t)round;
		void	*p = malloc(n);

		if (p == NULL)
			bad = round;
		else
		{
			sl_fill(p, n, (unsigned)round);
			if (!sl_check(p, n, (unsigned)round))
				bad = round;
			free(p);
		}
	}
	EXPECT_EQ(bad, -1);
}

/* POSIX.1-2024 (CX): errno is not modified, for NULL or a live block. */
TEST_OPT(errno_unchanged, .since = "POSIX.1-2024", .ref = "XSH free() (Issue 8)")
{
	void	*small = malloc(16);
	void	*big = malloc((size_t)4 << 20);

	ASSERT_NOTNULL(small);
	ASSERT_NOTNULL(big);
	errno = EDOM;
	free(NULL);
	EXPECT_EQ(errno, EDOM);
	free(small);
	EXPECT_EQ(errno, EDOM);
	free(big);
	EXPECT_EQ(errno, EDOM);
}

/* ----- Stress: threads (stdlib_procs.h) and a pause ----- */

# define F_PER	1500

/** One thread of free_from_another_thread: the blocks it frees, its own count. */
typedef struct s_fWorker
{
	void	**blocks;
	int		bad;
}	t_fWorker;

/* Free F_PER blocks of the main thread, allocating and freeing its own between. */
static void	*crossFreeWorker(void *arg)
{
	t_fWorker	*w = arg;

	for (int i = 0; i < F_PER && !w->bad; i++)
	{
		void	*own = malloc((size_t)(i % 300) + 1);

		if (own == NULL)
			w->bad = i + 1;
		free(w->blocks[i]);
		w->blocks[i] = NULL;
		free(own);
	}
	return (NULL);
}

/*
 * Blocks the main thread allocated are freed by 8 other threads; the
 * blocks it keeps are untouched, and the space can be allocated again.
 */
TEST_OPT(free_from_another_thread, .since = "POSIX.1-2008", .ref = "C11 7.22.3p2; XSH 2.9.1")
{
	static void		*b[8][F_PER];
	static void		*keep[F_PER];
	static size_t	n[F_PER];
	t_fWorker		w[8];
	int				bad = -1;

	for (int i = 0; i < F_PER; i++)
	{
		n[i] = sl_sizes(i * 13 % 1112);
		keep[i] = malloc(n[i]);
		ASSERT_NOTNULL(keep[i]);
		sl_fill(keep[i], n[i], (unsigned)i);
		for (int t = 0; t < 8; t++)
		{
			b[t][i] = malloc(sl_sizes((i + t * 7) % 1100));
			ASSERT_NOTNULL(b[t][i]);
		}
	}
	for (int t = 0; t < 8; t++)
	{
		w[t].blocks = b[t];
		w[t].bad = 0;
	}
	SL_THREADS(8, crossFreeWorker, w);
	for (int t = 0; t < 8; t++)
		EXPECT_EQ(w[t].bad, 0);
	for (int i = 0; i < F_PER && bad < 0; i++)
		if (!sl_check(keep[i], n[i], (unsigned)i))
			bad = i;
	for (int i = 0; i < F_PER; i++)
		free(keep[i]);
	for (int i = 0; i < 8 * F_PER && bad < 0; i++)
	{
		void	*p = malloc(sl_sizes(i % 1100));

		if (p == NULL)
			bad = 10000 + i;
		b[i / F_PER][i % F_PER] = p;
	}
	for (int i = 0; i < 8 * F_PER; i++)
		free(b[i / F_PER][i % F_PER]);
	EXPECT_EQ(bad, -1);
}

# define O_PER	200

/** One thread of blocks_outlive_their_thread: what it allocated. */
typedef struct s_oWorker
{
	unsigned	id;
	void		*p[O_PER];
	size_t		n[O_PER];
	int			bad;
}	t_oWorker;

/* Allocate O_PER blocks with the thread's pattern, and exit without freeing them. */
static void	*leaveBlocksWorker(void *arg)
{
	t_oWorker	*w = arg;

	for (int i = 0; i < O_PER; i++)
	{
		w->n[i] = sl_sizes((i * 11 + (int)w->id) % (i % 20 ? 1100 : 1118));
		w->p[i] = malloc(w->n[i]);
		if (w->p[i] == NULL)
		{
			w->bad = i + 1;
			return (NULL);
		}
		sl_fill(w->p[i], w->n[i], w->id * O_PER + (unsigned)i);
	}
	return (NULL);
}

/*
 * The lifetime of a block lasts until it is deallocated, not until its
 * thread ends: 3 rounds of 8 threads that allocate and exit; every block
 * of every round still holds its pattern, then the main thread frees them.
 */
TEST_OPT(blocks_outlive_their_thread, .since = "POSIX.1-2008", .ref = "C11 7.22.3p1")
{
	static t_oWorker	w[3][8];
	int					bad = -1;

	for (int r = 0; r < 3; r++)
	{
		for (int t = 0; t < 8; t++)
		{
			w[r][t].id = (unsigned)(r * 8 + t);
			w[r][t].bad = 0;
		}
		SL_THREADS(8, leaveBlocksWorker, w[r]);
	}
	for (int r = 0; r < 3; r++)
		for (int t = 0; t < 8; t++)
		{
			EXPECT_EQ(w[r][t].bad, 0);
			for (int i = 0; i < O_PER && bad < 0 && !w[r][t].bad; i++)
				if (!sl_check(w[r][t].p[i], w[r][t].n[i], w[r][t].id * O_PER + (unsigned)i))
					bad = (r * 8 + t) * O_PER + i;
		}
	for (int r = 0; r < 3; r++)
		for (int t = 0; t < 8; t++)
			for (int i = 0; i < O_PER && !w[r][t].bad; i++)
				free(w[r][t].p[i]);
	EXPECT_EQ(bad, -1);
}

/*
 * Much space freed (256 MiB in blocks of 64 KiB to 4 MiB), a pause
 * longer than the purge delays of common allocators (4.5 s: they give
 * freed space back to the system after a delay, on a later call), then
 * calls that let them do it, then as much allocated again: every block
 * is usable.
 */
/* The size of block @p i of freed_space_is_available_after_a_pause. */
static size_t	pauseSize(int i)
{
	return (i < 64 ? (size_t)64 << 10 : (size_t)(i % 4 + 1) << 20);
}

TEST_OPT(freed_space_is_available_after_a_pause, .timeout = 30, .ref = "C11 7.22.3.3p2")
{
	static void	*p[96];
	size_t		bad = 0;

	for (int i = 0; i < 96; i++)
	{
		p[i] = malloc(pauseSize(i));
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], pauseSize(i), (unsigned)i);
	}
	for (int i = 0; i < 96; i++)
		free(p[i]);
	lc_sleep(LC_NS_PER_SEC * 9 / 2);
	for (int i = 0; i < 25000; i++)
		free(malloc((size_t)(i % 200) * 16 + 1100));
	for (int i = 0; i < 96; i++)
	{
		p[i] = malloc(pauseSize(i));
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], pauseSize(i), (unsigned)i + 100);
	}
	for (int i = 0; i < 96 && !bad; i++)
		if (!sl_check(p[i], pauseSize(i), (unsigned)i + 100))
			bad = pauseSize(i);
	for (int i = 0; i < 96; i++)
		free(p[i]);
	EXPECT_UEQ(bad, 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	void	*a = malloc(8);
	void	*b = malloc(8);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	free(a);
	(free)(b);
	(free)(NULL);
	EXPECT_TRUE(1);
}
