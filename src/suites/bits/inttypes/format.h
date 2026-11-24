/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file format.h
 * @brief Generators of the PRI / SCN cases (<inttypes.h> 7.8.1).
 * @Created: 2026/11/24 12:14:27 by Moutig
 * @Updated: 2026/11/24 12:14:34 by Moutig
 *
 * The PRI and SCN macros are character string literals holding a
 * conversion specifier, possibly with a length modifier, "suitable" for
 * the corresponding type with the fprintf / fwprintf and fscanf / fwscanf
 * families. Generators, each case reported under its macro (TEST_SYM):
 *
 *   - IT_PRI_TESTS: a string literal ending with its conversion specifier;
 *     the extreme values of the type printed through snprintf() and
 *     swprintf() give the text of the converter of convert.h;
 *   - IT_SCN_TESTS: same literal checks; that text read back through
 *     sscanf() and swscanf() gives the value, and only sizeof(type) bytes
 *     are stored (guard bytes after the object): a wrong length modifier
 *     shows;
 *   - IT_SCN_I_TESTS (SCNi): the prefixes 0x and 0 select the base;
 *   - IT_PRIB_TESTS: PRIB<T> (C23, optional), when defined.
 *
 * snprintf, sscanf, swprintf and swscanf only exercise the macros: their
 * headers are included when they exist (the cases skip otherwise), and
 * referenced weakly (a function missing from the library skips instead of
 * breaking the link). The real headers are needed: a libc may map these
 * names (glibc: __isoc23_sscanf for %b in C23 mode). Instantiated by
 * pri.h and scn.h.
 */

#ifndef BITS_INTTYPES_FORMAT_H
# define BITS_INTTYPES_FORMAT_H

# include "framework/lc.h"

/* Includes other headers: after every other bits/ file of a defs.c. */
# define BITS_INCLUDES_OTHER_HEADERS	1

# include "bits/inttypes/convert.h"
# include "bits/inttypes/wide.h"

/* ----- Formatted I/O (not under test) ----- */

# if defined(__has_include)
#  if __has_include(<stdio.h>)
#   include <stdio.h>
#   define IT_HAVE_STDIO	1
#  endif
#  if __has_include(<wchar.h>)
#   include <wchar.h>
#   define IT_HAVE_WCHAR	1
#  endif
# endif

/*
 * Weak references: a function declared by the header but missing from
 * the library is NULL at run time (the case skips) instead of making the
 * unit fail to link.
 */
# ifdef IT_HAVE_STDIO
#  pragma weak snprintf
#  pragma weak sscanf
#  define IT_NEED(fn)		do { if (!(fn)) SKIP("no " #fn "() in this libc"); } while (0)
#  define it_snprintf		snprintf
#  define it_sscanf			sscanf
#  define IT_NEED_SNPRINTF()	IT_NEED(snprintf)
#  define IT_NEED_SSCANF()	IT_NEED(sscanf)
# else
#  define IT_NEED_SNPRINTF()	SKIP("no <stdio.h> in this libc")
#  define IT_NEED_SSCANF()	SKIP("no <stdio.h> in this libc")
#  define it_snprintf(...)	(-1)
#  define it_sscanf(...)	(-1)
# endif

# ifdef IT_HAVE_WCHAR
#  pragma weak swprintf
#  pragma weak swscanf
#  define it_swprintf		swprintf
#  define it_swscanf		swscanf
#  define IT_NEED_SWPRINTF()	do { if (!(swprintf)) SKIP("no swprintf() in this libc"); } while (0)
#  define IT_NEED_SWSCANF()	do { if (!(swscanf)) SKIP("no swscanf() in this libc"); } while (0)
# else
#  define IT_NEED_SWPRINTF()	SKIP("no <wchar.h> in this libc")
#  define IT_NEED_SWSCANF()	SKIP("no <wchar.h> in this libc")
#  define it_swprintf(...)	(-1)
#  define it_swscanf(...)	(-1)
# endif

/* ----- Literal checks, shared by PRI and SCN ----- */

/*
 * "%" M must be a valid concatenation (M is a character string literal)
 * whose last character is the conversion specifier CONV. ID (M##__) and
 * NAME (#M) are made by the caller: M itself is expanded when passed on
 * (PRId8 becomes "hhd").
 */
