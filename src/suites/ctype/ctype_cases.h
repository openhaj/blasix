/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ctype_cases.h
 * @brief Oracle and shared test cases of the <ctype.h> units.
 * @Created: 2026/11/24 03:35:02 by Moutig
 * @Updated: 2026/11/24 04:00:01 by Moutig
 *
 * Every <ctype.h> function is checked the same way, so the cases live
 * here and each unit (src/suites/ctype/<function>.c) instantiates them
 * with CT_CLASSIFIER_TESTS, CT_CASEMAP_TESTS or CT_LOCALE_TESTS, then
 * adds what is specific to its function.
 *
 * What the standards say, and what is checked:
 *
 *   - ISO C 7.4p1 (C23 7.4.1p1): the argument is EOF or a value of
 *     unsigned char; every value 0 ... UCHAR_MAX and EOF are tried,
 *     nothing else (undefined behaviour);
 *   - ISO C 7.4.1 (C23 7.4.2): in the "C" locale, which is the locale
 *     at program startup (7.11.1.1p4), alpha, upper, lower, space and
 *     blank are exactly the basic letters / white-space / blank
 *     characters, digit and xdigit are exact in every locale; cntrl,
 *     print, graph and punct are only constrained by relations (7.4p3,
 *     7.4.1.6, 7.4.1.9), checked in every locale available;
 *   - POSIX (XBD 7.3.1.1): the POSIX locale (the "C" locale of a POSIX
 *     system) classifies every character of the portable character set
 *     exactly, cntrl, print, graph and punct included;
 *   - POSIX.1-2017 on (XBD 6.2, 7.3.1.1): a byte outside the portable
 *     character set may only be added to cntrl or punct.
 *
 * The expected classes are built from character literals, not from
 * code values: the portable characters need not be encoded in ASCII.
 * Only the "ascii_codes" case, which also covers the control codes that
 * have no C escape (SOH ... US, DEL), assumes ASCII, and skips
 * otherwise.
 *
 * Each case calls the function both as written (a macro if the header
 * defines one) and as (name) (the function itself): both must agree.
 *
 * Only <ctype.h> is required. The cases that switch locales need
 * setlocale(), newlocale() and freelocale() of <locale.h>: that header
 * is included only if it exists and those functions are referenced
 * weakly, so a libc without them still builds every unit and these
 * cases are skipped (with the reason) instead.
 */

#ifndef CTYPE_CASES_H
# define CTYPE_CASES_H

# include "framework/lc.h"
# include <ctype.h>
# include <stdio.h>

# if defined(__has_include)
#  if __has_include(<locale.h>)
#   include <locale.h>
#   define CT_HAVE_LOCALE_H	1
#  endif
# endif

# ifdef CT_HAVE_LOCALE_H
/* Weak: NULL when the libc does not define them (see the file comment). */
extern char		*setlocale(int category, const char *locale) __attribute__((weak));
#  if defined(LC_CTYPE_MASK)
#   define CT_HAVE_NEWLOCALE	1
extern locale_t	newlocale(int mask, const char *locale, locale_t base) __attribute__((weak));
extern void		freelocale(locale_t locale) __attribute__((weak));
#  endif
# endif

/** Largest value of unsigned char: every argument from 0 to this is tried. */
# define CT_UCHAR_MAX	((int)(unsigned char)-1)

/* ----- Characters of the portable character set, by class ----- */

/* XBD 6.1, 7.3.1.1; ISO C 5.2.1 (basic character set). */
# define CT_UPPER_SET	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
# define CT_LOWER_SET	"abcdefghijklmnopqrstuvwxyz"
# define CT_DIGIT_SET	"0123456789"
# define CT_XDIGIT_SET	"0123456789ABCDEFabcdef"
/** Standard white-space characters (C 7.4.1.10). */
# define CT_SPACE_SET	" \f\n\r\t\v"
/** Standard blank characters (C 7.4.1.3). */
# define CT_BLANK_SET	" \t"
/** The 32 punct characters of the POSIX locale (XBD 7.3.1.1). */
# define CT_PUNCT_SET	"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
/** Control characters of the POSIX locale that C can spell (and NUL). */
# define CT_CNTRL_SET	"\a\b\t\n\v\f\r"

/* ----- Classes (bit masks) ----- */

