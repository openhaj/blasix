/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file lc.h
 * @brief Test-side API: TEST, EXPECT_*, ASSERT_*, helpers.
 * @Created: 2026/11/23 04:44:31 by Moutig
 * @Updated: 2026/11/24 05:13:13 by Moutig
 *
 * One test file = one function of the libc under test:
 *
 *     src/suites/<category>/<function>.c
 *
 * The Makefile derives LC_CATEGORY and LC_FUNCTION from that path,
 * so a test file only contains test bodies:
 *
 *     #include "framework/lc.h"
 *     #include <string.h>
 *
 *     TEST(empty)
 *     {
 *         EXPECT_UEQ(strlen(""), 0);
 *     }
 *
 * A test file depends on the function it tests and nothing else it can
 * avoid: preparing data, cleaning up and exiting go through the lc_*
 * helpers below, so a missing remove() or _Exit() in the libc does not
 * make the fopen or exit tests unavailable.
 *
 * Dependencies: none. This header includes no libc header and the
 * framework calls no libc function (raw syscalls only), so a broken
 * strlen, printf or malloc in the libc under test cannot break the
 * reporting. Tests are registered in the "lc_tests" ELF section, so
 * not even constructors are needed.
 *
 * Layout of this header:
 *   - expected outcome of a test (LC_NORMAL, LC_EXIT, LC_SIGNAL);
 *   - the test descriptor and the TEST / TEST_OPT / TEST_FN macros;
 *   - the check functions (lc.c) and the EXPECT_* / ASSERT_* macros
 *     built on them;
 *   - helpers that replace the libc in test bodies (lc_tmpPath, ...).
 */

#ifndef LC_H
# define LC_H

/*
 * A test file must be compiled by the Makefile, which names its
 * category and function. The framework sources define LC_FRAMEWORK to
 * skip the check.
 */
# ifndef LC_FRAMEWORK
#  ifndef LC_CATEGORY
#   error "LC_CATEGORY is set by the Makefile (src/suites/<category>/<function>.c)"
#  endif
#  ifndef LC_FUNCTION
#   error "LC_FUNCTION is set by the Makefile (src/suites/<category>/<function>.c)"
#  endif
# endif

/** size_t without <stddef.h>: no libc header is included here. */
typedef __SIZE_TYPE__	lc_size_t;

/* ----- Expected outcome ----- */

/*
 * Encoded in an int: 0 for a normal return, 0x100 | status for an exit,
 * 0x200 | signal for a death by signal. lcrun compares it with what the
 * test process actually did.
 */

/** The test must return normally (default). */
# define LC_NORMAL		0

/** The test must make the process exit with status @p n. */
# define LC_EXIT(n)		(0x100 | ((n) & 0xff))

/** The test must make the process die from signal @p s. */
# define LC_SIGNAL(s)	(0x200 | ((s) & 0xff))

/* ----- Test descriptor ----- */

/**
 * @brief One test case, placed in the "lc_tests" section.
 *
 * The linker gathers every descriptor of the binary between
 * __start_lc_tests and __stop_lc_tests; lc.c walks that array. Optional
 * fields are set with TEST_OPT(name, .field = value, ...) and are zero
 * (NULL) otherwise.
 */
struct lc_test
{
	const char	*category;	/* From the path (LC_CATEGORY). */
	const char	*function;	/* From the path (LC_FUNCTION) or TEST_FN. */
	const char	*name;		/* Case name. */
	const char	*file;		/* Source file (__FILE__), for failure messages. */
	int			line;		/* Line of the TEST macro (__LINE__). */
	int			timeout;	/* Seconds, 0 = runner default. */
	int			expect;		/* LC_NORMAL, LC_EXIT(n) or LC_SIGNAL(s). */
	const char	*since;		/* Behaviour required from this version on. */
	const char	*until;		/* Behaviour removed in this version. */
	const char	*opt;		/* Behaviour of an option group (XSI, ...). */
	const char	*ref;		/* Where the spec says so. */
	void		(*fn)(void);	/* Test body. */
};

/*
 * `retain` (GCC 11+, clang) keeps the descriptors when the link uses
 * --gc-sections; `used` alone only protects them from the compiler.
 */
# ifdef __has_attribute
#  if __has_attribute(retain)
#   define LC_RETAIN	__attribute__((retain))
#  endif
# endif
# ifndef LC_RETAIN
#  define LC_RETAIN
# endif

