/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file realpath.c
 * @brief Tests for realpath() (<stdlib.h>, POSIX).
 * @Created: 2026/11/27 13:37:57 by Moutig
 * @Updated: 2026/11/27 13:40:07 by Moutig
 *
 * realpath(file_name, resolved_name) derives an absolute pathname that
 * names the same file (POSIX.1-2008 on: resolves to the same directory
 * entry) and whose resolution involves no '.', no '..' and no symbolic
 * link; it is stored in resolved_name (up to PATH_MAX bytes) and returned.
 * A null resolved_name: implementation-defined in POSIX.1-2001, a buffer
 * allocated as if by malloc() from 2008. On failure: a null pointer and
 * errno: EINVAL (file_name null), ENOENT (a missing component, or an
 * empty string), ENOTDIR (a prefix component that is not a directory;
 * from 2008 also a trailing slash after a file), ELOOP (a loop of
 * symbolic links), ENAMETOOLONG (a component longer than NAME_MAX),
 * EACCES (search permission denied for a component of the prefix).
 *
 * The files are made with raw syscalls in /tmp/lc-<pid>-rp; the expected
 * absolute paths start with what the kernel gives as the current
 * directory there (getcwd(2)), /tmp itself may be a symbolic link.
 *
 * XSI in POSIX.1-2001 to 2017, base in POSIX.1-2024.
 *
 * Clauses: POSIX: XSH realpath().
 */
#include "stdlib/stdlib_cases.h"
#include "framework/sys.h"
#include <limits.h>

TEST_OPT(prototype, .ref = "XSH realpath()")
{
	EXPECT_TYPE(&realpath, char *(*)(const char *restrict, char *restrict));
}

/* ----- The tree ----- */

/*
 *   D/file              a file
 *   D/sub/inner         a directory, a file in it
 *   D/link_file   -> file          D/link_dir -> sub
 *   D/link_abs    -> <R>/sub       D/chain    -> link_dir
 *   D/dangling    -> nothere       D/loop1 -> loop2 -> loop1
 *   D/closed/x          a directory without search permission
 */
static char	g_d[256];		/* D as created (/tmp/lc-<pid>-rp) */
static char	g_r[PATH_MAX];	/* D as the kernel resolves it */

/* g_d + "/" + rel, in a static buffer of its own (k: 0 ... 3). */
static const char	*inD(const char *rel, int k)
{
	static char	buf[4][PATH_MAX];
	size_t		n = lc_strlen(g_d);
	size_t		m = lc_strlen(rel);

	for (size_t i = 0; i < n; i++)
		buf[k][i] = g_d[i];
	buf[k][n] = '/';
	for (size_t i = 0; i <= m; i++)
		buf[k][n + 1 + i] = rel[i];
	return (buf[k]);
}

/* g_r + "/" + rel (rel "" gives g_r). */
static const char	*inR(const char *rel)
{
	static char	buf[PATH_MAX];
	size_t		n = lc_strlen(g_r);
	size_t		m = lc_strlen(rel);

	for (size_t i = 0; i < n; i++)
		buf[i] = g_r[i];
	if (m == 0)
	{
		buf[n] = '\0';
		return (buf);
	}
	buf[n] = '/';
	for (size_t i = 0; i <= m; i++)
		buf[n + 1 + i] = rel[i];
	return (buf);
}

static int	makeFile(const char *path)
{
	long	fd = lc_sysOpen(path, LC_O_WRONLY | LC_O_CREAT | LC_O_TRUNC | LC_O_CLOEXEC, 0644);

	if (fd < 0)
		return (0);
	(void)lc_sysClose((int)fd);
	return (1);
}

/*
 * Make the tree and chdir() into D (one process per test: the current
 * directory of the others is not changed); g_r is the kernel's getcwd()
 * there. 1 when it is all there.
 */
static int	makeTree(void)
{
	(void)lc_tmpPath(g_d, sizeof(g_d), "rp");
	if (lc_sysMkdir(g_d, 0755) != 0 || lc_sysChdir(g_d) != 0
		|| lc_sysGetcwd(g_r, sizeof(g_r)) <= 0)
		return (0);
	return (makeFile("file") && lc_sysMkdir("sub", 0755) == 0 && makeFile("sub/inner")
		&& lc_sysSymlink("file", "link_file") == 0 && lc_sysSymlink("sub", "link_dir") == 0
		&& lc_sysSymlink(inR("sub"), "link_abs") == 0 && lc_sysSymlink("link_dir", "chain") == 0
		&& lc_sysSymlink("nothere", "dangling") == 0 && lc_sysSymlink("loop2", "loop1") == 0
		&& lc_sysSymlink("loop1", "loop2") == 0 && lc_sysMkdir("closed", 0755) == 0
		&& makeFile("closed/x") && lc_sysChmod("closed", 0) == 0);
}

