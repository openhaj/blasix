/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file version.h
 * @brief Cases of the version test macros of <unistd.h>.
 * @Created: 2026/11/28 09:36:09 by Moutig
 * @Updated: 2026/11/28 09:37:47 by Moutig
 *
 * _POSIX_VERSION and _POSIX2_VERSION are 200112L, 200809L (2008, 2017)
 * or 202405L for the edition the implementation conforms to, and
 * _XOPEN_VERSION (XSI only) 600, 700 or 800; usable in #if (XBD
 * <unistd.h>, "Version Test Macros"). An implementation of a later
 * edition gives a later value: at least the one of the edition under test.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_VERSION_H
# define BITS_UNISTD_VERSION_H

# include "framework/lc.h"

/* The value of the edition under test (_POSIX_C_SOURCE of the target). */
#  ifndef BITS_UNISTD_EDITION
#   if defined(_POSIX_C_SOURCE)
#    define BITS_UNISTD_EDITION	_POSIX_C_SOURCE
#   else
#    define BITS_UNISTD_EDITION	200112L
#   endif
#  endif

# if defined(LC_REQ__POSIX_VERSION) || defined(LC_DEFS_ALL)
/* Usable in #if; the value of the edition the implementation conforms to (at least the one under test). */
TEST_SYM(_POSIX_VERSION__value, "_POSIX_VERSION", at_least_the_edition_under_test, .ref = "XBD <unistd.h>")
{
#  if _POSIX_VERSION >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
	EXPECT_TRUE(_POSIX_VERSION == 200112L || _POSIX_VERSION == 200809L || _POSIX_VERSION == 202405L);
}
# endif

# if defined(LC_REQ__POSIX2_VERSION) || defined(LC_DEFS_ALL)
/* The same for the Shell and Utilities volume (2024: -1 allowed for a subprofile). */
TEST_SYM(_POSIX2_VERSION__value, "_POSIX2_VERSION", at_least_the_edition_under_test, .ref = "XBD <unistd.h>")
{
#  if defined(_POSIX_SUBPROFILE) && _POSIX2_VERSION == -1
	SKIP("_POSIX_SUBPROFILE: the Shell and Utilities volume is not supported");
#  elif _POSIX2_VERSION >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__XOPEN_VERSION) || defined(LC_DEFS_ALL)
/* XSI: 600 (2001), 700 (2008, 2017), 800 (2024), at least the one under test. */
TEST_SYM(_XOPEN_VERSION__value, "_XOPEN_VERSION", at_least_the_edition_under_test, .opt = "XSI",
	.ref = "XBD <unistd.h>")
{
#  if defined(_XOPEN_SOURCE) && _XOPEN_VERSION >= _XOPEN_SOURCE
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(_XOPEN_VERSION >= 600);
#  endif
	EXPECT_TRUE(_XOPEN_VERSION == 600 || _XOPEN_VERSION == 700 || _XOPEN_VERSION == 800);
}
# endif

#endif
