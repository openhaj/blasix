/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file convert.h
 * @brief Independent integer-to-text converter.
 * @Created: 2026/11/24 12:14:01 by Moutig
 * @Updated: 2026/11/24 12:14:13 by Moutig
 *
 * The text %d / %o / %u / %x / %X / %b write for a value, computed without
 * the libc: the oracle of the PRI / SCN cases and of the strto* units.
 * Only <inttypes.h> types are used (uintmax_t, intmax_t): the caller
 * includes <inttypes.h> first.
 */

#ifndef BITS_INTTYPES_CONVERT_H
# define BITS_INTTYPES_CONVERT_H

# include "framework/lc.h"

/** Sign of a comparison result: -1, 0 or 1. */
# define it_sign(x)		LC_SIGN(x)

/* ----- Independent converter ----- */

/**
 * @brief Write the text of a value in a base, like %d / %o / %x / %X / %b.
 *
 * @param buf   Destination, at least 72 bytes.
 * @param mag   Magnitude of the value.
 * @param neg   Non-zero: the value is -mag (a '-' is written first).
 * @param base  2, 8, 10 or 16.
 * @param upper Non-zero: hexadecimal digits in upper case.
 * @return @p buf.
 */
static inline char	*it_format(char *buf, uintmax_t mag, int neg, unsigned base, int upper)
{
	static const char	lo[] = "0123456789abcdef";
	static const char	up[] = "0123456789ABCDEF";
	char				tmp[72];
	int					n = 0;
	int					i = 0;

	do
	{
		tmp[n++] = (upper ? up : lo)[mag % base];
		mag /= base;
	} while (mag);
	if (neg)
		buf[i++] = '-';
	while (n)
		buf[i++] = tmp[--n];
	buf[i] = '\0';
	return (buf);
}

/** Text of a signed value (decimal, or another base for a negative magnitude). */
static inline char	*it_formatSigned(char *buf, intmax_t v, unsigned base)
{
	if (v < 0)
		return (it_format(buf, (uintmax_t)0 - (uintmax_t)v, 1, base, 0));
	return (it_format(buf, (uintmax_t)v, 0, base, 0));
}

/** Are two strings equal? */
static inline int	it_streq(const char *a, const char *b)
{
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return (*a == *b);
}

#endif