# define CT_UPPER		0x0001
# define CT_LOWER		0x0002
# define CT_ALPHA		0x0004
# define CT_DIGIT		0x0008
# define CT_XDIGIT		0x0010
# define CT_ALNUM		0x0020
# define CT_SPACE		0x0040
# define CT_BLANK		0x0080
# define CT_PUNCT		0x0100
# define CT_PRINT		0x0200
# define CT_GRAPH		0x0400
# define CT_CNTRL		0x0800
/** The value is a character of the portable set (or a spelled control). */
# define CT_PORTABLE	0x1000

/** Classes that ISO C makes exact in the "C" locale, for every value. */
# define CT_ISO_EXACT	(CT_UPPER | CT_LOWER | CT_ALPHA | CT_DIGIT | CT_XDIGIT \
						| CT_ALNUM | CT_SPACE | CT_BLANK)

/**
 * @brief Is @p c one of the characters of @p set?
 *
 * @param set Characters (NUL-terminated, NUL itself never matches).
 * @param c   Value 0 ... UCHAR_MAX.
 * @return 1 if found, 0 otherwise.
 */
static inline int	ct_in(const char *set, int c)
{
	for (; *set; set++)
		if ((unsigned char)*set == c)
			return (1);
	return (0);
}

/**
 * @brief Classes of @p c in the POSIX locale (XBD 7.3.1.1), for the
 *        characters the standard lists.
 *
 * alpha is upper and lower, alnum is alpha and digit, graph is alnum and
 * punct, print is graph and space. A value that is not in the portable
 * set nor a spelled control character gets 0 (no CT_PORTABLE): its
 * classes are not fixed, except where ISO C makes a class exact.
 *
 * @param c Value 0 ... UCHAR_MAX.
 * @return Bit mask of CT_* classes.
 */
static inline unsigned	ct_class(int c)
{
	unsigned	m = 0;

	if (c == 0 || ct_in(CT_CNTRL_SET, c))
		m |= CT_CNTRL | CT_PORTABLE;
	if (ct_in(CT_UPPER_SET, c))
		m |= CT_UPPER | CT_ALPHA;
	if (ct_in(CT_LOWER_SET, c))
		m |= CT_LOWER | CT_ALPHA;
	if (ct_in(CT_DIGIT_SET, c))
		m |= CT_DIGIT;
	if (ct_in(CT_XDIGIT_SET, c))
		m |= CT_XDIGIT;
	if (m & (CT_ALPHA | CT_DIGIT))
		m |= CT_ALNUM | CT_GRAPH | CT_PRINT | CT_PORTABLE;
	if (ct_in(CT_PUNCT_SET, c))
		m |= CT_PUNCT | CT_GRAPH | CT_PRINT | CT_PORTABLE;
	if (ct_in(CT_SPACE_SET, c))
		m |= CT_SPACE | CT_PORTABLE;
	if (ct_in(CT_BLANK_SET, c))
		m |= CT_BLANK;
	if (c == ' ')
		m |= CT_PRINT;
	return (m);
}

/**
 * @brief Is the execution character set ASCII (for the portable set)?
 *
 * @return 1 if every portable character checked has its ASCII value.
 */
static inline int	ct_isAscii(void)
{
	return ('A' == 0x41 && 'Z' == 0x5a && 'a' == 0x61 && 'z' == 0x7a
		&& '0' == 0x30 && ' ' == 0x20 && '~' == 0x7e && '!' == 0x21
		&& '\t' == 0x09 && '\n' == 0x0a && '\r' == 0x0d && '\a' == 0x07);
}

/**
 * @brief Classes of an ASCII code in the POSIX locale.
 *
 * As ct_class(), plus the control characters C cannot spell: in ASCII,
 * SOH ... US are 0x01 ... 0x1f and DEL is 0x7f (XBD 7.3.1.1 table).
 *
 * @param c Code 0 ... 0x7f.
 * @return Bit mask of CT_* classes.
 */
static inline unsigned	ct_asciiClass(int c)
{
	unsigned	m = ct_class(c);

	if (c < 0x20 || c == 0x7f)
		m |= CT_CNTRL | CT_PORTABLE;
	return (m);
}

/* ----- Relations between the classes, in every locale ----- */

/*
 * Each relation is a bit; a unit checks the ones about its function.
 * They hold in any locale, unless marked "C locale".
 */
