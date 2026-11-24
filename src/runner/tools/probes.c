/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file probes.c
 * @brief lcrun --spec-probes: the probe sources of a target.
 * @Created: 2026/11/24 08:19:06 by Moutig
 * @Updated: 2026/11/24 08:24:41 by Moutig
 *
 * One program per required interface (declared? linked?), one hidden
 * probe per interface the target forbids (strict mode: compiles = leak),
 * one reject probe per translation unit the standard requires to be
 * refused.
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @brief mkdir -p.
 *
 * @param path Directory.
 * @return 0 (also if it exists), -1 on error.
 */
static int	mkdirs(const char *path)
{
	char	tmp[4096];

	snprintf(tmp, sizeof(tmp), "%s", path);
	for (char *p = tmp + 1; *p; p++)
	{
		if (*p != '/')
			continue ;
		*p = '\0';
		mkdir(tmp, 0755);
		*p = '/';
	}
	return (mkdir(tmp, 0755) == 0 || errno == EEXIST ? 0 : -1);
}

/**
 * @brief Write the main() of the probe of @p e, by kind.
 *
 *   - function: take the address of (name), which bypasses a
 *     function-like macro: the function itself must exist;
 *   - macro: #ifndef name fails;
 *   - type: declare a pointer to it;
 *   - var: take its address;
 *   - either: a macro is enough, otherwise take the address.
 *
 * @param fp Probe source.
 * @param e  Interface.
 */
static void	probeBody(FILE *fp, const t_entry *e)
{
	const char	*n = e->name;

	switch (e->kind)
	{
		case KIND_MACRO:
			fprintf(fp, "#ifndef %s\n# error \"%s is not defined as a macro\"\n#endif\n\n"
				"int\tmain(void)\n{\n\treturn (0);\n}\n", n, n);
			break ;
		case KIND_TYPE:
			fprintf(fp, "int\tmain(void)\n{\n\t%s\t*volatile p = 0;\n\n"
				"\t(void)p;\n\treturn (0);\n}\n", n);
			break ;
		case KIND_VAR:
			fprintf(fp, "int\tmain(void)\n{\n\tconst volatile void\t*p = "
				"(const void *)&(%s);\n\n\treturn (p == 0);\n}\n", n);
			break ;
		case KIND_EITHER:
			/* A macro is enough; otherwise the identifier must exist. */
			fprintf(fp, "int\tmain(void)\n{\n#ifndef %s\n\tconst volatile void\t*p = "
				"(const void *)&(%s);\n\n\treturn (p == 0);\n#else\n\treturn (0);\n"
				"#endif\n}\n", n, n);
			break ;
		default:
			/* (name) suppresses a function-like macro: the function itself */
			fprintf(fp, "int\tmain(void)\n{\n\tvoid\t(*volatile p)(void) = "
				"(void (*)(void))(%s);\n\n\treturn (p == 0);\n}\n", n);
	}
}

/**
 * @brief Check that the other headers of same= declare @p e the same way.
 *
 * Written after the first #include, before main(). What it captures from
 * the first header must still agree once every other header is
 * included, or the probe does not compile (reported as undeclared):
 *   - type: a typedef of it and an object declared with both types
 *     (two declarations of one object need compatible types);
 *   - macro: its value, which must then be an integer constant, is
 *     compared after the other headers;
 *   - function, either, var: a conflicting redeclaration in the other
 *     header is already a compile error.
 *
 * @param fp Probe source.
 * @param e  Interface with a same= tag.
 */
static void	probeSame(FILE *fp, const t_entry *e)
{
	char	buf[256];
	char	*save;

	if (e->kind == KIND_TYPE)
		fprintf(fp, "typedef %s\tlc_same_t;\n", e->name);
	else if (e->kind == KIND_MACRO)
		fprintf(fp, "enum { lc_same_v = (int)(%s) };\n", e->name);
	snprintf(buf, sizeof(buf), "%s", e->same);
	for (char *h = strtok_r(buf, ",", &save); h; h = strtok_r(NULL, ",", &save))
		fprintf(fp, "#include <%s>\n", h);
	if (e->kind == KIND_TYPE)
		fprintf(fp, "extern lc_same_t\tlc_same_o;\nextern %s\tlc_same_o;\n", e->name);
	else if (e->kind == KIND_MACRO)
		fprintf(fp, "typedef char\tlc_same_chk[(lc_same_v == (int)(%s)) ? 1 : -1];\n",
			e->name);
	fprintf(fp, "\n");
}

/**
 * @brief Write <dir>/<cat>/<name>.c, the probe of one interface.
 *
 * @param e   Interface.
 * @param ext 1 for an extension: the probe defines _GNU_SOURCE & co.
 * @param dir Probe directory.
 * @return 0, or -1 if the file cannot be written.
 */