# define IT_LITERAL_TESTS(ID, NAME, M, CONV, REF)										\
	TEST_SYM(ID##is_a_string_literal, NAME, is_a_string_literal, .ref = REF)								\
	{																		\
		static const char	f_[] = "%" M;									\
		static const char	m_[] = M;										\
																			\
		EXPECT_GT(sizeof(m_), 1);											\
		EXPECT_EQ(f_[0], '%');												\
	}																		\
																			\
	TEST_SYM(ID##ends_with_the_conversion_specifier, NAME, ends_with_the_conversion_specifier, .ref = REF)				\
	{																		\
		static const char	m_[] = M;										\
																			\
		EXPECT_EQ(m_[sizeof(m_) - 2], CONV);								\
	}																		\
																			\
	/* Concatenates with a wide literal for fwprintf / fwscanf. */			\
	TEST_SYM(ID##concatenates_with_a_wide_literal, NAME, concatenates_with_a_wide_literal, .ref = REF)					\
	{																		\
		static const wchar_t	w_[] = L"%" M;								\
																			\
		EXPECT_EQ(w_[0], L'%');												\
		EXPECT_EQ(w_[sizeof(w_) / sizeof(w_[0]) - 2], (wchar_t)CONV);		\
	}

/* ----- fprintf macros ----- */

/*
 * M: the macro, CONV: its conversion character, T: the type, S: 1 if T
 * is signed, LO / HI: its limits (LO = 0 for an unsigned type), BASE /
 * UPPER: how CONV writes digits, REF: the clause.
 */
# define IT_PRI_TESTS(M, CONV, T, S, LO, HI, BASE, UPPER, REF)					\
	IT_LITERAL_TESTS(M##__, #M, M, CONV, REF)											\
																			\
	TEST_SYM(M##__prints_the_extreme_values, #M, prints_the_extreme_values, .ref = REF)							\
	{																		\
		const T	v_[] = {(T)(LO), (T)((LO) / 2), (T)0, (T)1, (T)((HI) / 3), (T)(HI)};	\
		char	got_[80];													\
		char	want_[80];													\
		int		bad_ = -1;													\
																			\
		IT_NEED_SNPRINTF();													\
		for (int i_ = 0; i_ < 6 && bad_ < 0; i_++)							\
		{																	\
			int	neg_ = (S) && (intmax_t)v_[i_] < 0;										\
																			\
			(void)it_format(want_, neg_ ? (uintmax_t)0 - (uintmax_t)(intmax_t)v_[i_]	\
				: (uintmax_t)v_[i_], neg_, BASE, UPPER);					\
			(void)it_snprintf(got_, sizeof(got_), "%" M, v_[i_]);				\
			if (!it_streq(got_, want_))										\
				bad_ = i_;													\
		}																	\
		EXPECT_EQ(bad_, -1);												\
	}																		\
																			\
	TEST_SYM(M##__wide_prints_the_extreme_values, #M, wide_prints_the_extreme_values, .ref = REF)					\
	{																		\
		const T	v_[] = {(T)(LO), (T)0, (T)1, (T)(HI)};						\
		wchar_t	got_[80];													\
		char	want_[80];													\
		int		bad_ = -1;													\
																			\
		IT_NEED_SWPRINTF();													\
		for (int i_ = 0; i_ < 4 && bad_ < 0; i_++)							\
		{																	\
			int	neg_ = (S) && (intmax_t)v_[i_] < 0;										\
																			\
			(void)it_format(want_, neg_ ? (uintmax_t)0 - (uintmax_t)(intmax_t)v_[i_]	\
				: (uintmax_t)v_[i_], neg_, BASE, UPPER);					\
			(void)it_swprintf(got_, 80, L"%" M, v_[i_]);						\
			if (!it_wideEq(got_, want_))									\
				bad_ = i_;													\
		}																	\
		EXPECT_EQ(bad_, -1);												\
	}

/* ----- fscanf macros ----- */

/* The object and guard bytes right after it: a wider store clobbers them. */
# define IT_GUARD	0xa5

# define IT_SCN_TESTS(M, CONV, T, S, LO, HI, BASE, REF)							\
	IT_LITERAL_TESTS(M##__, #M, M, CONV, REF)											\
																			\
	TEST_SYM(M##__scans_the_extreme_values, #M, scans_the_extreme_values, .ref = REF)							\
	{																		\
		const T		v_[] = {(T)(LO), (T)0, (T)1, (T)((HI) / 3), (T)(HI)};	\
		struct { T v; unsigned char g[sizeof(uintmax_t)]; }	box_;			\
		char		txt_[80];												\
		int			bad_ = -1;												\
																			\
		IT_NEED_SSCANF();													\
		for (int i_ = 0; i_ < 5 && bad_ < 0; i_++)							\
		{																	\
			int	neg_ = (S) && (intmax_t)v_[i_] < 0;										\
																			\
			(void)it_format(txt_, neg_ ? (uintmax_t)0 - (uintmax_t)(intmax_t)v_[i_]	\
				: (uintmax_t)v_[i_], neg_, BASE, 0);						\
			lc_fill(&box_, IT_GUARD, sizeof(box_));							\
			if (it_sscanf(txt_, "%" M, &box_.v) != 1 || box_.v != v_[i_])		\
				bad_ = i_;													\
			for (unsigned k_ = 0; k_ < sizeof(box_.g) && bad_ < 0; k_++)	\
				if (box_.g[k_] != IT_GUARD)									\
					bad_ = 100 + i_;										\
		}																	\
		EXPECT_EQ(bad_, -1);												\
	}																		\
																			\
	TEST_SYM(M##__wide_scans_the_extreme_values, #M, wide_scans_the_extreme_values, .ref = REF)						\
	{																		\
		const T		v_[] = {(T)(LO), (T)0, (T)(HI)};						\
		struct { T v; unsigned char g[sizeof(uintmax_t)]; }	box_;			\
		char		txt_[80];												\
		wchar_t		wtxt_[80];												\
		int			bad_ = -1;												\
																			\
		IT_NEED_SWSCANF();													\
		for (int i_ = 0; i_ < 3 && bad_ < 0; i_++)							\
		{																	\
			int	neg_ = (S) && (intmax_t)v_[i_] < 0;										\
																			\
			(void)it_format(txt_, neg_ ? (uintmax_t)0 - (uintmax_t)(intmax_t)v_[i_]	\
				: (uintmax_t)v_[i_], neg_, BASE, 0);						\
			lc_fill(&box_, IT_GUARD, sizeof(box_));							\
			if (it_swscanf(it_widen(wtxt_, txt_), L"%" M, &box_.v) != 1		\
				|| box_.v != v_[i_])										\
				bad_ = i_;													\
			for (unsigned k_ = 0; k_ < sizeof(box_.g) && bad_ < 0; k_++)	\
				if (box_.g[k_] != IT_GUARD)									\
					bad_ = 100 + i_;										\
		}																	\
		EXPECT_EQ(bad_, -1);												\
	}

/* %i takes the base from the prefix, like strtol() with base 0. */
# define IT_SCN_I_TESTS(M, T, REF)											\
	TEST_SYM(M##__prefix_selects_the_base, #M, prefix_selects_the_base, .ref = REF)							\
	{																		\
		T	v_ = 0;															\
																			\
		IT_NEED_SSCANF();													\
		EXPECT_EQ(it_sscanf("0x1f", "%" M, &v_), 1);						\
		EXPECT_EQ((long long)v_, 31);										\
		EXPECT_EQ(it_sscanf("017", "%" M, &v_), 1);							\
		EXPECT_EQ((long long)v_, 15);										\
	}

/* PRIB<T> (C23, optional): if defined, %B with a 0B alternate form;
   reported under PRIb<T> (M), the spec interface; MB is PRIB<T>. */
# define IT_PRIB_TESTS(M, MB, T, HI)												\
	TEST_SYM(M##__upper_b_macro_when_defined, #M, upper_b_macro_when_defined, .since = "C23", .ref = "C23 7.8.1p4")	\
	{																		\
		static const char	m_[] = MB;										\
		char				got_[80];										\
		char				want_[80];										\
																			\
		EXPECT_EQ(m_[sizeof(m_) - 2], 'B');									\
		IT_NEED_SNPRINTF();													\
		(void)it_snprintf(got_, sizeof(got_), "%" MB, (T)(HI));				\
		EXPECT_STREQ(got_, it_format(want_, (uintmax_t)(T)(HI), 0, 2, 0));	\
		(void)it_snprintf(got_, sizeof(got_), "%#" MB, (T)5);					\
		EXPECT_STREQ(got_, "0B101");										\
	}

#endif
