/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pri.h
 * @brief Cases of every PRI macro of <inttypes.h> (fprintf).
 * @Created: 2026/11/24 12:14:56 by Moutig
 * @Updated: 2026/11/24 12:15:02 by Moutig
 *
 * One block per PRI macro (7.8.1: d i o u x X, b in C23; N, LEASTN, FASTN,
 * MAX, PTR), compiled only when the target requires it (-DLC_REQ_<macro>,
 * -DLC_DEFS_ALL in the editor); generators in format.h. Written by hand
 * from the clause's lists, one line per macro: keep it in the order of
 * 7.8.1.
 */

#ifndef BITS_INTTYPES_PRI_H
# define BITS_INTTYPES_PRI_H

# include "framework/lc.h"

/* Includes other headers: after every other bits/ file of a defs.c. */
# define BITS_INCLUDES_OTHER_HEADERS	1

# include "bits/inttypes/format.h"

# if defined(LC_REQ_PRId8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRId8, 'd', int8_t, 1, INT8_MIN, INT8_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRId16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRId16, 'd', int16_t, 1, INT16_MIN, INT16_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRId32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRId32, 'd', int32_t, 1, INT32_MIN, INT32_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRId64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRId64, 'd', int64_t, 1, INT64_MIN, INT64_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdLEAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdLEAST8, 'd', int_least8_t, 1, INT_LEAST8_MIN, INT_LEAST8_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdLEAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdLEAST16, 'd', int_least16_t, 1, INT_LEAST16_MIN, INT_LEAST16_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdLEAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdLEAST32, 'd', int_least32_t, 1, INT_LEAST32_MIN, INT_LEAST32_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdLEAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdLEAST64, 'd', int_least64_t, 1, INT_LEAST64_MIN, INT_LEAST64_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdFAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdFAST8, 'd', int_fast8_t, 1, INT_FAST8_MIN, INT_FAST8_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdFAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdFAST16, 'd', int_fast16_t, 1, INT_FAST16_MIN, INT_FAST16_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdFAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdFAST32, 'd', int_fast32_t, 1, INT_FAST32_MIN, INT_FAST32_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdFAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdFAST64, 'd', int_fast64_t, 1, INT_FAST64_MIN, INT_FAST64_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdMAX) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdMAX, 'd', intmax_t, 1, INTMAX_MIN, INTMAX_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIdPTR) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIdPTR, 'd', intptr_t, 1, INTPTR_MIN, INTPTR_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIi8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIi8, 'i', int8_t, 1, INT8_MIN, INT8_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIi16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIi16, 'i', int16_t, 1, INT16_MIN, INT16_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIi32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIi32, 'i', int32_t, 1, INT32_MIN, INT32_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIi64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIi64, 'i', int64_t, 1, INT64_MIN, INT64_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiLEAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiLEAST8, 'i', int_least8_t, 1, INT_LEAST8_MIN, INT_LEAST8_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiLEAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiLEAST16, 'i', int_least16_t, 1, INT_LEAST16_MIN, INT_LEAST16_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiLEAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiLEAST32, 'i', int_least32_t, 1, INT_LEAST32_MIN, INT_LEAST32_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiLEAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiLEAST64, 'i', int_least64_t, 1, INT_LEAST64_MIN, INT_LEAST64_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiFAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiFAST8, 'i', int_fast8_t, 1, INT_FAST8_MIN, INT_FAST8_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiFAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiFAST16, 'i', int_fast16_t, 1, INT_FAST16_MIN, INT_FAST16_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiFAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiFAST32, 'i', int_fast32_t, 1, INT_FAST32_MIN, INT_FAST32_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiFAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiFAST64, 'i', int_fast64_t, 1, INT_FAST64_MIN, INT_FAST64_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiMAX) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiMAX, 'i', intmax_t, 1, INTMAX_MIN, INTMAX_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIiPTR) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIiPTR, 'i', intptr_t, 1, INTPTR_MIN, INTPTR_MAX, 10, 0, "C11 7.8.1p2")
# endif

