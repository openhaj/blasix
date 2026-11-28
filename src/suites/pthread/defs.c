/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <pthread.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/28 17:36:46 by Moutig
 * @Updated: 2026/11/28 17:36:52 by Moutig
 *
 * Every type and constant of <pthread.h> in spec/pthread.spec has its
 * cases here: the shared ones of src/suites/bits/ (the same for every
 * header that defines the symbol), then what only <pthread.h> says. Each
 * block is compiled only when the target requires its symbol and the
 * header declares it (-DLC_REQ_<name>) and is reported under that symbol.
 * Order: the header under test and nothing else first; the bits/ files
 * that include other headers (BITS_INCLUDES_OTHER_HEADERS) last.
 *
 * The pthread_*_t types, the attribute and state values, the barrier
 * value, the static initializers. Nothing calls a function (see
 * stdlib/defs.c): what the values do is checked by the units.
 */

#include "framework/lc.h"
#include <pthread.h>

/* ----- Shared cases (bits/): <pthread.h> and nothing else is included ----- */

#include "bits/sys_types/pthread_types.h"
#include "bits/pthread/attr.h"
#include "bits/pthread/barrier.h"
#include "bits/pthread/cancel.h"
#include "bits/pthread/mutex.h"
#include "bits/pthread/prio.h"
#include "bits/pthread/process.h"
#include "bits/pthread/initializers.h"

/* ----- What only <pthread.h> says: all of it is in bits/pthread/ ----- */
