/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <fcntl.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/28 11:13:11 by Moutig
 * @Updated: 2026/11/28 11:13:16 by Moutig
 *
 * Every type and constant of <fcntl.h> in spec/fcntl.spec has its cases
 * here, from src/suites/bits/; each block is compiled only when the target
 * requires its symbol and the header declares it (-DLC_REQ_<name>).
 *
 * <fcntl.h> (POSIX only): the fcntl() commands, the descriptor flags, the
 * lock types and owner types, the open() flags, SEEK_*, the file mode
 * bits, AT_*, POSIX_FADV_*; mode_t, off_t, pid_t.
 */

#include "framework/lc.h"
#include <fcntl.h>

/* ----- Shared cases (bits/): <fcntl.h> and nothing else is included ----- */

#include "bits/sys_types/mode_t.h"
#include "bits/sys_types/off_t.h"
#include "bits/sys_types/pid_t.h"
#include "bits/fcntl/commands.h"
#include "bits/fcntl/fd_flags.h"
#include "bits/fcntl/locks.h"
#include "bits/fcntl/owner.h"
#include "bits/fcntl/open_flags.h"
#include "bits/fcntl/at.h"
#include "bits/fcntl/fadv.h"
#include "bits/stdio/seek.h"
#include "bits/sys_stat/mode_bits.h"

/* ----- What only <fcntl.h> says: nothing calls a function (see stdlib/defs.c) ----- */
