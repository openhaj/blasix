/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file S_TYPEISTMO.c
 * @brief Tests for the S_TYPEISTMO() macro (<sys/stat.h>).
 * @Created: 2026/11/28 11:20:13 by Moutig
 * @Updated: 2026/11/28 11:20:13 by Moutig
 *
 * S_TYPEISTMO(buf) is nonzero when the implementation makes a typed memory object (TYM) a
 * distinct file type and the stat structure *buf describes one; 0
 * otherwise (XBD <sys/stat.h>). No file the cases find is one, so the
 * macro gives 0 for all of them.
 *
 * Clauses: POSIX: XBD <sys/stat.h>.
 */
#include "sys_stat/sys_stat_cases.h"

SS_TYPEIS_CASES(S_TYPEISTMO, , .opt = "TYM")
