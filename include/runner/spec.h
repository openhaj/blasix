/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file spec.h
 * @brief Standards, option groups, the spec database, requirements (spec/).
 * @Created: 2026/11/24 08:28:19 by Moutig
 * @Updated: 2026/11/24 08:29:19 by Moutig
 *
 * spec/standards.c  the 8 standards, option groups, targets and their flags;
 * spec/database.c   loading spec/<category>.spec;
 * spec/require.c    what a target requires of an interface;
 * spec/visibility.c what a header must hide (strict mode) or refuse.
 */

#ifndef LCRUN_SPEC_H
# define LCRUN_SPEC_H

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include <stddef.h>
# include <stdio.h>

/**
 * @brief One of the 8 standards a libc can be checked against.
 *
 * ISO C editions have posix == 0. A POSIX edition is built on an ISO C
 * edition, its "C base", given by @c c.
 */
typedef struct s_std
{
	const char	*id;		/* Command-line id: posix-2024. */
	const char	*name;		/* Spec name: POSIX.1-2024. */
	int			c;			/* ISO C level (C99=1 ... C23=4). */
	int			posix;		/* POSIX level (0 for a pure ISO C target). */
	long		posixMacro;	/* _POSIX_C_SOURCE value. */
	int			xopen;		/* _XOPEN_SOURCE value when XSI is on. */
	const char	*cstd;		/* -std= value. */
}	t_std;

/**
 * @brief What a libc is checked against: a standard, options, extensions.
 */
typedef struct s_target
{
	const t_std	*std;		/* The standard. */
	char		**opts;		/* Option groups turned on (XSI, THREADS, ...). */
	int			ext;		/* Include extensions (spec entries since EXT). */
}	t_target;

/** One option group (spec.c): a POSIX margin code or an ISO C feature. */
typedef struct s_optdef
{
	const char	*name;		/* XSI, TSF, ..., THREADS, LIB_EXT1. */
	const char	*from;		/* ISO C feature: conditional from this edition;
							   NULL for a POSIX option. */
	const char	*want;		/* Macro defined when selected, or NULL. */
	const char	*desc;		/* Description, for --options. */
}	t_optdef;

/** What an interface is: decides how it is probed. */
typedef enum e_kind
{
	KIND_FUNC,		/* A function (maybe also a macro). */
	KIND_MACRO,		/* A macro (object-like or function-like). */
	KIND_TYPE,		/* A type. */
	KIND_EITHER,	/* A macro or a function: setjmp, errno, generic functions. */
	KIND_VAR,		/* An object with external linkage: optarg, environ. */
}	t_kind;

/**
 * One interface of the spec database. An interface of both ISO C and
 * POSIX may have one line per ladder (memccpy: C23, and POSIX.1-2001
 * XSI): the second line is @c alt; the interface is required if either
 * line requires it.
 */
typedef struct s_entry
{
	char			*cat;		/* spec file name (= src/suites/ directory). */
	char			*header;	/* Header to include: string.h, sys/stat.h. */
	char			*name;		/* Identifier. */
	t_kind			kind;		/* Function, macro, type, either, var. */
	char			*since;		/* Version that introduced it, or EXT. */
	char			*opt;		/* Option group (XSI, THREADS, ...), NULL = base. */
	char			*base;		/* Version where it moved to the base. */
	char			*ob;		/* Obsolescent from (comma list). */
	char			*removed;	/* Removed in (comma list). */
	char			*ext;		/* Extension family (GNU, BSD). */
	char			*same;		/* Other headers that declare it too (comma
								   list): the probe checks they agree. */
	char			*file;		/* spec file of the line, for messages. */
	int				line;		/* Line in that file. */
	struct s_entry	*alt;		/* Line of the other ladder, or NULL. */
}	t_entry;

/**
 * Names a header may declare beyond what the target requires
 * (`reserved` lines of a spec file): ISO C future library directions,
 * XSH 2.2.2 prefixes and suffixes. Each pattern holds from its edition on.
 */
typedef struct s_hdr
{
	char	*cat;		/* Category (spec file name). */
	char	**pat;		/* fnmatch() patterns: "str[a-z]*", "*_t". */
	char	**since;	/* Edition from which each pattern holds. */
	size_t	n;			/* Number of patterns. */
}	t_hdr;

/** The spec database: every spec/<cat>.spec, sorted by category and name. */
typedef struct s_spec
{
	t_entry	*v;			/* Entries (one per interface). */
	size_t	n;			/* Number of entries. */
	t_hdr	*hdrs;		/* Reserved names, one per category. */
	size_t	nHdrs;		/* Number of categories. */
	int		errors;		/* Parse errors (already reported on stderr). */
}	t_spec;

