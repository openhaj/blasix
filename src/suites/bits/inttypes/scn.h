/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file scn.h
 * @brief Cases of every SCN macro of <inttypes.h> (fscanf).
 * @Created: 2026/11/24 12:15:44 by Moutig
 * @Updated: 2026/11/24 12:15:50 by Moutig
 *
 * One block per SCN macro (7.8.1: d i o u x, b in C23; N, LEASTN, FASTN,
 * MAX, PTR), compiled only when the target requires it (-DLC_REQ_<macro>,
 * -DLC_DEFS_ALL in the editor); generators in format.h. Written by hand
 * from the clause's lists, one line per macro: keep it in the order of
 * 7.8.1.
 */

#ifndef BITS_INTTYPES_SCN_H
# define BITS_INTTYPES_SCN_H

# include "framework/lc.h"

/* Includes other headers: after every other bits/ file of a defs.c. */
# define BITS_INCLUDES_OTHER_HEADERS	1

# include "bits/inttypes/format.h"

# if defined(LC_REQ_SCNd8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNd8, 'd', int8_t, 1, INT8_MIN, INT8_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNd16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNd16, 'd', int16_t, 1, INT16_MIN, INT16_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNd32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNd32, 'd', int32_t, 1, INT32_MIN, INT32_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNd64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNd64, 'd', int64_t, 1, INT64_MIN, INT64_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdLEAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdLEAST8, 'd', int_least8_t, 1, INT_LEAST8_MIN, INT_LEAST8_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdLEAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdLEAST16, 'd', int_least16_t, 1, INT_LEAST16_MIN, INT_LEAST16_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdLEAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdLEAST32, 'd', int_least32_t, 1, INT_LEAST32_MIN, INT_LEAST32_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdLEAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdLEAST64, 'd', int_least64_t, 1, INT_LEAST64_MIN, INT_LEAST64_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdFAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdFAST8, 'd', int_fast8_t, 1, INT_FAST8_MIN, INT_FAST8_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdFAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdFAST16, 'd', int_fast16_t, 1, INT_FAST16_MIN, INT_FAST16_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdFAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdFAST32, 'd', int_fast32_t, 1, INT_FAST32_MIN, INT_FAST32_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdFAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdFAST64, 'd', int_fast64_t, 1, INT_FAST64_MIN, INT_FAST64_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdMAX) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdMAX, 'd', intmax_t, 1, INTMAX_MIN, INTMAX_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNdPTR) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNdPTR, 'd', intptr_t, 1, INTPTR_MIN, INTPTR_MAX, 10, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNi8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNi8, 'i', int8_t, 1, INT8_MIN, INT8_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNi8, int8_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNi16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNi16, 'i', int16_t, 1, INT16_MIN, INT16_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNi16, int16_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNi32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNi32, 'i', int32_t, 1, INT32_MIN, INT32_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNi32, int32_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNi64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNi64, 'i', int64_t, 1, INT64_MIN, INT64_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNi64, int64_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiLEAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiLEAST8, 'i', int_least8_t, 1, INT_LEAST8_MIN, INT_LEAST8_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiLEAST8, int_least8_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiLEAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiLEAST16, 'i', int_least16_t, 1, INT_LEAST16_MIN, INT_LEAST16_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiLEAST16, int_least16_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiLEAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiLEAST32, 'i', int_least32_t, 1, INT_LEAST32_MIN, INT_LEAST32_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiLEAST32, int_least32_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiLEAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiLEAST64, 'i', int_least64_t, 1, INT_LEAST64_MIN, INT_LEAST64_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiLEAST64, int_least64_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiFAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiFAST8, 'i', int_fast8_t, 1, INT_FAST8_MIN, INT_FAST8_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiFAST8, int_fast8_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiFAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiFAST16, 'i', int_fast16_t, 1, INT_FAST16_MIN, INT_FAST16_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiFAST16, int_fast16_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiFAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiFAST32, 'i', int_fast32_t, 1, INT_FAST32_MIN, INT_FAST32_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiFAST32, int_fast32_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiFAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiFAST64, 'i', int_fast64_t, 1, INT_FAST64_MIN, INT_FAST64_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiFAST64, int_fast64_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiMAX) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiMAX, 'i', intmax_t, 1, INTMAX_MIN, INTMAX_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiMAX, intmax_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNiPTR) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNiPTR, 'i', intptr_t, 1, INTPTR_MIN, INTPTR_MAX, 10, "C11 7.8.1p4")
IT_SCN_I_TESTS(SCNiPTR, intptr_t, "C11 7.8.1p4")
# endif

