/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <sys/stat.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/28 12:23:38 by Moutig
 * @Updated: 2026/11/28 12:23:43 by Moutig
 *
 * Every type and constant of <sys/stat.h> in spec/sys_stat.spec has its
 * cases here: the shared ones of src/suites/bits/ (the same for every
 * header that defines the symbol), then what only <sys/stat.h> says. Each
 * block is compiled only when the target requires its symbol and the
 * header declares it (-DLC_REQ_<name>) and is reported under that symbol.
 * Order: the header under test and nothing else first; the bits/ files
 * that include other headers (BITS_INCLUDES_OTHER_HEADERS) last.
 */

#include "framework/lc.h"
#include <sys/stat.h>

/* ----- Shared cases (bits/): <sys/stat.h> and nothing else is included ----- */

#include "bits/sys_types/blkcnt_t.h"
#include "bits/sys_types/blksize_t.h"
#include "bits/sys_types/dev_t.h"
#include "bits/sys_types/ino_t.h"
#include "bits/sys_types/mode_t.h"
#include "bits/sys_types/nlink_t.h"
#include "bits/sys_types/uid_t.h"
#include "bits/sys_types/gid_t.h"
#include "bits/sys_types/off_t.h"
#include "bits/sys_types/time_t.h"
#include "bits/sys_stat/file_types.h"
#include "bits/sys_stat/mode_bits.h"

/* ----- What only <sys/stat.h> says ----- */

# if (defined(LC_REQ_st_atime) && defined(LC_REQ_st_mtime) && defined(LC_REQ_st_ctime)) \
	|| defined(LC_DEFS_ALL)
/* st_atime, st_mtime, st_ctime are st_atim.tv_sec, st_mtim.tv_sec, st_ctim.tv_sec. */
TEST_SYM(st_atime__seconds, "st_atime", are_the_seconds_of_the_timespec_members,
	.ref = "XBD <sys/stat.h>")
{
	struct stat	s;

	lc_fill(&s, 0, sizeof(s));
	s.st_atim.tv_sec = 11;
	s.st_mtim.tv_sec = 22;
	s.st_ctim.tv_sec = 33;
	EXPECT_EQ((long)s.st_atime, 11);
	EXPECT_EQ((long)s.st_mtime, 22);
	EXPECT_EQ((long)s.st_ctime, 33);
}

TEST_SYM(st_atime__macros, "st_atime", are_macros, .ref = "XBD <sys/stat.h>")
{
#  if defined(st_atime) && defined(st_mtime) && defined(st_ctime)
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if (defined(LC_REQ_UTIME_NOW) && defined(LC_REQ_UTIME_OMIT)) || defined(LC_DEFS_ALL)
/* Two different tv_nsec values outside [0, 999999999]. */
TEST_SYM(UTIME_NOW__outside_the_nanoseconds, "UTIME_NOW", distinct_values_outside_0_999999999,
	.ref = "XBD <sys/stat.h>")
{
	long	now = UTIME_NOW;
	long	omit = UTIME_OMIT;

	EXPECT_TRUE(now < 0 || now > 999999999L);
	EXPECT_TRUE(omit < 0 || omit > 999999999L);
	EXPECT_NE(now, omit);
}
# endif
