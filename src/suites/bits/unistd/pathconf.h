/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pathconf.h
 * @brief Cases of the pathconf() names of <unistd.h> (_PC_*).
 * @Created: 2026/11/28 09:35:51 by Moutig
 * @Updated: 2026/11/28 09:37:18 by Moutig
 *
 * The _PC_* symbolic constants name what pathconf() reports (XBD
 * <unistd.h>, "Constants for Functions"): each one must name something
 * of its own, so its value differs from every other _PC_* name the
 * header defines. What pathconf() answers is tested with pathconf().
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_PATHCONF_H
# define BITS_UNISTD_PATHCONF_H

# include "framework/lc.h"

/* The _PC_* names the header defines, to compare them. */
static const long	bitsPathconfNames[] = {
#  ifdef _PC_2_SYMLINKS
	(long)_PC_2_SYMLINKS,
#  endif
#  ifdef _PC_ALLOC_SIZE_MIN
	(long)_PC_ALLOC_SIZE_MIN,
#  endif
#  ifdef _PC_ASYNC_IO
	(long)_PC_ASYNC_IO,
#  endif
#  ifdef _PC_CHOWN_RESTRICTED
	(long)_PC_CHOWN_RESTRICTED,
#  endif
#  ifdef _PC_FALLOC
	(long)_PC_FALLOC,
#  endif
#  ifdef _PC_FILESIZEBITS
	(long)_PC_FILESIZEBITS,
#  endif
#  ifdef _PC_LINK_MAX
	(long)_PC_LINK_MAX,
#  endif
#  ifdef _PC_MAX_CANON
	(long)_PC_MAX_CANON,
#  endif
#  ifdef _PC_MAX_INPUT
	(long)_PC_MAX_INPUT,
#  endif
#  ifdef _PC_NAME_MAX
	(long)_PC_NAME_MAX,
#  endif
#  ifdef _PC_NO_TRUNC
	(long)_PC_NO_TRUNC,
#  endif
#  ifdef _PC_PATH_MAX
	(long)_PC_PATH_MAX,
#  endif
#  ifdef _PC_PIPE_BUF
	(long)_PC_PIPE_BUF,
#  endif
#  ifdef _PC_PRIO_IO
	(long)_PC_PRIO_IO,
#  endif
#  ifdef _PC_REC_INCR_XFER_SIZE
	(long)_PC_REC_INCR_XFER_SIZE,
#  endif
#  ifdef _PC_REC_MAX_XFER_SIZE
	(long)_PC_REC_MAX_XFER_SIZE,
#  endif
#  ifdef _PC_REC_MIN_XFER_SIZE
	(long)_PC_REC_MIN_XFER_SIZE,
#  endif
#  ifdef _PC_REC_XFER_ALIGN
	(long)_PC_REC_XFER_ALIGN,
#  endif
#  ifdef _PC_SYMLINK_MAX
	(long)_PC_SYMLINK_MAX,
#  endif
#  ifdef _PC_SYNC_IO
	(long)_PC_SYNC_IO,
#  endif
#  ifdef _PC_TEXTDOMAIN_MAX
	(long)_PC_TEXTDOMAIN_MAX,
#  endif
#  ifdef _PC_TIMESTAMP_RESOLUTION
	(long)_PC_TIMESTAMP_RESOLUTION,
#  endif
#  ifdef _PC_VDISABLE
	(long)_PC_VDISABLE,
#  endif
	-12345678L
};

/* Does v appear exactly once among the _PC_* names? */
static inline int	bitsPathconfUnique(long v)
{
	int	k = 0;

	for (int i = 0; i < (int)(sizeof(bitsPathconfNames) / sizeof(bitsPathconfNames[0])); i++)
		k += bitsPathconfNames[i] == v;
	return (k == 1);
}

# if defined(LC_REQ__PC_2_SYMLINKS) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_2_SYMLINKS__unique, "_PC_2_SYMLINKS", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_2_SYMLINKS));
}
# endif

# if defined(LC_REQ__PC_ALLOC_SIZE_MIN) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_ALLOC_SIZE_MIN__unique, "_PC_ALLOC_SIZE_MIN", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_ALLOC_SIZE_MIN));
}
# endif

