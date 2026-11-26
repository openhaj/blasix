/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <stdlib.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/25 12:51:12 by Moutig
 * @Updated: 2026/11/25 12:51:17 by Moutig
 *
 * Every type and constant of <stdlib.h> in spec/stdlib.spec has its cases
 * here: the shared ones of src/suites/bits/ (the same for every header
 * that defines the symbol), then what only <stdlib.h> says. Each block is
 * compiled only when the target requires its symbol and the header
 * declares it (-DLC_REQ_<name>) and is reported under that symbol.
 * Order: the header under test and nothing else first; the bits/ files
 * that include other headers (BITS_INCLUDES_OTHER_HEADERS) last.
 *
 * <stdlib.h>: size_t, wchar_t, div_t, ldiv_t, lldiv_t, NULL, EXIT_FAILURE,
 * EXIT_SUCCESS, RAND_MAX, MB_CUR_MAX (C99 on); __STDC_VERSION_STDLIB_H__,
 * once_flag, ONCE_FLAG_INIT (C23); the <sys/wait.h> macros (POSIX.1-2001
 * XSI, base from 2008); the <fcntl.h> constants (POSIX.1-2024); errno_t,
 * rsize_t, constraint_handler_t (Annex K, with OPTS=LIB_EXT1).
 */

#include "framework/lc.h"
#include <stdlib.h>

/* ----- Shared cases (bits/): <stdlib.h> and nothing else is included ----- */

#include "bits/stddef/size_t.h"
#include "bits/stddef/wchar_t.h"
#include "bits/stddef/NULL.h"
#include "bits/stdlib/div_t.h"
#include "bits/stdlib/ldiv_t.h"
#include "bits/stdlib/lldiv_t.h"
#include "bits/stdlib/EXIT_FAILURE.h"
#include "bits/stdlib/EXIT_SUCCESS.h"
#include "bits/stdlib/RAND_MAX.h"
#include "bits/stdlib/MB_CUR_MAX.h"
#include "bits/version/__STDC_VERSION_STDLIB_H__.h"
#include "bits/threads/once_flag.h"
#include "bits/threads/ONCE_FLAG_INIT.h"
#include "bits/wait/options.h"
#include "bits/wait/status.h"
#include "bits/fcntl/open_flags.h"
#include "bits/annex_k/errno_t.h"
#include "bits/annex_k/rsize_t.h"
#include "bits/annex_k/constraint_handler_t.h"

/* ----- What only <stdlib.h> says ----- */

/*
 * Nothing here uses a function of <stdlib.h>: LC_REQ_<name> only tells
 * that the type or macro is declared, and a function the libc lacks would
 * make the whole unit fail to build. That a type is the one of a function
 * (size_t of malloc, div_t of div, constraint_handler_t of
 * set_constraint_handler_s, ...) is checked by the prototype case of the
 * function's unit.
 */

/* ----- Last: what needs other headers ----- */

# if defined(LC_REQ_MB_CUR_MAX) || defined(LC_DEFS_ALL)
#  include <limits.h>

TEST_SYM(MB_CUR_MAX__mb_len_max, "MB_CUR_MAX", never_greater_than_MB_LEN_MAX, .ref = "C11 7.22p3")
{
	EXPECT_ULT(MB_CUR_MAX, (size_t)MB_LEN_MAX + 1);
}
# endif
