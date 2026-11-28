/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file sys_stat_cases.h
 * @brief Shared helpers of the <sys/stat.h> units.
 * @Created: 2026/11/28 12:20:36 by Moutig
 * @Updated: 2026/11/28 12:23:32 by Moutig
 *
 * The files the cases look at are made and read with raw syscalls
 * (framework/sys.h), never with the libc under test:
 *
 *   - ss_path, ss_makeFile: a fresh temporary path, a file holding n
 *     bytes;
 *   - ss_raw: what statx(2) says of a path or a descriptor (mode, size,
 *     inode, links, owner, mtime);
 *   - ss_objects: one object of each file type found on the system, with
 *     its st_mode as the kernel gives it: a regular file, a directory, a
 *     symbolic link, a FIFO (a pipe), a character device (/dev/null), and
 *     when one is there a block device and a socket. The S_IS* units test
 *     each macro on all of them.
 *
 * Only <sys/stat.h> is under test.
 */

#ifndef SYS_STAT_CASES_H
# define SYS_STAT_CASES_H

# include "framework/lc.h"
# include "framework/sys.h"
# include <sys/stat.h>

/** The file types of ss_objects. */
enum
{
	SS_REG,
	SS_DIR,
	SS_LNK,
	SS_FIFO,
	SS_CHR,
	SS_BLK,
	SS_SOCK,
	SS_KINDS
};

/** What statx(2) gives. */
typedef struct ss_st
{
	unsigned int		mode;
	unsigned int		nlink;
	unsigned int		uid;
	unsigned int		gid;
	unsigned long long	ino;
	unsigned long long	size;
	long long			mtimeSec;
	unsigned int		mtimeNsec;
}	ss_st;

/* statx of path (fd < 0; nofollow: a final link itself) or of fd (path NULL). 0 or -1. */
static inline int	ss_raw(int fd, const char *path, int nofollow, ss_st *st)
{
	union
	{
		unsigned long long	u64[32];
		unsigned int		u32[64];
		unsigned short		u16[128];
	}		raw;
	long	r;

	if (path)
		r = lc_sysStatx(LC_AT_FDCWD, path,
				nofollow ? LC_AT_SYMLINK_NOFOLLOW : 0, raw.u64);
	else
		r = lc_sysStatx(fd, "", LC_AT_EMPTY_PATH, raw.u64);
	if (r != 0)
		return (-1);
	st->mode = raw.u16[LC_STX_MODE / 2];
	st->nlink = raw.u32[LC_STX_NLINK / 4];
	st->uid = raw.u32[LC_STX_UID / 4];
	st->gid = raw.u32[LC_STX_GID / 4];
	st->ino = raw.u64[LC_STX_INO / 8];
	st->size = raw.u64[LC_STX_SIZE / 8];
	st->mtimeSec = (long long)raw.u64[LC_STX_MTIME / 8];
	st->mtimeNsec = raw.u32[LC_STX_MTIME / 4 + 2];
	return (0);
}

/* A temporary path for tag, with nothing there. */
static inline char	*ss_path(char *buf, unsigned long size, const char *tag)
{
	(void)lc_tmpPath(buf, size, tag);
	(void)lc_sysUnlink(buf);
	return (buf);
}

/* Make path a file of mode 0600 holding n bytes; open for reading and writing. fd or -1. */
static inline int	ss_makeFile(const char *path, unsigned long n)
{
	char	b[64];
	long	fd = lc_sysOpen(path, LC_O_RDWR | LC_O_CREAT | LC_O_TRUNC | LC_O_CLOEXEC, 0600);

	lc_fill(b, 'z', sizeof(b));
	if (fd < 0)
		return (-1);
	for (unsigned long done = 0; done < n; )
	{
		unsigned long	k = n - done < sizeof(b) ? n - done : sizeof(b);

		if (lc_sysWrite((int)fd, b, k) != (long)k)
			return ((void)lc_sysClose((int)fd), -1);
		done += k;
	}
	return ((int)fd);
}

/** The objects of ss_objects: st_mode of each kind, or 0 when none was found. */
typedef struct ss_objs
{
	unsigned int	mode[SS_KINDS];
	const char		*name[SS_KINDS];
}	ss_objs;

/* The first path of list (NULL-ended) that is of the file type ifmt, raw; its mode or 0. */
static inline unsigned int	ss_find(const char *const *list, unsigned int ifmt, const char **name)
{
	ss_st	st;

	for (; *list; list++)
		if (ss_raw(-1, *list, 1, &st) == 0 && (st.mode & 0170000) == ifmt)
			return ((void)(*name = *list), st.mode);
	return (0);
}

