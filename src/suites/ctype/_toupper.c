/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file _toupper.c
 * @brief Tests for _toupper() (<ctype.h>).
 * @Created: 2026/11/24 03:34:49 by Moutig
 * @Updated: 2026/11/24 03:51:02 by Moutig
 *
 * _toupper() is a macro, equivalent to toupper() except that the
 * application ensures that c is a lowercase letter (XSH _toupper()). XSI in
 * POSIX.1-2001, obsolescent (OB XSI) in 2008 and 2017, removed in
 * POSIX.1-2024. Only arguments for which islower() is true are tried.
 */
#include "ctype_cases.h"

TEST_OPT(is_a_macro, .ref = "POSIX <ctype.h>: defined as macros")
{
#ifdef _toupper
	EXPECT_TRUE(1);
#else
	EXPECT_TRUE(!"_toupper is not a macro");
#endif
}

TEST_OPT(c_locale_letters, .ref = "POSIX _toupper() DESCRIPTION")
{
	const char	*from = CT_LOWER_SET;
	int			first_wrong_letter = -1;

	for (int i = 0; from[i] && first_wrong_letter < 0; i++)
	{
		int	c = (unsigned char)from[i];

		if (_toupper(c) != toupper(c))
			first_wrong_letter = c;
	}
	EXPECT_EQ(first_wrong_letter, -1);
}

static void	ct_sameAstoupper(const char *name)
{
	int	first_wrong_value = -1;

	(void)name;
	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)
		if (islower(c) && _toupper(c) != toupper(c))
			first_wrong_value = c;
	EXPECT_EQ(first_wrong_value, -1);
}

TEST_OPT(other_locales, .ref = "POSIX _toupper() DESCRIPTION")
{
	CT_SKIP_LOCALES(ct_forEachLocale(ct_sameAstoupper));
}
