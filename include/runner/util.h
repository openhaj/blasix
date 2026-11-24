/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file util.h
 * @brief Small helpers of lcrun (util.c).
 * @Created: 2026/11/24 08:28:49 by Moutig
 * @Updated: 2026/11/24 08:29:29 by Moutig
 *
 *
 * Allocation that exits on failure (status 2), growing strings, comma
 * lists and filters, a monotonic clock.
 */

#ifndef LCRUN_UTIL_H
# define LCRUN_UTIL_H

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include <stddef.h>

/**
 * @brief malloc() that exits on failure (status 2).
 *
 * @param n Size (0 allocates 1 byte).
 * @return The block.
 */
void		*xmalloc(size_t n);

/**
 * @brief realloc() that exits on failure (status 2).
 *
 * @param p Block, or NULL.
 * @param n New size (0 means 1 byte).
 * @return The block.
 */
void		*xrealloc(void *p, size_t n);

/**
 * @brief strdup() that exits on failure.
 *
 * @param s String.
 * @return New copy.
 */
char		*xstrdup(const char *s);

/**
 * @brief Copy the first @p n bytes of @p s into a new string.
 *
 * @param s Source (at least @p n bytes).
 * @param n Number of bytes.
 * @return New NUL-terminated string.
 */
char		*xstrndup(const char *s, size_t n);

/**
 * @brief Append @p n bytes of @p s to the heap string *@p dst.
 *
 * @param dst Heap string, or pointer to NULL (then created).
 * @param s   Bytes to append.
 * @param n   Number of bytes.
 */
void		strAppend(char **dst, const char *s, size_t n);

/**
 * @brief Monotonic clock, in milliseconds.
 *
 * @return Current time (only differences are meaningful).
 */
double		nowMs(void);






/**
 * @brief Is @p s in a filter list?
 *
 * @param list NULL-terminated list; NULL means "no filter".
 * @param s    String to look for.
 * @return 1 if found or @p list is NULL, 0 otherwise.
 */
int			inList(char **list, const char *s);

#endif
