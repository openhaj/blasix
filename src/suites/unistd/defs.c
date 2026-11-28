/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <unistd.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/28 09:46:51 by Moutig
 * @Updated: 2026/11/28 09:47:03 by Moutig
 *
 * Every type and constant of <unistd.h> in spec/unistd.spec has its cases
 * here: the shared ones of src/suites/bits/ (the same for every header
 * that defines the symbol), then what only <unistd.h> says. Each block is
 * compiled only when the target requires its symbol and the header
 * declares it (-DLC_REQ_<name>) and is reported under that symbol.
 * Order: the header under test and nothing else first; what needs other
 * headers last.
 *
 * <unistd.h> (POSIX only): the version test macros, the constants for
 * options, the _CS_, _PC_, _SC_ names, F_OK ... X_OK, F_LOCK ... F_ULOCK,
 * SEEK_*, *_FILENO, _POSIX_VDISABLE, POSIX_CLOSE_RESTART, O_CLOEXEC and
 * O_CLOFORK (2024), NULL; size_t, ssize_t, uid_t, gid_t, off_t, pid_t,
 * useconds_t (2001), intptr_t.
 */

#include "framework/lc.h"
#include <unistd.h>

/* ----- Shared cases (bits/): <unistd.h> and nothing else is included ----- */

#include "bits/stddef/size_t.h"
#include "bits/stddef/NULL.h"
#include "bits/sys_types/ssize_t.h"
#include "bits/sys_types/uid_t.h"
#include "bits/sys_types/gid_t.h"
#include "bits/sys_types/off_t.h"
#include "bits/sys_types/pid_t.h"
#include "bits/sys_types/useconds_t.h"
#include "bits/stdint/intptr_t.h"
#include "bits/stdio/seek.h"
#include "bits/fcntl/open_flags.h"
#include "bits/unistd/version.h"
#include "bits/unistd/options.h"
#include "bits/unistd/access.h"
#include "bits/unistd/lockf.h"
#include "bits/unistd/fileno.h"
#include "bits/unistd/misc.h"
#include "bits/unistd/confstr.h"
#include "bits/unistd/pathconf.h"
#include "bits/unistd/sysconf.h"

/* ----- What only <unistd.h> says: nothing calls a function (see stdlib/defs.c) ----- */

/* ----- Last: what needs other headers ----- */

# if defined(LC_REQ_ssize_t) || defined(LC_DEFS_ALL)
#  include <limits.h>

/* ssize_t holds at least [-1, SSIZE_MAX] (XBD <sys/types.h>). */
TEST_SYM(ssize_t__range, "ssize_t", holds_minus_1_to_SSIZE_MAX, .ref = "XBD <sys/types.h>")
{
#  ifdef SSIZE_MAX
	ssize_t	lo = -1;
	ssize_t	hi = SSIZE_MAX;

	EXPECT_TRUE(lo == -1 && hi == SSIZE_MAX && hi > 0);
#  else
	NOT_IMPLEMENTED("SSIZE_MAX (<limits.h>)");
#  endif
}
# endif
