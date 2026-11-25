/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file archive.c
 * @brief lcrun --ar-index and --libc-key: what of a static libc a link uses.
 * @Created: 2026/11/25 09:32:23 by Moutig
 * @Updated: 2026/11/25 09:50:23 by Moutig
 *
 * A unit (or a link probe) only has to be linked again when a member of
 * the libc archive it pulled changed. make (mk/tests.mk) asks:
 *
 *   --ar-index ARCHIVE
 *       one line per member of the archive, "<name>\t<hash of its
 *       content>", in archive order; the symbol table ("/", written by
 *       ar s) comes first, hashed as its (symbol, member name) pairs: it
 *       changes when a symbol is added, removed or moves to another
 *       member, not when a member only changes size (the table also
 *       holds the offsets of the members).
 *   --libc-key INDEX ARCHIVE MAP
 *       the key of one link: a hash of the symbol table and of every
 *       member MAP (the linker map, -Wl,-Map) says was pulled from
 *       ARCHIVE. Without MAP (the link failed, or never ran), the hash of
 *       the whole index: any change of the archive tries again.
 *
 * The symbol table is part of every key: a new member can change which
 * member resolves a symbol. Two members may have the same name (two
 * exit.o from different directories): a name stands for all of them.
 *
 * Only the common ar format is read (GNU and BSD names); a thin archive
 * is refused.
 */

#include "runner/lcrun.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** FNV-1a 64-bit offset basis and prime. */
#define LC_FNV_BASIS	0xcbf29ce484222325ULL
#define LC_FNV_PRIME	0x100000001b3ULL

/**
 * @brief Add @p n bytes to an FNV-1a hash.
 *
 * @param h Hash so far.
 * @param p Bytes.
 * @param n Number of bytes.
 * @return New hash.
 */
static uint64_t	fnv(uint64_t h, const void *p, size_t n)
{
	const unsigned char	*b = p;

	for (size_t i = 0; i < n; i++)
		h = (h ^ b[i]) * LC_FNV_PRIME;
	return (h);
}

/**
 * @brief Read a whole file.
 *
 * @param path File.
 * @param len  Receives its size.
 * @return New NUL-terminated buffer, or NULL if it cannot be read.
 */
static char	*slurp(const char *path, size_t *len)
{
	FILE	*fp = fopen(path, "rb");
	char	*buf = NULL;
	size_t	n = 0;
	char	tmp[1 << 16];
	size_t	got;

	if (fp == NULL)
		return (NULL);
	while ((got = fread(tmp, 1, sizeof(tmp), fp)) > 0)
	{
		buf = xrealloc(buf, n + got + 1);
		memcpy(buf + n, tmp, got);
		n += got;
	}
	fclose(fp);
	buf = xrealloc(buf, n + 1);
	buf[n] = '\0';
	*len = n;
	return (buf);
}

/**
 * @brief Name of a member from its header and the long-name table.
 *
 * "/" and "/SYM64/" are the symbol table ("/" for both); "//" the
 * long-name table; "/123" an offset in it; "name/" a GNU short name;
 * "#1/len" a BSD name stored first in the data (@p skip set to len).
 *
 * @param raw   The 16-byte name field.
 * @param names Long-name table, or NULL.
 * @param nLen  Its size.
 * @param out   Destination (at least 256 bytes).
 * @param skip  Receives the bytes of data that hold the name (BSD).
 * @param data  The member's data (BSD names).
 */
static void	memberName(const char *raw, const char *names, size_t nLen,
		char *out, size_t *skip, const char *data)
{
	char	f[17];
	size_t	n;

	memcpy(f, raw, 16);
	f[16] = '\0';
	for (n = 16; n > 0 && f[n - 1] == ' '; n--)
		f[n - 1] = '\0';
	*skip = 0;
	if (!strcmp(f, "/") || !strcmp(f, "/SYM64/"))
		snprintf(out, 256, "/");
	else if (!strcmp(f, "//"))
		snprintf(out, 256, "//");
	else if (f[0] == '/' && names)
	{
		size_t	off = strtoul(f + 1, NULL, 10);
		size_t	k = 0;

		while (off + k < nLen && names[off + k] != '/' && names[off + k] != '\n'
			&& k < 255)
			k++;
		snprintf(out, 256, "%.*s", (int)k, off < nLen ? names + off : "");
	}
	else if (!strncmp(f, "#1/", 3))
	{
		*skip = strtoul(f + 3, NULL, 10);
		snprintf(out, 256, "%.*s", (int)(*skip < 255 ? *skip : 255), data);
	}
	else
	{
		n = strlen(f);
		if (n && f[n - 1] == '/')
			f[n - 1] = '\0';
		snprintf(out, 256, "%s", f);
	}
}