/** What a target says of one interface. */
typedef enum e_req
{
	REQ_NOT_YET,	/* Newer than the target. */
	REQ_REMOVED,	/* Removed in or before the target. */
	REQ_OPTION_OFF,	/* In an option group that is not selected. */
	REQ_BASE,		/* Required. */
	REQ_OPTION,		/* Required by a selected option group. */
	REQ_EXT,		/* Extension, selected with EXT=1. */
}	t_req;

/**
 * May the header make an interface visible in a target? (strict mode)
 *
 * A required interface must be declared. One that is not (not yet,
 * removed, option off, extension off) "shall not be made visible"
 * (XSH 2.2.1; ISO C 4p6 and 7.1.3 for a strictly conforming program),
 * unless its name is reserved for the header (spec `reserved` lines,
 * leading underscore). Annex K follows K.3.1.1 instead: with
 * __STDC_WANT_LIB_EXT1__ defined as 0 its identifiers are not declared,
 * reserved or not.
 */
typedef enum e_vis
{
	VIS_REQUIRED,	/* Required: must be declared (declaration probe). */
	VIS_ALLOWED,	/* Not required, reserved name: may be declared. */
	VIS_HIDDEN,		/* Not required, not reserved: must not be declared. */
	VIS_UNCHECKED,	/* The header is not part of the target. */
}	t_vis;

/**
 * @brief Find a standard by id (posix-2024) or name (POSIX.1-2024).
 *
 * @param s Id or name, case-insensitive.
 * @return The standard, or NULL if unknown.
 */
const t_std		*stdFind(const char *s);

/**
 * @brief The i-th standard, in the order C99 ... C23, POSIX.1-2001 ... 2024.
 *
 * @param i Index.
 * @return The standard, or NULL past the last one.
 */
const t_std		*stdAt(size_t i);

/**
 * @brief Is @p version usable in a spec file or in .since / .until?
 *
 * @param version A standard id or name, or "EXT".
 * @return 1 if valid, 0 otherwise.
 */
int				stdValid(const char *version);

/**
 * @brief Print the standards, one per line (--standards).
 *
 * @param fp Output stream.
 */
void			stdPrintAll(FILE *fp);

/**
 * @brief Does target @p t include @p version?
 *
 * An ISO C edition is included when it is not newer than the target's
 * C base; a POSIX edition when the target is a POSIX edition not older
 * than it; "EXT" when extensions are on.
 *
 * @param t       Target.
 * @param version Standard name, "EXT", or NULL / "" (always included).
 * @return 1 if included, 0 otherwise.
 */
int				stdIncludes(const t_target *t, const char *version);

/**
 * @brief Does target @p t include any version of a comma list?
 *
 * @param t    Target.
 * @param list "POSIX.1-2008,C23", or NULL (never included).
 * @return 1 if one of them is included, 0 otherwise.
 */
int				stdIncludesAny(const t_target *t, const char *list);

/**
 * @brief Find an option group by name (case-insensitive).
 *
 * @param name XSI, THREADS, ...
 * @return The option group, or NULL if unknown.
 */
const t_optdef	*optFind(const char *name);

/**
 * @brief The i-th option group (ISO C features first, then POSIX codes).
 *
 * @param i Index.
 * @return The option group, or NULL past the last one.
 */
const t_optdef	*optAt(size_t i);

/**
 * @brief Check the options of a target.
 *
 * @param t Target.
 * @return 0 if every option is known and fits the target (no POSIX
 *         option on an ISO C target), -1 otherwise (reported on stderr).
 */
int				targetCheck(const t_target *t);

/**
 * @brief Is option group @p opt selected in @p t?
 *
 * @param t   Target.
 * @param opt Option name, case-insensitive.
 * @return 1 if selected, 0 otherwise.
 */
int				targetHasOpt(const t_target *t, const char *opt);

/**
 * @brief What target @p t says of interface @p e.
 *
 * Each line of the entry (and of its other ladder, if any) is decided
 * separately; the strongest requirement wins (base > option > extension
 * > option off > removed > not yet).
 *
 * @param e  Entry of the spec.
 * @param t  Target.
 * @param ob Set to 1 if the interface is obsolescent in the target.
 * @return The requirement.
 */
t_req			specRequired(const t_entry *e, const t_target *t, int *ob);

/**
 * @brief Does @p r put the interface in the run (base, option, extension)?
 *
 * @param r Requirement.
 * @return 1 if selected, 0 otherwise.
 */
int				reqSelected(t_req r);

/**
 * @brief Name of a requirement, for listings ("option off", ...).
 *
 * @param r Requirement.
 * @return Static string.
 */
