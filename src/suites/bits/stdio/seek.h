/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file seek.h
 * @brief Cases of SEEK_SET, SEEK_CUR, SEEK_END (and SEEK_DATA, SEEK_HOLE).
 * @Created: 2026/11/28 09:30:37 by Moutig
 * @Updated: 2026/11/28 09:30:43 by Moutig
 *
 * SEEK_SET, SEEK_CUR and SEEK_END are integer constant expressions with
 * distinct values (C 7.21.1p3 <stdio.h>; <unistd.h> and <fcntl.h> define
 * them as described there). POSIX.1-2024: <unistd.h> also defines
 * SEEK_DATA and SEEK_HOLE, distinct from each other and from the three.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDIO_SEEK_H
# define BITS_STDIO_SEEK_H

# include "framework/lc.h"

# if defined(LC_REQ_SEEK_SET) || defined(LC_DEFS_ALL)
TEST_SYM(SEEK_SET__int, "SEEK_SET", is_an_integer_constant_expression, .ref = "C11 7.21.1p3")
{
	static const char	a[(SEEK_SET) * 0 + 1];

	EXPECT_UEQ(sizeof(a), 1);
}
# endif

# if defined(LC_REQ_SEEK_CUR) || defined(LC_DEFS_ALL)
TEST_SYM(SEEK_CUR__int, "SEEK_CUR", is_an_integer_constant_expression, .ref = "C11 7.21.1p3")
{
	static const char	a[(SEEK_CUR) * 0 + 1];

	EXPECT_UEQ(sizeof(a), 1);
}
# endif

# if defined(LC_REQ_SEEK_END) || defined(LC_DEFS_ALL)
TEST_SYM(SEEK_END__int, "SEEK_END", is_an_integer_constant_expression, .ref = "C11 7.21.1p3")
{
	static const char	a[(SEEK_END) * 0 + 1];

	EXPECT_UEQ(sizeof(a), 1);
}
# endif

# if (defined(LC_REQ_SEEK_SET) && defined(LC_REQ_SEEK_CUR) && defined(LC_REQ_SEEK_END)) \
	|| defined(LC_DEFS_ALL)
TEST_SYM(SEEK_SET__distinct, "SEEK_SET", distinct_from_SEEK_CUR_and_SEEK_END, .ref = "C11 7.21.1p3")
{
	EXPECT_TRUE(SEEK_SET != SEEK_CUR && SEEK_SET != SEEK_END && SEEK_CUR != SEEK_END);
}
# endif

# if (defined(LC_REQ_SEEK_DATA) && defined(LC_REQ_SEEK_HOLE)) || defined(LC_DEFS_ALL)
/* POSIX.1-2024: integer constant expressions, distinct from each other and from SEEK_*. */
TEST_SYM(SEEK_DATA__distinct, "SEEK_DATA", distinct_from_SEEK_HOLE_and_the_other_whences,
	.ref = "XBD <unistd.h> (Issue 8)")
{
	static const char	a[(SEEK_DATA) * 0 + (SEEK_HOLE) * 0 + 1];
	const long			v[5] = {SEEK_SET, SEEK_CUR, SEEK_END, SEEK_DATA, SEEK_HOLE};
	int					bad = 0;

	for (int i = 0; i < 5; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
	EXPECT_UEQ(sizeof(a), 1);
}
# endif

#endif