/** Put a descriptor in the "lc_tests" section, pointer-aligned. */
# define LC_SECTION	__attribute__((used, section("lc_tests"), \
						aligned(sizeof(void *)))) LC_RETAIN

/**
 * @brief Define a test descriptor and open the body that follows.
 *
 * @param id    Unique C identifier of the case in the file.
 * @param func  Function the case belongs to (string).
 * @param label Case name (string).
 * @param ...   Designated initializers of the optional fields.
 */
# define LC_DEFINE(id, func, label, ...)								\
	static void lc_body_##id(void);										\
	static const struct lc_test lc_desc_##id LC_SECTION = {				\
		.category = LC_CATEGORY, .function = func, .name = label,		\
		.file = __FILE__, .line = __LINE__, .fn = lc_body_##id,			\
		__VA_ARGS__														\
	};																	\
	static void lc_body_##id(void)

/** Test case of the file's function. */
# define TEST(name)					LC_DEFINE(name, LC_FUNCTION, #name, .timeout = 0)

/**
 * Same, with options (designated initializers, any order):
 *   .timeout = 30                      seconds
 *   .expect  = LC_EXIT(3) | LC_SIGNAL(SIGABRT)
 *   .since   = "POSIX.1-2008"          behaviour required from this version
 *   .until   = "POSIX.1-2024"          behaviour removed in this version
 *   .opt     = "XSI"                   behaviour of an option group (make options)
 *   .ref     = "C11 7.21.5.3p5"        where the spec says so (shown on failure)
 * A test whose since / until / opt excludes the target is not run; an
 * unknown version or option is reported as an error by lcrun.
 */
# define TEST_OPT(name, ...)		LC_DEFINE(name, LC_FUNCTION, #name, __VA_ARGS__)

/** Test case attached to another function (e.g. getc_unlocked in getc.c). */
# define TEST_FN(func, name)		LC_DEFINE(func##__##name, #func, #name, .timeout = 0)

/**
 * Test case of a symbol (type, constant), with options: the cases of
 * src/suites/bits/, instantiated by a <category>/defs.c. The result is
 * reported under the symbol, not under "defs".
 *
 *   id   unique identifier (symbol and name pasted: size_t__is_unsigned)
 *   sym  the symbol, a string ("size_t"; #M in a generator, so that a
 *        macro such as PRId8 is not expanded)
 *   name the case
 */
# define TEST_SYM(id, sym, name, ...)	LC_DEFINE(id, sym, #name, __VA_ARGS__)

/* ----- Checks (implemented in lc.c) ----- */

/*
 * Every check counts one assertion. On failure it reports the source
 * location, the text of the macro and the values involved, on fd 3
 * (@fail event, for lcrun) or on stderr (human output), then returns 0
 * so that the ASSERT_* macros can leave the test body. Tests call them
 * through the EXPECT_* / ASSERT_* macros, not directly.
 */

/**
 * @brief Check a condition.
 *
 * @param ok   Result of the condition (non-zero: passed).
 * @param file Source file of the check.
 * @param line Source line of the check.
 * @param text Text of the macro, e.g. "EXPECT_TRUE(x > 0)".
 * @return 1 if the check passed, 0 if it failed (and was reported).
 */
int		lc_check(int ok, const char *file, int line, const char *text);

/**
 * @brief Check a comparison of signed integers.
 *
 * @param ok   Result of `a op b`.
 * @param file Source file of the check.
 * @param line Source line of the check.
 * @param text Text of the macro, e.g. "EXPECT_EQ(f(), 3)".
 * @param a    Left operand, as long long.
 * @param b    Right operand, as long long.
 * @param op   Operator, e.g. "==", shown as "2 == 3 is false".
 * @return 1 if the check passed, 0 if it failed.
 */
int		lc_checkInt(int ok,
					const char	*file,	int			line,	const char	*text,
					long long	a,		long long	b,		const char	*op);

/**
 * @brief Check a comparison of unsigned integers (size_t, ...).
 *
 * @param ok   Result of `a op b`.
 * @param file Source file of the check.
 * @param line Source line of the check.
 * @param text Text of the macro.
 * @param a    Left operand, as unsigned long long.
 * @param b    Right operand, as unsigned long long.
 * @param op   Operator, e.g. "==".
 * @return 1 if the check passed, 0 if it failed.
 */
