/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <string.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/24 07:38:34 by Moutig
 * @Updated: 2026/11/24 07:38:34 by Moutig
 *
 * Every type and constant of <string.h> in spec/string.spec has its cases here:
 * the shared ones of src/suites/bits/ (the same for every header that
 * defines the symbol), then what only <string.h> says. Each block is compiled
 * only when the target requires its symbol (-DLC_REQ_<name>, from lcrun)
 * and is reported under that symbol, not under "defs". Order: the header
 * under test and nothing else first; the bits/ files that include other
 * headers (BITS_INCLUDES_OTHER_HEADERS) last.
 *
 * <string.h>: size_t, NULL (C99 on), locale_t (POSIX.1-2008 on),
 * __STDC_VERSION_STRING_H__ (C23), errno_t and rsize_t (Annex K, with
 * OPTS=LIB_EXT1: the target defines __STDC_WANT_LIB_EXT1__ 1).
 */

#include "framework/lc.h"
#include <string.h>

/* ----- Shared cases (bits/): <string.h> and nothing else is included ----- */

#include "bits/stddef/size_t.h"
#include "bits/stddef/NULL.h"
#include "bits/locale/locale_t.h"
#include "bits/annex_k/errno_t.h"
#include "bits/annex_k/rsize_t.h"
#include "bits/version/__STDC_VERSION_STRING_H__.h"

/* ----- What only <string.h> says ----- */

# if defined(LC_REQ_size_t) || defined(LC_DEFS_ALL)
TEST_SYM(size_t__string_lengths, "size_t", is_the_type_of_string_lengths, .ref = "C11 7.24.6.3p1")
{
	EXPECT_TYPE(strlen(""), size_t);
	EXPECT_TYPE(strspn("", ""), size_t);
}
# endif

# if defined(LC_REQ_NULL) || defined(LC_DEFS_ALL)
/* The search functions return a value equal to NULL when nothing is found. */
TEST_SYM(NULL__returned, "NULL", equals_a_returned_null_pointer, .ref = "C11 7.24.5.2p3")
{
	EXPECT_TRUE(strchr("abc", 'z') == NULL);
}
# endif

# if defined(LC_REQ_locale_t) || defined(LC_DEFS_ALL)
TEST_SYM(locale_t__string_l, "locale_t", is_the_parameter_of_the_l_functions, .ref = "XBD <string.h>")
{
	EXPECT_TYPE(&strcoll_l, int (*)(const char *, const char *, locale_t));
	EXPECT_TYPE(&strerror_l, char *(*)(int, locale_t));
	EXPECT_TYPE(&strxfrm_l, size_t (*)(char *restrict, const char *restrict, size_t, locale_t));
}
# endif

# if defined(LC_REQ_errno_t) || defined(LC_DEFS_ALL)
TEST_SYM(errno_t__s_result, "errno_t", is_the_result_of_the_s_functions, .ref = "C11 K.3.7.1.1p1")
{
	EXPECT_TYPE(&memcpy_s, errno_t (*)(void *restrict, rsize_t, const void *restrict, rsize_t));
	EXPECT_TYPE(&strcpy_s, errno_t (*)(char *restrict, rsize_t, const char *restrict));
}
# endif

# if defined(LC_REQ_rsize_t) || defined(LC_DEFS_ALL)
TEST_SYM(rsize_t__s_size, "rsize_t", is_the_size_parameter_of_the_s_functions, .ref = "C11 K.3.7.4.1p1")
{
	EXPECT_TYPE(&memset_s, errno_t (*)(void *, rsize_t, int, rsize_t));
	EXPECT_TYPE(&strnlen_s, size_t (*)(const char *, size_t));
}
# endif

/* ----- Last: bits/ files that include other headers ----- */

#include "bits/locale/locale_t_same.h"