/** One member of the archive (toolArIndex()). */
typedef struct s_member
{
	size_t		off;		/* Offset of its header in the archive. */
	char		name[256];	/* Name ("/" for the symbol table). */
	uint64_t	hash;		/* Hash of its content. */
	const char	*data;		/* Content (after a BSD name). */
	size_t		size;		/* Bytes of content. */
	int			sym64;		/* Symbol table with 64-bit offsets (/SYM64/). */
}	t_member;

/**
 * @brief Big-endian unsigned integer of @p n bytes (4 or 8).
 *
 * @param p Bytes.
 * @param n Width.
 * @return Value.
 */
static uint64_t	beUint(const unsigned char *p, int n)
{
	uint64_t	v = 0;

	for (int i = 0; i < n; i++)
		v = (v << 8) | p[i];
	return (v);
}

/**
 * @brief Hash of a GNU symbol table as its (symbol, member name) pairs.
 *
 * Layout: a count N, N member offsets, then N NUL-terminated names (4 or
 * 8 bytes per number, big-endian). An offset is replaced by the name of
 * the member it points to, so that a member that only changes size does
 * not change the table. Another layout is hashed as raw bytes.
 *
 * @param sym Symbol table member.
 * @param m   Every member.
 * @param n   Number of members.
 * @return Hash.
 */
static uint64_t	symtabHash(const t_member *sym, const t_member *m, size_t n)
{
	const unsigned char	*p = (const unsigned char *)sym->data;
	int					w = sym->sym64 ? 8 : 4;
	uint64_t			count;
	const char			*s;
	const char			*end = sym->data + sym->size;
	uint64_t			h = LC_FNV_BASIS;

	if (sym->size < (size_t)w)
		return (sym->hash);
	count = beUint(p, w);
	if (count > (sym->size - (size_t)w) / (size_t)w)
		return (sym->hash);
	s = sym->data + w + count * (uint64_t)w;
	for (uint64_t i = 0; i < count && s < end; i++)
	{
		uint64_t	off = beUint(p + w + i * (uint64_t)w, w);
		size_t		slen = strnlen(s, (size_t)(end - s));
		const char	*owner = "?";

		for (size_t k = 0; k < n; k++)
			if (m[k].off == off)
				owner = m[k].name;
		h = fnv(h, s, slen + 1);
		h = fnv(h, owner, strlen(owner) + 1);
		s += slen + 1;
	}
	return (h);
}

int	toolArIndex(const char *archive)
{
	size_t		len;
	char		*a = slurp(archive, &len);
	size_t		off = 8;
	const char	*names = NULL;
	size_t		nLen = 0;
	t_member	*m = NULL;
	size_t		n = 0;

	if (a == NULL || len < 8 || memcmp(a, "!<arch>\n", 8) != 0)
	{
		fprintf(stderr, "lcrun: %s: not an ar archive%s\n", archive,
			a && len >= 8 && !memcmp(a, "!<thin>\n", 8) ? " (thin archives are not supported)" : "");
		free(a);
		return (2);
	}
	while (off + 60 <= len)
	{
		const char	*hdr = a + off;
		size_t		size = strtoul(hdr + 48, NULL, 10);
		const char	*data = hdr + 60;
		size_t		skip;
		t_member	x = {0};

		if (off + 60 + size > len)
			break ;
		memberName(hdr, names, nLen, x.name, &skip, data);
		if (!strcmp(x.name, "//"))
		{
			names = data;
			nLen = size;
		}
		else if (skip <= size)
		{
			x.off = off;
			x.data = data + skip;
			x.size = size - skip;
			x.hash = fnv(LC_FNV_BASIS, x.data, x.size);
			x.sym64 = !strncmp(hdr, "/SYM64/", 7);
			m = xrealloc(m, (n + 1) * sizeof(*m));
			m[n++] = x;
		}
		off += 60 + size + (size & 1);
	}
	for (size_t i = 0; i < n; i++)
		if (!strcmp(m[i].name, "/"))
			printf("/\t%016llx\n", (unsigned long long)symtabHash(&m[i], m, n));
	for (size_t i = 0; i < n; i++)
		if (strcmp(m[i].name, "/") != 0)
			printf("%s\t%016llx\n", m[i].name, (unsigned long long)m[i].hash);
	free(m);
	free(a);
	return (0);
}