/** isalnum(c) == isalpha(c) || isdigit(c) (7.4.1.1). */
# define CT_R_ALNUM			0x00001
/** isalpha(c): none of iscntrl, isdigit, ispunct, isspace (7.4.1.2). */
# define CT_R_ALPHA			0x00002
/** isupper(c) or islower(c) implies isalpha(c) (7.4.1.2). */
# define CT_R_CASE_ALPHA	0x00004
/** isupper(c): none of iscntrl, isdigit, ispunct, isspace (7.4.1.11). */
# define CT_R_UPPER			0x00008
/** islower(c): none of iscntrl, isdigit, ispunct, isspace (7.4.1.7). */
# define CT_R_LOWER			0x00010
/** isdigit(c) iff c is '0' ... '9', in every locale (7.4.1.5, 5.2.1). */
# define CT_R_DIGIT			0x00020
/** isxdigit(c) iff c is a hexadecimal digit, in every locale (7.4.1.12). */
# define CT_R_XDIGIT		0x00040
/** isgraph(c) == isprint(c) && c != ' ' (7.4.1.6, 7.4.1.8). */
# define CT_R_GRAPH			0x00080
/** ispunct(c) implies isprint(c), !isspace(c), !isalnum(c) (7.4.1.9). */
# define CT_R_PUNCT			0x00100
/** C locale: ispunct(c) == isprint(c) && !isspace(c) && !isalnum(c). */
# define CT_R_PUNCT_C		0x00200
/** isspace(c) implies !isalnum(c); standard white space is space (7.4.1.10). */
# define CT_R_SPACE			0x00400
/** Standard blanks are blank; isblank(c) implies isspace(c) (7.4.1.3). */
# define CT_R_BLANK			0x00800
/** iscntrl(c) implies !isprint(c): control characters do not print (7.4p3). */
# define CT_R_CNTRL			0x01000
/** isalnum(c) implies isprint(c): letters and digits print (7.4p3). */
# define CT_R_ALNUM_PRINT	0x02000
/** isprint(' '): print includes space (7.4.1.8). */
# define CT_R_SPACE_PRINT	0x04000
/** tolower(c) is c, or a lowercase letter when isupper(c) (7.4.2.1). */
# define CT_R_TOLOWER		0x08000
/** toupper(c) is c, or an uppercase letter when islower(c) (7.4.2.2). */
# define CT_R_TOUPPER		0x10000

/**
 * @brief The first relation of @p which that does not hold for @p c.
 *
 * Uses the functions as written (macros if the header defines them).
 *
 * @param c     Value 0 ... UCHAR_MAX.
 * @param which CT_R_* bits to check.
 * @return The broken CT_R_* bit, or 0 if all hold.
 */
static inline unsigned	ct_broken(int c, unsigned which)
{
	int	alpha = isalpha(c) != 0, digit = isdigit(c) != 0, alnum = isalnum(c) != 0;
	int	upper = isupper(c) != 0, lower = islower(c) != 0, space = isspace(c) != 0;
	int	cntrl = iscntrl(c) != 0, punct = ispunct(c) != 0, print = isprint(c) != 0;
	int	graph = isgraph(c) != 0, xdigit = isxdigit(c) != 0, blank = isblank(c) != 0;
	int	other = cntrl || digit || punct || space;

	if ((which & CT_R_ALNUM) && alnum != (alpha || digit))
		return (CT_R_ALNUM);
	if ((which & CT_R_ALPHA) && alpha && other)
		return (CT_R_ALPHA);
	if ((which & CT_R_CASE_ALPHA) && (upper || lower) && !alpha)
		return (CT_R_CASE_ALPHA);
	if ((which & CT_R_UPPER) && upper && other)
		return (CT_R_UPPER);
	if ((which & CT_R_LOWER) && lower && other)
		return (CT_R_LOWER);
	if ((which & CT_R_DIGIT) && digit != ct_in(CT_DIGIT_SET, c))
		return (CT_R_DIGIT);
	if ((which & CT_R_XDIGIT) && xdigit != ct_in(CT_XDIGIT_SET, c))
		return (CT_R_XDIGIT);
	if ((which & CT_R_GRAPH) && graph != (print && c != ' '))
		return (CT_R_GRAPH);
	if ((which & CT_R_PUNCT) && punct && (!print || space || alnum))
		return (CT_R_PUNCT);
	if ((which & CT_R_PUNCT_C) && punct != (print && !space && !alnum))
		return (CT_R_PUNCT_C);
	if ((which & CT_R_SPACE) && ((space && alnum) || (ct_in(CT_SPACE_SET, c) && !space)))
		return (CT_R_SPACE);
	if ((which & CT_R_BLANK) && ((blank && !space) || (ct_in(CT_BLANK_SET, c) && !blank)))
		return (CT_R_BLANK);
	if ((which & CT_R_CNTRL) && cntrl && print)
		return (CT_R_CNTRL);
	if ((which & CT_R_ALNUM_PRINT) && alnum && !print)
		return (CT_R_ALNUM_PRINT);
	if ((which & CT_R_SPACE_PRINT) && c == ' ' && !print)
		return (CT_R_SPACE_PRINT);
	if ((which & CT_R_TOLOWER) && tolower(c) != c && !(upper && islower(tolower(c))))
		return (CT_R_TOLOWER);
	if ((which & CT_R_TOUPPER) && toupper(c) != c && !(lower && isupper(toupper(c))))
		return (CT_R_TOUPPER);
	return (0);
}