/* One object of each file type, as the kernel describes it (Linux st_mode). */
static inline void	ss_objects(ss_objs *o)
{
	static const char *const	blk[] = {"/dev/loop0", "/dev/sda", "/dev/vda", "/dev/nvme0n1",
		"/dev/xvda", "/dev/mmcblk0", "/dev/ram0", 0};
	static const char *const	sock[] = {"/run/systemd/notify", "/run/systemd/journal/socket",
		"/dev/log", "/run/dbus/system_bus_socket", "/var/run/dbus/system_bus_socket",
		"/tmp/.X11-unix/X0", 0};
	static const char *const	chr[] = {"/dev/null", 0};
	static const char *const	dir[] = {"/", 0};
	char						p[128];
	char						l[128];
	int							fds[2];
	ss_st						st;
	int							fd;

	lc_fill(o, 0, sizeof(*o));
	fd = ss_makeFile(ss_path(p, sizeof(p), "sso"), 1);
	if (fd >= 0 && ss_raw(fd, 0, 0, &st) == 0)
		o->mode[SS_REG] = st.mode, o->name[SS_REG] = "a regular file";
	if (lc_sysSymlink(p, ss_path(l, sizeof(l), "ssl")) == 0 && ss_raw(-1, l, 1, &st) == 0)
		o->mode[SS_LNK] = st.mode, o->name[SS_LNK] = "a symbolic link";
	(void)lc_sysUnlink(l);
	(void)lc_sysUnlink(p);
	if (fd >= 0)
		(void)lc_sysClose(fd);
	if (lc_sysPipe(fds, LC_O_CLOEXEC) == 0)
	{
		if (ss_raw(fds[0], 0, 0, &st) == 0)
			o->mode[SS_FIFO] = st.mode, o->name[SS_FIFO] = "a pipe";
		(void)lc_sysClose(fds[0]);
		(void)lc_sysClose(fds[1]);
	}
	o->mode[SS_DIR] = ss_find(dir, 0040000, &o->name[SS_DIR]);
	o->mode[SS_CHR] = ss_find(chr, 0020000, &o->name[SS_CHR]);
	o->mode[SS_BLK] = ss_find(blk, 0060000, &o->name[SS_BLK]);
	o->mode[SS_SOCK] = ss_find(sock, 0140000, &o->name[SS_SOCK]);
}

/*
 * The cases of a file type test macro, for the object kind it tests:
 * nonzero on that object (SKIP when the system has none), 0 on every
 * other object, and the same answer whatever the permission bits.
 */
# define SS_TYPE_MACRO_CASES(MACRO, KIND, WHAT)									\
	TEST_OPT(nonzero_for_##WHAT, .ref = "XBD <sys/stat.h>")					\
	{																			\
		ss_objs	o;																\
																				\
		ss_objects(&o);															\
		if (o.mode[KIND] == 0)													\
			SKIP("no such file on this system");								\
		LOG(o.name[KIND]);														\
		EXPECT_NE(MACRO((mode_t)o.mode[KIND]), 0);								\
	}																			\
																				\
	TEST_OPT(zero_for_the_other_file_types, .ref = "XBD <sys/stat.h>")			\
	{																			\
		ss_objs	o;																\
		int		bad = 0;														\
																				\
		ss_objects(&o);															\
		for (int k = 0; k < SS_KINDS; k++)										\
			if (k != (KIND) && o.mode[k] != 0 && MACRO((mode_t)o.mode[k]) != 0)	\
			{																	\
				LOG(o.name[k]);													\
				bad++;															\
			}																	\
		EXPECT_EQ(bad, 0);														\
	}																			\
																				\
	TEST_OPT(permission_bits_do_not_matter, .ref = "XBD <sys/stat.h>")			\
	{																			\
		ss_objs	o;																\
		int		bad = 0;														\
																				\
		ss_objects(&o);															\
		for (int k = 0; k < SS_KINDS; k++)										\
		{																		\
			mode_t	m = (mode_t)o.mode[k];										\
			mode_t	all = (mode_t)(m | 07777);									\
			mode_t	none = (mode_t)(m & ~(mode_t)07777);						\
																				\
			if (o.mode[k] != 0)													\
				bad += (MACRO(m) != 0) != (MACRO(all) != 0)						\
					|| (MACRO(m) != 0) != (MACRO(none) != 0);					\
		}																		\
		EXPECT_EQ(bad, 0);														\
	}

/*
 * The cases of an S_TYPEIS* macro: no file of ss_objects is a message
 * queue, a semaphore, a shared or typed memory object, so the macro gives
 * 0 for each of them, whether or not the libc implements that type.
 */
# define SS_TYPEIS_CASES(MACRO, OPTS)											\
	TEST_OPT(takes_a_pointer_to_struct_stat OPTS, .ref = "XBD <sys/stat.h>")	\
	{																			\
		struct stat	s;															\
		const struct stat	*p = &s;											\
																				\
		lc_fill(&s, 0, sizeof(s));												\
		EXPECT_EQ(MACRO(p) != 0, MACRO(&s) != 0);								\
	}																			\
																				\
	TEST_OPT(zero_for_files_of_the_other_types OPTS, .ref = "XBD <sys/stat.h>")	\
	{																			\
		ss_objs		o;															\
		struct stat	s;															\
		int			bad = 0;													\
																				\
		ss_objects(&o);															\
		for (int k = 0; k < SS_KINDS; k++)										\
			if (o.mode[k] != 0)													\
			{																	\
				lc_fill(&s, 0, sizeof(s));										\
				s.st_mode = (mode_t)o.mode[k];									\
				if (MACRO(&s) != 0)												\
				{																\
					LOG(o.name[k]);												\
					bad++;														\
				}																\
			}																	\
		EXPECT_EQ(bad, 0);														\
	}

#endif
