/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strsep.c
 * @brief Tests for strsep() (<string.h>, extension).
 * @Created: 2026/11/25 10:30:30 by Moutig
 * @Updated: 2026/11/25 10:30:57 by Moutig
 *
 * strsep() splits the string *stringp at the first character of delim:
 * that character is overwritten with a null character, *stringp is
 * advanced past it, and the start of the token is returned. When no
 * delimiter is found, the token is the rest of the string and *stringp
 * becomes a null pointer; when *stringp is already a null pointer,
 * strsep() returns a null pointer. Unlike strtok(), empty fields are
 * returned (two delimiters in a row give an empty token).
 *
 * In no standard (EXT=1). Reference: strsep(3) of 4.4BSD (FreeBSD,
 * OpenBSD); the glibc manual, "Finding Tokens in a String", agrees.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "BSD strsep(3)")
{
	EXPECT_TYPE(&strsep, char *(*)(char **, const char *));
}

TEST_OPT(splits_and_advances, .ref = "BSD strsep(3)")
{
	char	buf[] = "ab,c";
	char	*p = buf;

	EXPECT_PTREQ(strsep(&p, ","), buf);
	EXPECT_EQ(buf[2], '\0');
	EXPECT_PTREQ(p, buf + 3);
	EXPECT_STREQ(buf, "ab");
}

TEST_OPT(last_token_sets_null, .ref = "BSD strsep(3)")
{
	char	buf[] = "ab,c";
	char	*p = buf;

	(void)strsep(&p, ",");
	EXPECT_PTREQ(strsep(&p, ","), buf + 3);
	EXPECT_NULL(p);
	EXPECT_STREQ(buf + 3, "c");
}

TEST_OPT(null_stringp_returns_null, .ref = "BSD strsep(3)")
{
	char	*p = NULL;

	EXPECT_NULL(strsep(&p, ","));
	EXPECT_NULL(p);
}

TEST_OPT(empty_fields_are_tokens, .ref = "BSD strsep(3)")
{
	char	buf[] = "a,,b";
	char	*p = buf;

	EXPECT_STREQ(strsep(&p, ","), "a");
	EXPECT_STREQ(strsep(&p, ","), "");
	EXPECT_STREQ(strsep(&p, ","), "b");
	EXPECT_NULL(p);
	EXPECT_NULL(strsep(&p, ","));
}

TEST_OPT(trailing_delimiter_gives_empty_token, .ref = "BSD strsep(3)")
{
	char	buf[] = "a,";
	char	*p = buf;

	EXPECT_STREQ(strsep(&p, ","), "a");
	EXPECT_PTREQ(p, buf + 2);
	EXPECT_PTREQ(strsep(&p, ","), buf + 2);
	EXPECT_NULL(p);
}

TEST_OPT(any_character_of_delim, .ref = "BSD strsep(3)")
{
	char	buf[] = "a b,c;d";
	char	*p = buf;

	EXPECT_STREQ(strsep(&p, " ,"), "a");
	EXPECT_STREQ(strsep(&p, ", "), "b");
	EXPECT_STREQ(strsep(&p, " ,"), "c;d");
	EXPECT_NULL(p);
}

TEST_OPT(empty_string_is_one_empty_token, .ref = "BSD strsep(3)")
{
	char	buf[] = "";
	char	*p = buf;

	EXPECT_PTREQ(strsep(&p, ","), buf);
	EXPECT_NULL(p);
}

TEST_OPT(empty_delim_takes_everything, .ref = "BSD strsep(3)")
{
	char	buf[] = "a,b";
	char	*p = buf;

	EXPECT_PTREQ(strsep(&p, ""), buf);
	EXPECT_NULL(p);
	EXPECT_STREQ(buf, "a,b");
}

/* Delimiters compare as bytes: high bytes, and no case folding. */
TEST_OPT(delimiters_are_exact_bytes, .ref = "BSD strsep(3)")
{
	char	buf[] = "a\xe9" "bAc";
	char	*p = buf;

	EXPECT_STREQ(strsep(&p, "\xe9"), "a");
	EXPECT_STREQ(strsep(&p, "a"), "bAc");
	EXPECT_NULL(p);
}

/* Every length (1 to 48) and delimiter position, with a delimiter set. */
TEST_OPT(every_length_and_position, .ref = "BSD strsep(3)")
{
	char	buf[56];
	int		bad = -1;

	for (int len = 1; len <= 48 && bad < 0; len++)
		for (int d = 0; d < len && bad < 0; d++)
		{
			char	*p = buf;

			lc_fill(buf, 'x', sizeof(buf));
			buf[len] = '\0';
			buf[d] = ';';
			if (strsep(&p, ",;:") != buf || buf[d] != '\0' || p != buf + d + 1
				|| lc_strlen(buf) != (lc_size_t)d)
				bad = len * 100 + d;
		}
	EXPECT_EQ(bad, -1);
}