# if defined(LC_REQ_SCNo8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNo8, 'o', uint8_t, 0, 0, UINT8_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNo16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNo16, 'o', uint16_t, 0, 0, UINT16_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNo32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNo32, 'o', uint32_t, 0, 0, UINT32_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNo64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNo64, 'o', uint64_t, 0, 0, UINT64_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoLEAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoLEAST8, 'o', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoLEAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoLEAST16, 'o', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoLEAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoLEAST32, 'o', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoLEAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoLEAST64, 'o', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoFAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoFAST8, 'o', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoFAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoFAST16, 'o', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoFAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoFAST32, 'o', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoFAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoFAST64, 'o', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoMAX) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoMAX, 'o', uintmax_t, 0, 0, UINTMAX_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNoPTR) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNoPTR, 'o', uintptr_t, 0, 0, UINTPTR_MAX, 8, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNu8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNu8, 'u', uint8_t, 0, 0, UINT8_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNu16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNu16, 'u', uint16_t, 0, 0, UINT16_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNu32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNu32, 'u', uint32_t, 0, 0, UINT32_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNu64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNu64, 'u', uint64_t, 0, 0, UINT64_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuLEAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuLEAST8, 'u', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuLEAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuLEAST16, 'u', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuLEAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuLEAST32, 'u', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuLEAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuLEAST64, 'u', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuFAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuFAST8, 'u', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuFAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuFAST16, 'u', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuFAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuFAST32, 'u', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuFAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuFAST64, 'u', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuMAX) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuMAX, 'u', uintmax_t, 0, 0, UINTMAX_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNuPTR) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNuPTR, 'u', uintptr_t, 0, 0, UINTPTR_MAX, 10, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNx8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNx8, 'x', uint8_t, 0, 0, UINT8_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNx16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNx16, 'x', uint16_t, 0, 0, UINT16_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNx32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNx32, 'x', uint32_t, 0, 0, UINT32_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNx64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNx64, 'x', uint64_t, 0, 0, UINT64_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxLEAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxLEAST8, 'x', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxLEAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxLEAST16, 'x', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxLEAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxLEAST32, 'x', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxLEAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxLEAST64, 'x', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxFAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxFAST8, 'x', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxFAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxFAST16, 'x', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxFAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxFAST32, 'x', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxFAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxFAST64, 'x', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxMAX) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxMAX, 'x', uintmax_t, 0, 0, UINTMAX_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNxPTR) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNxPTR, 'x', uintptr_t, 0, 0, UINTPTR_MAX, 16, "C11 7.8.1p5")
# endif

# if defined(LC_REQ_SCNb8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNb8, 'b', uint8_t, 0, 0, UINT8_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNb16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNb16, 'b', uint16_t, 0, 0, UINT16_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNb32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNb32, 'b', uint32_t, 0, 0, UINT32_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNb64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNb64, 'b', uint64_t, 0, 0, UINT64_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbLEAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbLEAST8, 'b', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbLEAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbLEAST16, 'b', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbLEAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbLEAST32, 'b', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbLEAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbLEAST64, 'b', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbFAST8) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbFAST8, 'b', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbFAST16) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbFAST16, 'b', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbFAST32) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbFAST32, 'b', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbFAST64) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbFAST64, 'b', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbMAX) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbMAX, 'b', uintmax_t, 0, 0, UINTMAX_MAX, 2, "C23 7.8.1p6")
# endif

# if defined(LC_REQ_SCNbPTR) || defined(LC_DEFS_ALL)
IT_SCN_TESTS(SCNbPTR, 'b', uintptr_t, 0, 0, UINTPTR_MAX, 2, "C23 7.8.1p6")
# endif

#endif