/**
 * @brief qsort() order of strings.
 *
 * @param a First (char **).
 * @param b Second.
 * @return Negative, zero or positive.
 */
static int	cmpStr(const void *a, const void *b)
{
	return (strcmp(*(char *const *)a, *(char *const *)b));
}

/**
 * @brief Every member of @p archive the linker map @p map pulled, sorted,
 *        without duplicates ("<archive>(<member>)" in the map).
 *
 * @param map     Map text.
 * @param archive Archive path, as given to the linker.
 * @param n       Receives the number of members.
 * @return New array of new strings.
 */
static char	**pulledMembers(const char *map, const char *archive, size_t *n)
{
	char		**v = NULL;
	size_t		alen = strlen(archive);
	const char	*p = map;

	*n = 0;
	while ((p = strstr(p, archive)) != NULL)
	{
		const char	*end;

		p += alen;
		if (*p != '(' || (end = strchr(p, ')')) == NULL)
			continue ;
		v = xrealloc(v, (*n + 1) * sizeof(*v));
		v[(*n)++] = xstrndup(p + 1, (size_t)(end - p - 1));
		p = end;
	}
	if (*n)
		qsort(v, *n, sizeof(*v), cmpStr);
	return (v);
}

/**
 * @brief Add to @p h the hashes of every member named @p name in the
 *        index ("?" if there is none).
 *
 * @param h     Hash so far.
 * @param index Index text (--ar-index).
 * @param name  Member name.
 * @return New hash.
 */
static uint64_t	addMember(uint64_t h, const char *index, const char *name)
{
	size_t		nlen = strlen(name);
	int			found = 0;

	h = fnv(h, name, nlen + 1);
	for (const char *l = index; *l; )
	{
		const char	*eol = strchr(l, '\n');
		size_t		ll = eol ? (size_t)(eol - l) : strlen(l);

		if (ll > nlen && !strncmp(l, name, nlen) && l[nlen] == '\t')
		{
			h = fnv(h, l + nlen + 1, ll - nlen - 1);
			found = 1;
		}
		l += ll + (eol != NULL);
	}
	if (!found)
		h = fnv(h, "?", 1);
	return (h);
}

int	toolLibcKey(const char *indexPath, const char *archive, const char *mapPath)
{
	size_t		len;
	char		*index = slurp(indexPath, &len);
	char		*map = slurp(mapPath, &len);
	char		**m;
	size_t		n = 0;
	uint64_t	h = LC_FNV_BASIS;

	if (index == NULL)
	{
		fprintf(stderr, "lcrun: %s: cannot read the archive index\n", indexPath);
		free(map);
		return (2);
	}
	m = map ? pulledMembers(map, archive, &n) : NULL;
	if (n == 0)
		h = fnv(fnv(h, "all", 3), index, strlen(index));
	else
	{
		h = addMember(h, index, "/");
		for (size_t i = 0; i < n; i++)
			if (i == 0 || strcmp(m[i], m[i - 1]) != 0)
				h = addMember(h, index, m[i]);
	}
	printf("%016llx\n", (unsigned long long)h);
	for (size_t i = 0; i < n; i++)
		free(m[i]);
	free(m);
	free(map);
	free(index);
	return (0);
}
