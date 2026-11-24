/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file wide.h
 * @brief Wide-text helpers of the <inttypes.h> cases.
 * @Created: 2026/11/24 12:15:58 by Moutig
 * @Updated: 2026/11/24 12:16:05 by Moutig
 *
 * Widen ASCII text and compare it with a wide string, for the fwprintf /
 * fwscanf and wcsto* cases. Includes <stddef.h> for wchar_t (C 7.8.2.4:
 * "#include <stddef.h> // for wchar_t").
 */

#ifndef BITS_INTTYPES_WIDE_H
# define BITS_INTTYPES_WIDE_H

# include "framework/lc.h"

/* Includes other headers: after every other bits/ file of a defs.c. */
# define BITS_INCLUDES_OTHER_HEADERS	1

# include <stddef.h>

/** Widen ASCII text (members of the basic character set, C 7.19p2). */
static inline wchar_t	*it_widen(wchar_t *dst, const char *src)
{
	int	i = 0;

	while (src[i])
	{
		dst[i] = (wchar_t)src[i];
		i++;
	}
	dst[i] = L'\0';
	return (dst);
}

/** Do a narrow and a wide string hold the same ASCII text? */
static inline int	it_wideEq(const wchar_t *w, const char *s)
{
	int	i = 0;

	while (s[i] && w[i] == (wchar_t)s[i])
		i++;
	return (s[i] == '\0' && w[i] == L'\0');
}

#endif