/* Remove the tree, so that nothing stays in /tmp (best effort). */
static void	removeTree(void)
{
	static const char	*links[] = {"link_file", "link_dir", "link_abs", "chain", "dangling",
		"loop1", "loop2", "file", "sub/inner"};

	if (lc_sysChdir(g_d) != 0)
		return ;
	(void)lc_sysChmod("closed", 0755);
	(void)lc_sysUnlink("closed/x");
	(void)lc_sysRmdir("closed");
	for (int i = 0; i < 9; i++)
		(void)lc_sysUnlink(links[i]);
	(void)lc_sysRmdir("sub");
	(void)lc_sysChdir("/");
	(void)lc_sysRmdir(g_d);
}

# define TREE()															\
	if (!makeTree())													\
	{																	\
		removeTree();													\
		SKIP("could not make the files in /tmp");						\
	}

static int	streq(const char *a, const char *b)
{
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return (*a == *b);
}

/* realpath(path, buf) returns buf holding want. */
static int	resolvesTo(const char *path, const char *want)
{
	static char	buf[PATH_MAX];
	char		*r = realpath(path, buf);

	if (r != buf || !streq(buf, want))
	{
		LOG(path);
		return (0);
	}
	return (1);
}

/* realpath(path, buf) fails with err. */
static int	failsWith(const char *path, int err)
{
	static char	buf[PATH_MAX];

	errno = 0;
	if (realpath(path, buf) != NULL || errno != err)
	{
		LOG(path);
		return (0);
	}
	return (1);
}

/* ----- Cases ----- */

TEST_OPT(absolute_path_of_a_file, .ref = "XSH realpath()")
{
	TREE();
	EXPECT_TRUE(resolvesTo(inD("file", 0), inR("file")));
	EXPECT_TRUE(resolvesTo(inD("sub/inner", 0), inR("sub/inner")));
	EXPECT_TRUE(resolvesTo(inD("sub", 0), inR("sub")));
	EXPECT_TRUE(resolvesTo(g_d, g_r));
	removeTree();
}

/* No '.', no '..', no repeated or trailing slash in the result. */
TEST_OPT(dot_dotdot_and_slashes_resolved, .ref = "XSH realpath()")
{
	TREE();
	EXPECT_TRUE(resolvesTo(inD("./sub/../file", 0), inR("file")));
	EXPECT_TRUE(resolvesTo(inD("sub/./inner", 0), inR("sub/inner")));
	EXPECT_TRUE(resolvesTo(inD("sub//.//inner", 0), inR("sub/inner")));
	EXPECT_TRUE(resolvesTo(inD("sub/..", 0), g_r));
	EXPECT_TRUE(resolvesTo(inD("sub/", 0), inR("sub")));
	EXPECT_TRUE(resolvesTo(inD("sub/../sub/../sub/inner", 0), inR("sub/inner")));
	removeTree();
}

/* Symbolic links: relative, absolute, to a directory, a link to a link. */
TEST_OPT(symbolic_links_resolved, .ref = "XSH realpath()")
{
	TREE();
	EXPECT_TRUE(resolvesTo(inD("link_file", 0), inR("file")));
	EXPECT_TRUE(resolvesTo(inD("link_dir", 0), inR("sub")));
	EXPECT_TRUE(resolvesTo(inD("link_dir/inner", 0), inR("sub/inner")));
	EXPECT_TRUE(resolvesTo(inD("link_abs/inner", 0), inR("sub/inner")));
	EXPECT_TRUE(resolvesTo(inD("chain/inner", 0), inR("sub/inner")));
	removeTree();
}

/* '..' after a link goes up from where the link leads (no link in the result). */
TEST_OPT(dotdot_after_a_link_is_physical, .ref = "XSH realpath()")
{
	TREE();
	EXPECT_TRUE(resolvesTo(inD("link_dir/../file", 0), inR("file")));
	EXPECT_TRUE(resolvesTo(inD("link_abs/..", 0), g_r));
	EXPECT_TRUE(resolvesTo(inD("chain/../link_file", 0), inR("file")));
	removeTree();
}

/* A relative path is resolved from the current directory. */
TEST_OPT(relative_path_from_the_current_directory, .ref = "XSH realpath()")
{
	TREE();
	ASSERT_EQ(lc_sysChdir("sub"), 0);
	EXPECT_TRUE(resolvesTo("inner", inR("sub/inner")));
	EXPECT_TRUE(resolvesTo(".", inR("sub")));
	EXPECT_TRUE(resolvesTo("..", g_r));
	EXPECT_TRUE(resolvesTo("../link_file", inR("file")));
	EXPECT_TRUE(resolvesTo("./../sub/./inner", inR("sub/inner")));
	removeTree();
}