int		lc_checkUint(int ok,
					 const char			*file,	int					line,	const char	*text,
					 unsigned long long	a,		unsigned long long	b,		const char	*op);

/**
 * @brief Check a comparison of pointers.
 *
 * @param ok   Result of `a == b` or `a != b`.
 * @param file Source file of the check.
 * @param line Source line of the check.
 * @param text Text of the macro.
 * @param a    Left pointer.
 * @param b    Right pointer.
 * @return 1 if the check passed, 0 if it failed (both addresses shown).
 */
int		lc_checkPtr(int ok,
					const char	*file,	int			line, const char *text,
					const void	*a,		const void	*b);

/**
 * @brief Compare two NUL-terminated strings (NULL-safe).
 *
 * Two NULL pointers are equal; NULL and a string are different. On
 * failure both strings are shown quoted and escaped, with the index of
 * the first difference.
 *
 * @param wantEqual 1 for STREQ, 0 for STRNE.
 * @param file      Source file of the check.
 * @param line      Source line of the check.
 * @param text      Text of the macro.
 * @param a         First string, or NULL.
 * @param b         Second string, or NULL.
 * @return 1 if the check passed, 0 if it failed.
 */
int		lc_checkStr(int wantEqual,
					const char	*file,	int			line, const char *text,
					const char	*a,		const char	*b);

/**
 * @brief Compare @p n bytes of two memory areas.
 *
 * @param file Source file of the check.
 * @param line Source line of the check.
 * @param text Text of the macro.
 * @param a    First area (NULL fails unless both are NULL).
 * @param b    Second area.
 * @param n    Number of bytes.
 * @return 1 if the areas are equal, 0 otherwise (first differing byte
 *         and both values shown).
 */
int		lc_checkMem(const char	*file,	int			line,	const char	*text,
					const void	*a,		const void	*b,		lc_size_t	n);

/**
 * @brief Mark the current test as skipped (use SKIP, which also returns).
 *
 * @param reason Why: shown in the report.
 */
void	lc_skip(const char *reason);

/**
 * @brief Attach a message to the current test (use LOG).
 *
 * @param msg Message, shown with -v and on failure.
 */
void	lc_log(const char *msg);

/* ----- Assertion macros ----- */

/*
 * EXPECT_* records the failure and continues.
 * ASSERT_* records the failure and returns from the test body: use it
 * only directly in a TEST body (in a helper it would only return from
 * the helper).
 *
 * The operands are evaluated once, converted to the type of the check
 * (long long, unsigned long long, const void *), then compared.
 */

/** What an EXPECT_* does on failure: nothing. */
# define LC_ON_FAIL_EXPECT	(void)0

/** What an ASSERT_* does on failure: leave the test body. */
# define LC_ON_FAIL_ASSERT	return