/**
 * @brief Check relations @p which for every value in the current locale.
 *
 * Reports the first value and the first relation that fail (the
 * relation as its CT_R_* bit, see above).
 *
 * @param which CT_R_* bits.
 */
static inline void	ct_checkRelations(unsigned which)
{
	int			first_wrong_value = -1;
	unsigned	broken_relation = 0;

	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)
	{
		broken_relation = ct_broken(c, which);
		if (broken_relation)
			first_wrong_value = c;
	}
	EXPECT_EQ(first_wrong_value, -1);
	EXPECT_UEQ(broken_relation, 0);
}

/* ----- Other locales ----- */

/**
 * Locales tried besides "C" (the first few that the system has). Single
 * byte locales are the interesting ones: there, bytes above 0x7f are
 * characters with classes of their own.
 */
static const char *const	ct_locales[] = {
	"C.UTF-8", "C.utf8", "en_US.UTF-8", "en_US.utf8", "fr_FR.UTF-8",
	"fr_FR.utf8", "de_DE.UTF-8", "de_DE.utf8", "en_US.ISO-8859-1",
	"en_US.iso88591", "fr_FR.ISO-8859-1", "fr_FR.iso88591", "fr_FR",
	"de_DE.ISO-8859-1", "de_DE.iso88591", "de_DE", "en_US", 0
};

/**
 * @brief Run @p check with LC_CTYPE set to each locale of ct_locales the
 *        system has, then go back to "C".
 *
 * The name of each locale tried is logged.
 *
 * @param check Called once per locale, with its name.
 * @return Number of locales tried; 0 if none is installed, -1 if the
 *         libc has no setlocale() (the caller skips, see CT_SKIP_LOCALES).
 */
static inline int	ct_forEachLocale(void (*check)(const char *name))
{
#ifndef CT_HAVE_LOCALE_H
	(void)check;
	return (-1);
#else
	int	n = 0;

	if (setlocale == 0)
		return (-1);
	for (int i = 0; ct_locales[i]; i++)
	{
		if (setlocale(LC_CTYPE, ct_locales[i]) == 0)
			continue ;
		LOG(ct_locales[i]);
		check(ct_locales[i]);
		n++;
	}
	setlocale(LC_CTYPE, "C");
	return (n);
#endif
}

/** Skip when ct_forEachLocale() returned @p n <= 0, saying why. */
# define CT_SKIP_LOCALES(n)														do { if ((n) < 0) SKIP("no setlocale() (<locale.h>) in this libc");			if ((n) == 0) SKIP("no other locale installed"); } while (0)

/** Skip when the libc has no newlocale() / freelocale(). */
# ifdef CT_HAVE_NEWLOCALE
#  define CT_NEED_NEWLOCALE()													do { if (newlocale == 0 || freelocale == 0)										SKIP("no newlocale() / freelocale() (<locale.h>) in this libc"); } while (0)
# else
#  define CT_NEED_NEWLOCALE()													SKIP("no <locale.h> with LC_CTYPE_MASK in this libc")
# endif

/* ----- Test generators ----- */