# if defined(LC_REQ__PC_ASYNC_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_ASYNC_IO__unique, "_PC_ASYNC_IO", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_ASYNC_IO));
}
# endif

# if defined(LC_REQ__PC_CHOWN_RESTRICTED) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_CHOWN_RESTRICTED__unique, "_PC_CHOWN_RESTRICTED", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_CHOWN_RESTRICTED));
}
# endif

# if defined(LC_REQ__PC_FALLOC) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_FALLOC__unique, "_PC_FALLOC", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_FALLOC));
}
# endif

# if defined(LC_REQ__PC_FILESIZEBITS) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_FILESIZEBITS__unique, "_PC_FILESIZEBITS", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_FILESIZEBITS));
}
# endif

# if defined(LC_REQ__PC_LINK_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_LINK_MAX__unique, "_PC_LINK_MAX", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_LINK_MAX));
}
# endif

# if defined(LC_REQ__PC_MAX_CANON) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_MAX_CANON__unique, "_PC_MAX_CANON", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_MAX_CANON));
}
# endif

# if defined(LC_REQ__PC_MAX_INPUT) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_MAX_INPUT__unique, "_PC_MAX_INPUT", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_MAX_INPUT));
}
# endif

# if defined(LC_REQ__PC_NAME_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_NAME_MAX__unique, "_PC_NAME_MAX", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_NAME_MAX));
}
# endif

# if defined(LC_REQ__PC_NO_TRUNC) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_NO_TRUNC__unique, "_PC_NO_TRUNC", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_NO_TRUNC));
}
# endif

# if defined(LC_REQ__PC_PATH_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_PATH_MAX__unique, "_PC_PATH_MAX", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_PATH_MAX));
}
# endif

# if defined(LC_REQ__PC_PIPE_BUF) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_PIPE_BUF__unique, "_PC_PIPE_BUF", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_PIPE_BUF));
}
# endif

# if defined(LC_REQ__PC_PRIO_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_PRIO_IO__unique, "_PC_PRIO_IO", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_PRIO_IO));
}
# endif

# if defined(LC_REQ__PC_REC_INCR_XFER_SIZE) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_REC_INCR_XFER_SIZE__unique, "_PC_REC_INCR_XFER_SIZE", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_REC_INCR_XFER_SIZE));
}
# endif

# if defined(LC_REQ__PC_REC_MAX_XFER_SIZE) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_REC_MAX_XFER_SIZE__unique, "_PC_REC_MAX_XFER_SIZE", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_REC_MAX_XFER_SIZE));
}
# endif

# if defined(LC_REQ__PC_REC_MIN_XFER_SIZE) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_REC_MIN_XFER_SIZE__unique, "_PC_REC_MIN_XFER_SIZE", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_REC_MIN_XFER_SIZE));
}
# endif

# if defined(LC_REQ__PC_REC_XFER_ALIGN) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_REC_XFER_ALIGN__unique, "_PC_REC_XFER_ALIGN", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_REC_XFER_ALIGN));
}
# endif

# if defined(LC_REQ__PC_SYMLINK_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_SYMLINK_MAX__unique, "_PC_SYMLINK_MAX", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_SYMLINK_MAX));
}
# endif

# if defined(LC_REQ__PC_SYNC_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_SYNC_IO__unique, "_PC_SYNC_IO", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_SYNC_IO));
}
# endif

# if defined(LC_REQ__PC_TEXTDOMAIN_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_TEXTDOMAIN_MAX__unique, "_PC_TEXTDOMAIN_MAX", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_TEXTDOMAIN_MAX));
}
# endif

# if defined(LC_REQ__PC_TIMESTAMP_RESOLUTION) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_TIMESTAMP_RESOLUTION__unique, "_PC_TIMESTAMP_RESOLUTION", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_TIMESTAMP_RESOLUTION));
}
# endif

# if defined(LC_REQ__PC_VDISABLE) || defined(LC_DEFS_ALL)
TEST_SYM(_PC_VDISABLE__unique, "_PC_VDISABLE", distinct_from_the_other_pc_names, .ref = "XBD <unistd.h> (pathconf())")
{
	EXPECT_TRUE(bitsPathconfUnique((long)_PC_VDISABLE));
}
# endif


#endif
