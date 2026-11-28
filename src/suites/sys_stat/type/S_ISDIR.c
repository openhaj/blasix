/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file S_ISDIR.c
 * @brief Tests for the S_ISDIR() macro (<sys/stat.h>).
 * @Created: 2026/11/28 11:20:13 by Moutig
 * @Updated: 2026/11/28 11:20:13 by Moutig
 *
 * S_ISDIR(m) tests whether st_mode m is that of a directory: nonzero if so,
 * 0 if not (XBD <sys/stat.h>). Checked on the st_mode the kernel gives
 * for one object of each file type (sys_stat_cases.h).
 *
 * Clauses: POSIX: XBD <sys/stat.h>.
 */
#include "sys_stat/sys_stat_cases.h"

SS_TYPE_MACRO_CASES(S_ISDIR, SS_DIR, a_directory)