/** Normalize a classification result: true / false. */
# define CT_TRUTH(x)	((x) != 0)
/** Normalize a mapping result: the value itself. */
# define CT_VALUE(x)	(x)

/**
 * @brief The cases every classification function has.
 *
 *   prototype                  int F(int)
 *   eof_is_in_no_class         F(EOF) is 0
 *   c_locale_exact             every value (CT_ISO_EXACT classes) or
 *                              every portable character (since POSIX:
 *                              the POSIX locale fixes them)
 *   ascii_codes                with ASCII: every code 0 ... 0x7f
 *   macro_and_function_agree   F(c) and (F)(c) give the same truth
 *   relations_c_locale         relations @p REL_C, every value
 *   relations_other_locales    relations @p REL in each other locale
 *
 * @param F      Function (isalpha).
 * @param CLASS  Its class (CT_ALPHA).
 * @param SINCE  NULL if ISO C fixes the class in the "C" locale,
 *               "POSIX.1-2001" if only POSIX does.
 * @param REL_C  CT_R_* relations of F, checked in the "C" locale.
 * @param REL    CT_R_* relations of F, checked in the other locales.
 * @param REF    Clause of F, e.g. "C17 7.4.1.2".
 */
# define CT_CLASSIFIER_TESTS(F, CLASS, SINCE, REL_C, REL, REF)				\
																			\
TEST_OPT(prototype, .ref = REF " Synopsis")									\
{																			\
	EXPECT_TYPE(&F, int (*)(int));											\
}																			\
																			\
TEST_OPT(eof_is_in_no_class, .ref = "C17 7.4p1, 7.4.1p1")					\
{																			\
	EXPECT_EQ(CT_TRUTH(F(EOF)), 0);											\
	EXPECT_EQ(CT_TRUTH((F)(EOF)), 0);										\
}																			\
																			\
TEST_OPT(c_locale_exact, .since = SINCE,									\
	.ref = REF "; XBD 7.3.1.1 (POSIX locale)")								\
{																			\
	int	first_wrong_value = -1;												\
																			\
	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)		\
	{																		\
		int	want = (ct_class(c) & (CLASS)) != 0;							\
																			\
		if (!((CLASS) & CT_ISO_EXACT) && !(ct_class(c) & CT_PORTABLE))		\
			continue ;														\
		if (CT_TRUTH(F(c)) != want || CT_TRUTH((F)(c)) != want)				\
			first_wrong_value = c;											\
	}																		\
	EXPECT_EQ(first_wrong_value, -1);										\
}																			\
																			\
TEST_OPT(ascii_codes, .since = "POSIX.1-2001",								\
	.ref = "XBD 7.3.1.1 (POSIX locale); C17 7.4p3 footnote 203")			\
{																			\
	int	first_wrong_value = -1;												\
																			\
	if (!ct_isAscii())														\
		SKIP("the execution character set is not ASCII");					\
	for (int c = 0; c <= 0x7f && first_wrong_value < 0; c++)				\
		if (CT_TRUTH(F(c)) != ((ct_asciiClass(c) & (CLASS)) != 0))			\
			first_wrong_value = c;											\
	EXPECT_EQ(first_wrong_value, -1);										\
}																			\
																			\
TEST_OPT(macro_and_function_agree, .ref = "C17 7.1.4p1")					\
{																			\
	int	first_wrong_value = -1;												\
																			\
	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)		\
		if (CT_TRUTH(F(c)) != CT_TRUTH((F)(c)))								\
			first_wrong_value = c;											\
	EXPECT_EQ(first_wrong_value, -1);										\
}																			\
																			\
TEST_OPT(relations_c_locale, .ref = REF)									\
{																			\
	ct_checkRelations(REL_C);												\
}																			\
																			\
static void	ct_otherLocale_##F(const char *name)							\
{																			\
	(void)name;																\
	ct_checkRelations(REL);													\
}																			\
																			\