static int	writeProbe(const t_entry *e, int ext, const char *dir)
{
	char	path[4096];
	FILE	*fp;

	snprintf(path, sizeof(path), "%s/%s", dir, e->cat);
	if (mkdirs(path) < 0)
		return (-1);
	snprintf(path, sizeof(path), "%s/%s/%s.c", dir, e->cat, e->name);
	fp = fopen(path, "w");
	if (fp == NULL)
		return (-1);
	fprintf(fp, "/* probe: %s %s, <%s>, since %s (%s:%d) */\n",
		kindName(e->kind), e->name, e->header, e->since, e->file, e->line);
	if (ext)
		fprintf(fp, "#define _DEFAULT_SOURCE\n#define _GNU_SOURCE\n#define _BSD_SOURCE\n");
	fprintf(fp, "#include <%s>\n\n", e->header);
	if (e->same)
		probeSame(fp, e);
	probeBody(fp, e);
	return (fclose(fp));
}

/**
 * @brief Write <dir>/<cat>/<name>.hidden.c, the hidden probe of @p e.
 *
 * It includes the header in the target's context only (no extension
 * macro; the option's want macro defined as 0 for Annex K) and compiles
 * exactly when the name is visible: defined as a macro, or declared
 * (the declaration probe body). Compiled only, never linked.
 *
 * @param e    Interface the target forbids.
 * @param want Want macro to define as 0, or NULL.
 * @param why  Why it must be hidden (written in the probe, for the log).
 * @param dir  Probe directory.
 * @return 0, or -1 if the file cannot be written.
 */
static int	writeHidden(const t_entry *e, const char *want, const char *why,
		const char *dir)
{
	char	path[4096];
	FILE	*fp;

	snprintf(path, sizeof(path), "%s/%s", dir, e->cat);
	if (mkdirs(path) < 0)
		return (-1);
	snprintf(path, sizeof(path), "%s/%s/%s.hidden.c", dir, e->cat, e->name);
	fp = fopen(path, "w");
	if (fp == NULL)
		return (-1);
	fprintf(fp, "/* hidden probe: %s %s must not be visible in <%s> (%s); "
		"compiles = leak */\n", kindName(e->kind), e->name, e->header, why);
	if (want)
		fprintf(fp, "#define %s 0\n", want);
	fprintf(fp, "#include <%s>\n\n", e->header);
	if (e->kind == KIND_MACRO || e->kind == KIND_TYPE)
		probeBody(fp, e);
	else
	{
		/* Visible as a macro (a function-like macro is enough) or declared. */
		fprintf(fp, "#ifdef %s\nint\tmain(void)\n{\n\treturn (0);\n}\n#else\n", e->name);
		probeBody(fp, e);
		fprintf(fp, "#endif\n");
	}
	return (fclose(fp));
}

/**
 * @brief Write <dir>/<cat>/<id>.reject.c: a translation unit the
 *        implementation must refuse to compile.
 *
 * K.3.1.1p4: the header is included twice, with the want macro defined
 * as rj->first, then as rj->second. A conforming implementation "shall
 * issue a diagnostic as if a preprocessor error directive were used".
 *
 * @param rj  Reject probe.
 * @param dir Probe directory.
 * @return 0, or -1 if the file cannot be written.
 */
static int	writeReject(const t_reject *rj, const char *dir)
{
	char	path[4096];
	FILE	*fp;

	snprintf(path, sizeof(path), "%s/%s", dir, rj->cat);
	if (mkdirs(path) < 0)
		return (-1);
	snprintf(path, sizeof(path), "%s/%s/%s.reject.c", dir, rj->cat, rj->id);
	fp = fopen(path, "w");
	if (fp == NULL)
		return (-1);
	fprintf(fp, "/* reject probe (%s): %s defined as %d, then %d, for two inclusions "
		"of <%s>; the implementation shall refuse it as if by #error */\n",
		rj->ref, rj->want, rj->first, rj->second, rj->header);
	fprintf(fp, "#undef %s\n#define %s %d\n#include <%s>\n\n", rj->want, rj->want,
		rj->first, rj->header);
	fprintf(fp, "#undef %s\n#define %s %d\n#include <%s>\n\n", rj->want, rj->want,
		rj->second, rj->header);
	fprintf(fp, "int\tmain(void)\n{\n\treturn (0);\n}\n");
	return (fclose(fp));
}

int	toolProbes(const t_run *r, const char *dir)
{
	int	n = 0;
	int	nh = 0;

	for (size_t i = 0; i < r->spec.n; i++)
	{
		const t_entry	*e = &r->spec.v[i];
		int				ob;
		t_req			q = specRequired(e, &r->o.target, &ob);
		const char		*why;
		const char		*want;

		if (!reqSelected(q))
		{
			if (r->o.lax
				|| specVisibility(&r->spec, e, &r->o.target, &why, &want) != VIS_HIDDEN)
				continue ;
			if (writeHidden(e, want, why, dir) < 0)
			{
				perror(dir);
				return (2);
			}
			nh++;
			continue ;
		}
		if (writeProbe(e, q == REQ_EXT, dir) < 0)
		{
			perror(dir);
			return (2);
		}
		n++;
	}
	{
		t_reject	*rj;
		size_t		nr = specRejects(&r->spec, &r->o.target, &rj);

		for (size_t i = 0; i < nr; i++)
			if (writeReject(&rj[i], dir) < 0)
			{
				perror(dir);
				free(rj);
				return (2);
			}
		free(rj);
		fprintf(stderr, "lcrun: %d probes, %d hidden, %zu reject in %s\n", n, nh, nr, dir);
	}
	return (0);
}
