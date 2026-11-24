/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isprint.c
 * @brief Tests for isprint() (<ctype.h>).
 * @Created: 2026/11/24 03:39:32 by Moutig
 * @Updated: 2026/11/24 03:54:00 by Moutig
 *
 * isprint() tests for any printing character including space (C 7.4.1.8);
 * letters and digits are printing characters, control characters are
 * not (7.4p3). In the POSIX locale: alnum, punct and space (XBD 7.3.1.1).
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.8; C23 7.4.2.8. POSIX: XSH isprint(), XBD 7.3.1.
 */
#include "ctype_cases.h"

CT_CLASSIFIER_TESTS(isprint, CT_PRINT, "POSIX.1-2001",
	CT_R_GRAPH | CT_R_CNTRL | CT_R_ALNUM_PRINT | CT_R_SPACE_PRINT,
	CT_R_GRAPH | CT_R_CNTRL | CT_R_ALNUM_PRINT | CT_R_SPACE_PRINT,
	"C17 7.4.1.8")

TEST_OPT(space_is_print, .ref = "C17 7.4.1.8")
{
	EXPECT_NE(CT_TRUTH(isprint(' ')), 0);
	EXPECT_NE(CT_TRUTH((isprint)(' ')), 0);
}

TEST_OPT(posix_other_bytes_print_only_if_punct, .since = "POSIX.1-2017",
	.ref = "XBD 6.2, 7.3.1.1 (only cntrl and punct may be added)")
{
	int	first_wrong_value = -1;

	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)
	{
		if ((ct_class(c) & CT_PORTABLE) || (ct_isAscii() && c <= 0x7f))
			continue ;
		if (isprint(c) && !ispunct(c))
			first_wrong_value = c;
	}
	EXPECT_EQ(first_wrong_value, -1);
}