TEST_OPT(root, .ref = "XSH realpath()")
{
	EXPECT_TRUE(resolvesTo("/", "/"));
	EXPECT_TRUE(resolvesTo("/.", "/"));
	EXPECT_TRUE(resolvesTo("/..", "/"));
	EXPECT_TRUE(resolvesTo("/../..", "/"));
}

/* POSIX.1-2008: a null resolved_name gives a buffer from malloc(), free() accepts it. */
TEST_OPT(null_buffer_is_allocated, .since = "POSIX.1-2008", .ref = "XSH realpath()")
{
	char	*a;
	char	*b;

	TREE();
	a = realpath(inD("link_dir/inner", 0), NULL);
	b = realpath("/", NULL);
	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	EXPECT_TRUE(streq(a, inR("sub/inner")));
	EXPECT_TRUE(streq(b, "/"));
	EXPECT_PTRNE(a, b);
	free(a);
	free(b);
	removeTree();
}

/* A missing component, an empty string, a dangling link: ENOENT. */
TEST_OPT(missing_file_gives_enoent, .ref = "XSH realpath() ERRORS")
{
	TREE();
	EXPECT_TRUE(failsWith(inD("nothere", 0), ENOENT));
	EXPECT_TRUE(failsWith(inD("nothere/inner", 0), ENOENT));
	EXPECT_TRUE(failsWith(inD("sub/nothere", 0), ENOENT));
	EXPECT_TRUE(failsWith(inD("dangling", 0), ENOENT));
	EXPECT_TRUE(failsWith("", ENOENT));
	removeTree();
}

/* A file in the prefix: ENOTDIR. */
TEST_OPT(file_in_the_prefix_gives_enotdir, .ref = "XSH realpath() ERRORS")
{
	TREE();
	EXPECT_TRUE(failsWith(inD("file/x", 0), ENOTDIR));
	EXPECT_TRUE(failsWith(inD("link_file/x", 0), ENOTDIR));
	EXPECT_TRUE(failsWith(inD("sub/inner/..", 0), ENOTDIR));
	removeTree();
}

/* POSIX.1-2008: a trailing slash after a file (not a directory): ENOTDIR. */
TEST_OPT(trailing_slash_after_a_file_gives_enotdir, .since = "POSIX.1-2008",
	.ref = "XSH realpath() ERRORS")
{
	TREE();
	EXPECT_TRUE(failsWith(inD("file/", 0), ENOTDIR));
	EXPECT_TRUE(failsWith(inD("link_file/", 0), ENOTDIR));
	EXPECT_TRUE(resolvesTo(inD("link_dir/", 0), inR("sub")));
	removeTree();
}

TEST_OPT(loop_of_links_gives_eloop, .ref = "XSH realpath() ERRORS")
{
	TREE();
	EXPECT_TRUE(failsWith(inD("loop1", 0), ELOOP));
	EXPECT_TRUE(failsWith(inD("loop2/x", 0), ELOOP));
	removeTree();
}

/* A component longer than NAME_MAX: ENAMETOOLONG. */
TEST_OPT(long_component_gives_enametoolong, .ref = "XSH realpath() ERRORS")
{
	static char	path[600];

	path[0] = '/';
	path[1] = 't';
	path[2] = 'm';
	path[3] = 'p';
	path[4] = '/';
	lc_fill(path + 5, 'a', 300);
	path[305] = '\0';
	EXPECT_TRUE(failsWith(path, ENAMETOOLONG));
	path[305] = '/';
	path[306] = 'b';
	path[307] = '\0';
	EXPECT_TRUE(failsWith(path, ENAMETOOLONG));
}

/* No search permission on a directory of the prefix: EACCES (not as root). */
TEST_OPT(no_search_permission_gives_eacces, .ref = "XSH realpath() ERRORS")
{
	TREE();
	if (lc_sysGeteuid() == 0)
	{
		removeTree();
		SKIP("running as root: permissions are not checked");
	}
	EXPECT_TRUE(failsWith(inD("closed/x", 0), EACCES));
	removeTree();
}

TEST_OPT(null_file_name_gives_einval, .ref = "XSH realpath() ERRORS")
{
	char	buf[PATH_MAX];

	errno = 0;
	EXPECT_NULL(realpath(NULL, buf));
	EXPECT_EQ(errno, EINVAL);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[PATH_MAX];
	char	b[PATH_MAX];

	EXPECT_PTREQ(realpath("/", a), a);
	EXPECT_PTREQ((realpath)("/", b), b);
	EXPECT_TRUE(streq(a, b));
}