# if defined(LC_REQ_PRIo8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIo8, 'o', uint8_t, 0, 0, UINT8_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIo16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIo16, 'o', uint16_t, 0, 0, UINT16_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIo32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIo32, 'o', uint32_t, 0, 0, UINT32_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIo64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIo64, 'o', uint64_t, 0, 0, UINT64_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoLEAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoLEAST8, 'o', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoLEAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoLEAST16, 'o', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoLEAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoLEAST32, 'o', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoLEAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoLEAST64, 'o', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoFAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoFAST8, 'o', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoFAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoFAST16, 'o', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoFAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoFAST32, 'o', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoFAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoFAST64, 'o', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoMAX) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoMAX, 'o', uintmax_t, 0, 0, UINTMAX_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIoPTR) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIoPTR, 'o', uintptr_t, 0, 0, UINTPTR_MAX, 8, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIu8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIu8, 'u', uint8_t, 0, 0, UINT8_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIu16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIu16, 'u', uint16_t, 0, 0, UINT16_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIu32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIu32, 'u', uint32_t, 0, 0, UINT32_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIu64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIu64, 'u', uint64_t, 0, 0, UINT64_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuLEAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuLEAST8, 'u', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuLEAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuLEAST16, 'u', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuLEAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuLEAST32, 'u', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuLEAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuLEAST64, 'u', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuFAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuFAST8, 'u', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuFAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuFAST16, 'u', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuFAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuFAST32, 'u', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuFAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuFAST64, 'u', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuMAX) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuMAX, 'u', uintmax_t, 0, 0, UINTMAX_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIuPTR) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIuPTR, 'u', uintptr_t, 0, 0, UINTPTR_MAX, 10, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIx8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIx8, 'x', uint8_t, 0, 0, UINT8_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIx16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIx16, 'x', uint16_t, 0, 0, UINT16_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIx32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIx32, 'x', uint32_t, 0, 0, UINT32_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIx64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIx64, 'x', uint64_t, 0, 0, UINT64_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxLEAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxLEAST8, 'x', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxLEAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxLEAST16, 'x', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxLEAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxLEAST32, 'x', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxLEAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxLEAST64, 'x', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxFAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxFAST8, 'x', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxFAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxFAST16, 'x', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxFAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxFAST32, 'x', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxFAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxFAST64, 'x', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxMAX) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxMAX, 'x', uintmax_t, 0, 0, UINTMAX_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIxPTR) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIxPTR, 'x', uintptr_t, 0, 0, UINTPTR_MAX, 16, 0, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIX8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIX8, 'X', uint8_t, 0, 0, UINT8_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIX16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIX16, 'X', uint16_t, 0, 0, UINT16_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIX32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIX32, 'X', uint32_t, 0, 0, UINT32_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIX64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIX64, 'X', uint64_t, 0, 0, UINT64_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXLEAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXLEAST8, 'X', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXLEAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXLEAST16, 'X', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXLEAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXLEAST32, 'X', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXLEAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXLEAST64, 'X', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXFAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXFAST8, 'X', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXFAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXFAST16, 'X', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXFAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXFAST32, 'X', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXFAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXFAST64, 'X', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXMAX) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXMAX, 'X', uintmax_t, 0, 0, UINTMAX_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIXPTR) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIXPTR, 'X', uintptr_t, 0, 0, UINTPTR_MAX, 16, 1, "C11 7.8.1p3")
# endif

# if defined(LC_REQ_PRIb8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIb8, 'b', uint8_t, 0, 0, UINT8_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIB8
IT_PRIB_TESTS(PRIb8, PRIB8, uint8_t, UINT8_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIb16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIb16, 'b', uint16_t, 0, 0, UINT16_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIB16
IT_PRIB_TESTS(PRIb16, PRIB16, uint16_t, UINT16_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIb32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIb32, 'b', uint32_t, 0, 0, UINT32_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIB32
IT_PRIB_TESTS(PRIb32, PRIB32, uint32_t, UINT32_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIb64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIb64, 'b', uint64_t, 0, 0, UINT64_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIB64
IT_PRIB_TESTS(PRIb64, PRIB64, uint64_t, UINT64_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbLEAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbLEAST8, 'b', uint_least8_t, 0, 0, UINT_LEAST8_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBLEAST8
IT_PRIB_TESTS(PRIbLEAST8, PRIBLEAST8, uint_least8_t, UINT_LEAST8_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbLEAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbLEAST16, 'b', uint_least16_t, 0, 0, UINT_LEAST16_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBLEAST16
IT_PRIB_TESTS(PRIbLEAST16, PRIBLEAST16, uint_least16_t, UINT_LEAST16_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbLEAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbLEAST32, 'b', uint_least32_t, 0, 0, UINT_LEAST32_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBLEAST32
IT_PRIB_TESTS(PRIbLEAST32, PRIBLEAST32, uint_least32_t, UINT_LEAST32_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbLEAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbLEAST64, 'b', uint_least64_t, 0, 0, UINT_LEAST64_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBLEAST64
IT_PRIB_TESTS(PRIbLEAST64, PRIBLEAST64, uint_least64_t, UINT_LEAST64_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbFAST8) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbFAST8, 'b', uint_fast8_t, 0, 0, UINT_FAST8_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBFAST8
IT_PRIB_TESTS(PRIbFAST8, PRIBFAST8, uint_fast8_t, UINT_FAST8_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbFAST16) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbFAST16, 'b', uint_fast16_t, 0, 0, UINT_FAST16_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBFAST16
IT_PRIB_TESTS(PRIbFAST16, PRIBFAST16, uint_fast16_t, UINT_FAST16_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbFAST32) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbFAST32, 'b', uint_fast32_t, 0, 0, UINT_FAST32_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBFAST32
IT_PRIB_TESTS(PRIbFAST32, PRIBFAST32, uint_fast32_t, UINT_FAST32_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbFAST64) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbFAST64, 'b', uint_fast64_t, 0, 0, UINT_FAST64_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBFAST64
IT_PRIB_TESTS(PRIbFAST64, PRIBFAST64, uint_fast64_t, UINT_FAST64_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbMAX) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbMAX, 'b', uintmax_t, 0, 0, UINTMAX_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBMAX
IT_PRIB_TESTS(PRIbMAX, PRIBMAX, uintmax_t, UINTMAX_MAX)
#  endif
# endif

# if defined(LC_REQ_PRIbPTR) || defined(LC_DEFS_ALL)
IT_PRI_TESTS(PRIbPTR, 'b', uintptr_t, 0, 0, UINTPTR_MAX, 2, 0, "C23 7.8.1p3")
#  ifdef PRIBPTR
IT_PRIB_TESTS(PRIbPTR, PRIBPTR, uintptr_t, UINTPTR_MAX)
#  endif
# endif

#endif