/** Boolean check: @p cond must be @p want (1 for TRUE, 0 for FALSE). */
# define LC_BOOL(kind, mac, want, cond)									\
	do { if (!lc_check(!!(cond) == (want), __FILE__, __LINE__,			\
			#kind "_" mac "(" #cond ")")) LC_ON_FAIL_##kind; } while (0)

/** Signed integer check: `a op b`. */
# define LC_INT(kind, mac, a, op, b)									\
	do { long long lc_a_ = (long long)(a), lc_b_ = (long long)(b);		\
		if (!lc_checkInt(lc_a_ op lc_b_, __FILE__, __LINE__,			\
			#kind "_" mac "(" #a ", " #b ")", lc_a_, lc_b_, #op))		\
			LC_ON_FAIL_##kind; } while (0)

/** Unsigned integer check: `a op b`. */
# define LC_UINT(kind, mac, a, op, b)									\
	do { unsigned long long lc_a_ = (unsigned long long)(a);			\
		unsigned long long lc_b_ = (unsigned long long)(b);				\
		if (!lc_checkUint(lc_a_ op lc_b_, __FILE__, __LINE__,			\
			#kind "_" mac "(" #a ", " #b ")", lc_a_, lc_b_, #op))		\
			LC_ON_FAIL_##kind; } while (0)

/** Pointer check: `a op b` (== or !=). */
# define LC_PTR(kind, mac, a, op, b)									\
	do { const void *lc_a_ = (const void *)(a);							\
		const void *lc_b_ = (const void *)(b);							\
		if (!lc_checkPtr(lc_a_ op lc_b_, __FILE__, __LINE__,			\
			#kind "_" mac "(" #a ", " #b ")", lc_a_, lc_b_))			\
			LC_ON_FAIL_##kind; } while (0)

/** String check: equal (@p want 1) or different (@p want 0). */
# define LC_STR(kind, mac, want, a, b)									\
	do { if (!lc_checkStr(want, __FILE__, __LINE__,						\
			#kind "_" mac "(" #a ", " #b ")", (a), (b)))				\
			LC_ON_FAIL_##kind; } while (0)

/** Memory check: @p n bytes of @p a and @p b are equal. */
# define LC_MEM(kind, a, b, n)											\
	do { if (!lc_checkMem(__FILE__, __LINE__,							\
			#kind "_MEMEQ(" #a ", " #b ", " #n ")", (a), (b), (n)))		\
			LC_ON_FAIL_##kind; } while (0)

/* Boolean */
# define EXPECT_TRUE(c)			LC_BOOL(EXPECT, "TRUE", 1, c)
# define EXPECT_FALSE(c)		LC_BOOL(EXPECT, "FALSE", 0, c)
# define ASSERT_TRUE(c)			LC_BOOL(ASSERT, "TRUE", 1, c)
# define ASSERT_FALSE(c)		LC_BOOL(ASSERT, "FALSE", 0, c)

/* Signed integers (char, int, long, ssize_t, ...) */
# define EXPECT_EQ(a, b)		LC_INT(EXPECT, "EQ", a, ==, b)
# define EXPECT_NE(a, b)		LC_INT(EXPECT, "NE", a, !=, b)
# define EXPECT_LT(a, b)		LC_INT(EXPECT, "LT", a, <, b)
# define EXPECT_LE(a, b)		LC_INT(EXPECT, "LE", a, <=, b)
# define EXPECT_GT(a, b)		LC_INT(EXPECT, "GT", a, >, b)
# define EXPECT_GE(a, b)		LC_INT(EXPECT, "GE", a, >=, b)
# define ASSERT_EQ(a, b)		LC_INT(ASSERT, "EQ", a, ==, b)
# define ASSERT_NE(a, b)		LC_INT(ASSERT, "NE", a, !=, b)
# define ASSERT_LT(a, b)		LC_INT(ASSERT, "LT", a, <, b)
# define ASSERT_LE(a, b)		LC_INT(ASSERT, "LE", a, <=, b)
# define ASSERT_GT(a, b)		LC_INT(ASSERT, "GT", a, >, b)
# define ASSERT_GE(a, b)		LC_INT(ASSERT, "GE", a, >=, b)

/* Unsigned integers (size_t, uintmax_t, ...) */
# define EXPECT_UEQ(a, b)		LC_UINT(EXPECT, "UEQ", a, ==, b)
# define EXPECT_UNE(a, b)		LC_UINT(EXPECT, "UNE", a, !=, b)
# define EXPECT_ULT(a, b)		LC_UINT(EXPECT, "ULT", a, <, b)
# define EXPECT_UGT(a, b)		LC_UINT(EXPECT, "UGT", a, >, b)
# define ASSERT_UEQ(a, b)		LC_UINT(ASSERT, "UEQ", a, ==, b)
# define ASSERT_UNE(a, b)		LC_UINT(ASSERT, "UNE", a, !=, b)

/* Pointers */
# define EXPECT_PTREQ(a, b)		LC_PTR(EXPECT, "PTREQ", a, ==, b)
# define EXPECT_PTRNE(a, b)		LC_PTR(EXPECT, "PTRNE", a, !=, b)
# define EXPECT_NULL(p)			LC_PTR(EXPECT, "NULL", p, ==, 0)
# define EXPECT_NOTNULL(p)		LC_PTR(EXPECT, "NOTNULL", p, !=, 0)
# define ASSERT_PTREQ(a, b)		LC_PTR(ASSERT, "PTREQ", a, ==, b)
# define ASSERT_NULL(p)			LC_PTR(ASSERT, "NULL", p, ==, 0)
# define ASSERT_NOTNULL(p)		LC_PTR(ASSERT, "NOTNULL", p, !=, 0)

/* Strings (NUL-terminated, NULL-safe) and memory */
# define EXPECT_STREQ(a, b)		LC_STR(EXPECT, "STREQ", 1, a, b)
# define EXPECT_STRNE(a, b)		LC_STR(EXPECT, "STRNE", 0, a, b)
# define ASSERT_STREQ(a, b)		LC_STR(ASSERT, "STREQ", 1, a, b)
# define EXPECT_MEMEQ(a, b, n)	LC_MEM(EXPECT, a, b, n)
# define ASSERT_MEMEQ(a, b, n)	LC_MEM(ASSERT, a, b, n)

/*
 * Types: is the type of an expression compatible with a type? Checked at
 * run time (reported like any check, the unit still builds).
 *
 *     EXPECT_TYPE(&isalpha, int (*)(int));
 *
 * Through _Generic (C11; accepted as an extension in C99 mode by GCC and
 * clang), not __builtin_types_compatible_p: GCC records the const and
 * noreturn attributes in the function type, so a pointer to
 * `int ffs(int) __attribute__((const))` would not match int (*)(int).
 * _Generic converts the expression as an rvalue first (arrays decay,
 * top-level qualifiers drop): compare with the unqualified type.
 */

/** Is the type of @p expr compatible with @p type? (1 or 0, constant) */
# define LC_TYPE_IS(expr, type)	(__extension__ _Generic((expr), type: 1, default: 0))

# define EXPECT_TYPE(expr, type)	LC_BOOL(EXPECT, "TYPE", 1, LC_TYPE_IS(expr, type))
# define ASSERT_TYPE(expr, type)	LC_BOOL(ASSERT, "TYPE", 1, LC_TYPE_IS(expr, type))

/** Sign of a comparison result (strcmp, memcmp, ...): -1, 0 or 1. */
# define LC_SIGN(x)				(((x) > 0) - ((x) < 0))

/** Mark the test as skipped and leave the body. */
# define SKIP(reason)			do { lc_skip(reason); return; } while (0)

/** Attach a message to the test result (shown with -v and on failure). */
# define LOG(msg)				lc_log(msg)

/* ----- Helpers ----- */

/*
 * Replacements for the few libc functions a test needs around the
 * function it tests. They use raw syscalls (sys.c), so they work even
 * when the libc under test lacks the real ones.
 */

/**
 * @brief Build a unique temporary path: "/tmp/lc-<pid>-<tag>".
 *
 * The pid makes paths unique between tests running in parallel; the
 * tag between paths of the same test. The result is truncated to fit.
 *
 * @param buf  Destination buffer.
 * @param size Size of @p buf, including the terminating NUL.
 * @param tag  Suffix chosen by the test.
 * @return @p buf.
 */
char	*lc_tmpPath(char *buf, lc_size_t size, const char *tag);

/**
 * @brief Length of a string, without the libc under test.
 *
 * @param s NUL-terminated string.
 * @return Number of bytes before the terminating NUL.
 */
lc_size_t	lc_strlen(const char *s);

/**
 * @brief Fill memory, without the libc under test.
 *
 * Writes through a volatile pointer, so the compiler cannot turn the
 * loop into a call to memset().
 *
 * @param p Memory area.
 * @param c Byte value (converted to unsigned char).
 * @param n Number of bytes.
 */
void	lc_fill(void *p, int c, lc_size_t n);

/**
 * @brief Memory that ends right before an inaccessible page.
 *
 * Returns @p n writable bytes followed by a page with no access: a
 * function that reads (or writes) one byte past them crashes the test.
 * For the "shall not examine more than n bytes" requirements (strnlen,
 * memchr, strndup, ...). The bytes are not initialized (zero) and the
 * memory is never released (one process per test).
 *
 * @param n Number of usable bytes (may be 0).
 * @return Start of the @p n bytes, or NULL if the mapping failed.
 */
void	*lc_guarded(lc_size_t n);

/**
 * @brief Remove a file (cleanup), without the libc under test.
 *
 * @param path Path of the file.
 * @return 0 on success, -errno on failure.
 */
int		lc_unlink(const char *path);

/**
 * @brief End the process now (exit_group).
 *
 * No atexit handler runs and no stdio stream is flushed: for tests that
 * must stop without going through the libc's exit path.
 *
 * @param status Exit status.
 */
void	lc_exitNow(int status) __attribute__((noreturn));

/**
 * @brief Write the .gcda of every instrumented object of this process.
 *
 * Called by main() after a test run with --run, in coverage builds only
 * (weak symbol in lc.c).
 */
void	lc_covDump(void);

#endif /* LC_H */