TEST_OPT(relations_other_locales, .ref = REF)								\
{																			\
	CT_SKIP_LOCALES(ct_forEachLocale(ct_otherLocale_##F));					\
}

/**
 * @brief The cases of tolower / toupper.
 *
 *   prototype                  int F(int)
 *   eof_unchanged              F(EOF) is EOF
 *   c_locale_letters           each letter of FROM gives the same letter of TO
 *   c_locale_others_unchanged  every other value is returned unchanged
 *   macro_and_function_agree   F(c) == (F)(c)
 *   relations_c_locale / relations_other_locales   relation @p REL
 *
 * @param F    tolower or toupper.
 * @param FROM Letters it converts (CT_UPPER_SET for tolower).
 * @param TO   Their images (CT_LOWER_SET for tolower).
 * @param REL  CT_R_TOLOWER or CT_R_TOUPPER.
 * @param REF  Clause of F.
 */
# define CT_CASEMAP_TESTS(F, FROM, TO, REL, REF)								\
																			\
TEST_OPT(prototype, .ref = REF " Synopsis")									\
{																			\
	EXPECT_TYPE(&F, int (*)(int));											\
}																			\
																			\
TEST_OPT(eof_unchanged, .ref = REF " Returns")								\
{																			\
	EXPECT_EQ(F(EOF), EOF);													\
	EXPECT_EQ((F)(EOF), EOF);												\
}																			\
																			\
TEST_OPT(c_locale_letters, .ref = REF " Returns; XBD 7.3.1.1 tolower, toupper")	\
{																			\
	const char	*from = FROM;												\
	const char	*to = TO;													\
	int			first_wrong_letter = -1;									\
																			\
	for (int i = 0; from[i] && first_wrong_letter < 0; i++)					\
		if (F((unsigned char)from[i]) != (unsigned char)to[i]				\
			|| (F)((unsigned char)from[i]) != (unsigned char)to[i])			\
			first_wrong_letter = (unsigned char)from[i];					\
	EXPECT_EQ(first_wrong_letter, -1);										\
}																			\
																			\
TEST_OPT(c_locale_others_unchanged, .ref = REF " Returns")					\
{																			\
	int	first_wrong_value = -1;												\
																			\
	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)		\
		if (!ct_in(FROM, c) && (F(c) != c || (F)(c) != c))					\
			first_wrong_value = c;											\
	EXPECT_EQ(first_wrong_value, -1);										\
}																			\
																			\
TEST_OPT(macro_and_function_agree, .ref = "C17 7.1.4p1")					\
{																			\
	int	first_wrong_value = -1;												\
																			\
	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)		\
		if (F(c) != (F)(c))													\
			first_wrong_value = c;											\
	EXPECT_EQ(first_wrong_value, -1);										\
}																			\
																			\
TEST_OPT(relations_c_locale, .ref = REF " Returns")							\
{																			\
	ct_checkRelations(REL);													\
}																			\
																			\
static void	ct_otherLocale_##F(const char *name)							\
{																			\
	(void)name;																\
	ct_checkRelations(REL);													\
}																			\
																			\