const char		*reqName(t_req r);

/**
 * @brief May the header declare @p e in target @p t? (see t_vis)
 *
 * @param sp   Database (for the reserved names and the header's status).
 * @param e    Entry.
 * @param t    Target.
 * @param why  Set to a static or database string saying why: the
 *             reserved pattern, "K.3.1.1p1", ... (may be NULL).
 * @param want Set to the want macro the hidden probe defines as 0
 *             (__STDC_WANT_LIB_EXT1__), or NULL (may be NULL).
 * @return The visibility.
 */
t_vis			specVisibility(const t_spec *sp, const t_entry *e, const t_target *t,
					const char **why, const char **want);

/**
 * @brief Does target @p t require at least one interface of category
 *        @p cat (is the header part of the target)?
 *
 * @param sp  Database.
 * @param cat Category.
 * @param t   Target.
 * @return 1 or 0.
 */
int				specHeaderInTarget(const t_spec *sp, const char *cat, const t_target *t);

/**
 * @brief Is @p e a definition (a type, or a macro without a unit of its
 *        own): its cases are in src/suites/bits/, run by <cat>/defs.c?
 *
 * Function-like macros (_tolower) have a unit like a function.
 *
 * @param e        Entry.
 * @param ownUnit  1 if a file <cat>/.../<name>.c exists.
 * @return 1 or 0.
 */
int				specIsDef(const t_entry *e, int ownUnit);

/** Result of a reject probe (<probedir>/<cat>/<id>.reject). */
typedef enum e_rej
{
	REJ_UNKNOWN,	/* Not built. */
	REJ_REJECTED,	/* Did not compile: the diagnostic required is there. */
	REJ_ACCEPTED,	/* Compiled: the diagnostic required is missing. */
}	t_rej;

/**
 * A reject probe: a translation unit the standard requires the
 * implementation to refuse ("shall issue a diagnostic as if a
 * preprocessor error directive were used"), generated for a target.
 *
 * K.3.1.1p4: __STDC_WANT_LIB_EXT1__ must be defined identically for all
 * inclusions of Annex K headers; one probe per header with Annex K
 * interfaces and per order (1 then 0, 0 then 1), when LIB_EXT1 is on.
 */
typedef struct s_reject
{
	const char	*cat;		/* Category of the header. */
	const char	*header;	/* Header included twice. */
	char		id[96];		/* File name: __want_<OPT>_<a>_<b>. */
	char		want[64];	/* Want macro (__STDC_WANT_LIB_EXT1__). */
	int			first;		/* Its value at the first inclusion. */
	int			second;		/* Its value at the second inclusion. */
	const char	*ref;		/* Clause that requires the diagnostic. */
	t_rej		st;			/* Result (filled by the run). */
}	t_reject;

/**
 * @brief The reject probes of a target.
 *
 * @param sp  Database.
 * @param t   Target.
 * @param out Set to a new array (free() it), NULL when there is none.
 * @return Number of probes.
 */
size_t			specRejects(const t_spec *sp, const t_target *t, t_reject **out);

/**
 * @brief Name of a visibility, for listings ("must be hidden", ...).
 *
 * @param v Visibility.
 * @return Static string.
 */
const char		*visName(t_vis v);

/**
 * @brief Load every <dir>/<cat>.spec into @p sp, sorted.
 *
 * Errors in the files are reported on stderr and counted in
 * sp->errors; the valid lines are still loaded.
 *
 * @param sp  Database to fill (overwritten).
 * @param dir Spec directory.
 * @return 0, or -1 if @p dir cannot be opened (errno set).
 */
int				specLoad(t_spec *sp, const char *dir);

/**
 * @brief Find an interface in the database.
 *
 * @param sp   Database.
 * @param cat  Category (spec file name).
 * @param name Identifier.
 * @return The entry, or NULL.
 */
const t_entry	*specFind(const t_spec *sp, const char *cat, const char *name);

/**
 * @brief Compiler flags of a target: -std=, feature-test and want macros.
 *
 * @param t    Target.
 * @param buf  Destination.
 * @param size Size of @p buf.
 */
void			targetCflags(const t_target *t, char *buf, size_t size);

/**
 * @brief Human description of a target: "POSIX.1-2024 + XSI".
 *
 * @param t    Target.
 * @param buf  Destination.
 * @param size Size of @p buf.
 */
void			targetDescribe(const t_target *t, char *buf, size_t size);

/**
 * @brief Name of a kind, as written in spec files ("function", ...).
 *
 * @param k Kind.
 * @return Static string.
 */
const char	*kindName(t_kind k);

#endif