TEST_OPT(relations_other_locales, .ref = REF " Returns")					\
{																			\
	CT_SKIP_LOCALES(ct_forEachLocale(ct_otherLocale_##F));					\
}

/* ----- Locale objects (the *_l units) ----- */

/*
 * Wrappers, so that the generator below needs no #ifdef: without
 * newlocale() they are macros that never touch the libc (and are only
 * expanded by the *_l units, which need locale_t from <ctype.h> anyway).
 */
# ifdef CT_HAVE_NEWLOCALE

/**
 * @brief An LC_CTYPE locale object for @p name (newlocale()).
 *
 * @param name Locale name ("POSIX", "C.UTF-8", ...).
 * @return The object, or (locale_t)0 if it cannot be created.
 */
static inline locale_t	ct_newLocale(const char *name)
{
	return (newlocale(LC_CTYPE_MASK, name, (locale_t)0));
}

/**
 * @brief Free an object of ct_newLocale() (none: nothing to do).
 *
 * @param loc Object, or (locale_t)0.
 */
static inline void	ct_freeLocale(locale_t loc)
{
	if (loc != (locale_t)0)
		freelocale(loc);
}

# else
#  define ct_newLocale(name)	((void)(name), (locale_t)0)
#  define ct_freeLocale(loc)	((void)(loc))
# endif

/**
 * @brief The cases of a *_l function (POSIX.1-2008).
 *
 *   prototype                    int FL(int, locale_t)
 *   eof                          FL(EOF, loc) is what F(EOF) is
 *   posix_object_is_c_locale     with an LC_CTYPE object of "POSIX", FL
 *                                agrees with F in the "C" locale
 *   ignores_the_global_locale    that object keeps its classes whatever
 *                                setlocale() did
 *   follows_its_locale_argument  with an object of another locale, FL
 *                                agrees with F under that locale
 *
 * The cases that need newlocale() / freelocale() (<locale.h>) skip when
 * the libc does not have them.
 *
 * @param FL   The *_l function (isalpha_l).
 * @param F    Its plain version (isalpha).
 * @param NORM CT_TRUTH (classification) or CT_VALUE (tolower_l, toupper_l).
 * @param REF  XSH page, e.g. "POSIX isalpha_l()".
 */
# define CT_LOCALE_TESTS(FL, F, NORM, REF)									\
																			\
static int		ct_ref_##FL[CT_UCHAR_MAX + 1];								\
static locale_t	ct_posix_##FL;												\
																			\
TEST_OPT(prototype, .ref = REF " SYNOPSIS")									\
{																			\
	EXPECT_TYPE(&FL, int (*)(int, locale_t));								\
}																			\
																			\
TEST_OPT(eof, .ref = REF " DESCRIPTION")									\
{																			\
	locale_t	loc;														\
																			\
	CT_NEED_NEWLOCALE();													\
	loc = ct_newLocale("POSIX");											\
	ASSERT_TRUE(loc != (locale_t)0);										\
	EXPECT_EQ(NORM(FL(EOF, loc)), NORM(F(EOF)));							\
	EXPECT_EQ(NORM((FL)(EOF, loc)), NORM(F(EOF)));							\
	ct_freeLocale(loc);														\
}																			\
																			\
TEST_OPT(posix_object_is_c_locale, .ref = REF " DESCRIPTION; XBD 7.2")		\
{																			\
	locale_t	loc;														\
	int			first_wrong_value = -1;										\
																			\
	CT_NEED_NEWLOCALE();													\
	loc = ct_newLocale("POSIX");											\
	ASSERT_TRUE(loc != (locale_t)0);										\
	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)		\
		if (NORM(FL(c, loc)) != NORM(F(c))									\
			|| NORM((FL)(c, loc)) != NORM(F(c)))							\
			first_wrong_value = c;											\
	EXPECT_EQ(first_wrong_value, -1);										\
	ct_freeLocale(loc);														\
}																			\
																			\
static void	ct_global_##FL(const char *name)								\
{																			\
	int	first_wrong_value = -1;												\
																			\
	(void)name;																\
	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)		\
		if (NORM(FL(c, ct_posix_##FL)) != ct_ref_##FL[c])					\
			first_wrong_value = c;											\
	EXPECT_EQ(first_wrong_value, -1);										\
}																			\
																			\
TEST_OPT(ignores_the_global_locale, .ref = REF " DESCRIPTION")				\
{																			\
	int	n;																	\
																			\
	CT_NEED_NEWLOCALE();													\
	ct_posix_##FL = ct_newLocale("POSIX");									\
	ASSERT_TRUE(ct_posix_##FL != (locale_t)0);								\
	for (int c = 0; c <= CT_UCHAR_MAX; c++)									\
		ct_ref_##FL[c] = NORM(F(c));										\
	n = ct_forEachLocale(ct_global_##FL);									\
	ct_freeLocale(ct_posix_##FL);											\
	CT_SKIP_LOCALES(n);														\
}																			\
																			\
static void	ct_arg_##FL(const char *name)									\
{																			\
	locale_t	loc = ct_newLocale(name);									\
	int			first_wrong_value = -1;										\
																			\
	EXPECT_TRUE(loc != (locale_t)0);										\
	if (loc == (locale_t)0)													\
		return ;															\
	for (int c = 0; c <= CT_UCHAR_MAX && first_wrong_value < 0; c++)		\
		if (NORM(FL(c, loc)) != NORM(F(c)))									\
			first_wrong_value = c;											\
	EXPECT_EQ(first_wrong_value, -1);										\
	ct_freeLocale(loc);														\
}																			\
																			\
TEST_OPT(follows_its_locale_argument, .ref = REF " DESCRIPTION")			\
{																			\
	CT_NEED_NEWLOCALE();													\
	CT_SKIP_LOCALES(ct_forEachLocale(ct_arg_##FL));							\
}

#endif /* CTYPE_CASES_H */
