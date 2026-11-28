/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file options.h
 * @brief Cases of the constants for options and option groups of <unistd.h>.
 * @Created: 2026/11/28 09:35:42 by Moutig
 * @Updated: 2026/11/28 09:37:07 by Moutig
 *
 * Each of these constants, when defined, has the value -1, 0 or greater
 * and is usable in #if (XBD <unistd.h>, "Constants for Options and Option
 * Groups"; XBD 2.1.6). The ones an edition makes mandatory "shall always
 * be set to the value" of the edition (at least the one under test, from
 * the edition that says it); _POSIX_JOB_CONTROL, _POSIX_REGEXP,
 * _POSIX_SAVED_IDS, _POSIX_SHELL are always greater than zero;
 * _POSIX_CHOWN_RESTRICTED, _POSIX_NO_TRUNC, _XOPEN_ENH_I18N, _XOPEN_SHM
 * never -1, nor _XOPEN_UNIX on an XSI-conforming system (XBD 2.1.4). A constant of an option is required only with that option
 * (spec/unistd.spec, OPTS=).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_OPTIONS_H
# define BITS_UNISTD_OPTIONS_H

# include "framework/lc.h"

/* The value of the edition under test (_POSIX_C_SOURCE of the target). */
#  ifndef BITS_UNISTD_EDITION
#   if defined(_POSIX_C_SOURCE)
#    define BITS_UNISTD_EDITION	_POSIX_C_SOURCE
#   else
#    define BITS_UNISTD_EDITION	200112L
#   endif
#  endif

# if defined(LC_REQ__POSIX2_C_BIND) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX2_C_BIND__value, "_POSIX2_C_BIND", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX2_C_BIND == -1 || _POSIX2_C_BIND >= 0
	EXPECT_TRUE(_POSIX2_C_BIND == -1 || _POSIX2_C_BIND >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2001 on). */
TEST_SYM(_POSIX2_C_BIND__always, "_POSIX2_C_BIND", always_the_value_of_the_edition, .since = "POSIX.1-2001",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX2_C_BIND >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_ADVISORY_INFO) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_ADVISORY_INFO__value, "_POSIX_ADVISORY_INFO", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_ADVISORY_INFO == -1 || _POSIX_ADVISORY_INFO >= 0
	EXPECT_TRUE(_POSIX_ADVISORY_INFO == -1 || _POSIX_ADVISORY_INFO >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_ASYNCHRONOUS_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_ASYNCHRONOUS_IO__value, "_POSIX_ASYNCHRONOUS_IO", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_ASYNCHRONOUS_IO == -1 || _POSIX_ASYNCHRONOUS_IO >= 0
	EXPECT_TRUE(_POSIX_ASYNCHRONOUS_IO == -1 || _POSIX_ASYNCHRONOUS_IO >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_ASYNCHRONOUS_IO__always, "_POSIX_ASYNCHRONOUS_IO", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_ASYNCHRONOUS_IO >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_BARRIERS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_BARRIERS__value, "_POSIX_BARRIERS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_BARRIERS == -1 || _POSIX_BARRIERS >= 0
	EXPECT_TRUE(_POSIX_BARRIERS == -1 || _POSIX_BARRIERS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_BARRIERS__always, "_POSIX_BARRIERS", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_BARRIERS >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_CHOWN_RESTRICTED) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_CHOWN_RESTRICTED__value, "_POSIX_CHOWN_RESTRICTED", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_CHOWN_RESTRICTED == -1 || _POSIX_CHOWN_RESTRICTED >= 0
	EXPECT_TRUE(_POSIX_CHOWN_RESTRICTED == -1 || _POSIX_CHOWN_RESTRICTED >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(_POSIX_CHOWN_RESTRICTED__not_minus_1, "_POSIX_CHOWN_RESTRICTED", never_minus_1, .ref = "XBD <unistd.h>")
{
	EXPECT_NE(_POSIX_CHOWN_RESTRICTED, -1);
}
# endif

# if defined(LC_REQ__POSIX_CLOCK_SELECTION) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_CLOCK_SELECTION__value, "_POSIX_CLOCK_SELECTION", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_CLOCK_SELECTION == -1 || _POSIX_CLOCK_SELECTION >= 0
	EXPECT_TRUE(_POSIX_CLOCK_SELECTION == -1 || _POSIX_CLOCK_SELECTION >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_CLOCK_SELECTION__always, "_POSIX_CLOCK_SELECTION", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_CLOCK_SELECTION >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_CPUTIME) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_CPUTIME__value, "_POSIX_CPUTIME", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_CPUTIME == -1 || _POSIX_CPUTIME >= 0
	EXPECT_TRUE(_POSIX_CPUTIME == -1 || _POSIX_CPUTIME >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_DEVICE_CONTROL) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_DEVICE_CONTROL__value, "_POSIX_DEVICE_CONTROL", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_DEVICE_CONTROL == -1 || _POSIX_DEVICE_CONTROL >= 0
	EXPECT_TRUE(_POSIX_DEVICE_CONTROL == -1 || _POSIX_DEVICE_CONTROL >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_FSYNC) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_FSYNC__value, "_POSIX_FSYNC", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_FSYNC == -1 || _POSIX_FSYNC >= 0
	EXPECT_TRUE(_POSIX_FSYNC == -1 || _POSIX_FSYNC >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_IPV6) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_IPV6__value, "_POSIX_IPV6", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_IPV6 == -1 || _POSIX_IPV6 >= 0
	EXPECT_TRUE(_POSIX_IPV6 == -1 || _POSIX_IPV6 >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_JOB_CONTROL) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_JOB_CONTROL__value, "_POSIX_JOB_CONTROL", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_JOB_CONTROL == -1 || _POSIX_JOB_CONTROL >= 0
	EXPECT_TRUE(_POSIX_JOB_CONTROL == -1 || _POSIX_JOB_CONTROL >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(_POSIX_JOB_CONTROL__positive, "_POSIX_JOB_CONTROL", always_greater_than_zero, .ref = "XBD <unistd.h>")
{
	EXPECT_GT(_POSIX_JOB_CONTROL, 0);
}
# endif

# if defined(LC_REQ__POSIX_MAPPED_FILES) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_MAPPED_FILES__value, "_POSIX_MAPPED_FILES", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_MAPPED_FILES == -1 || _POSIX_MAPPED_FILES >= 0
	EXPECT_TRUE(_POSIX_MAPPED_FILES == -1 || _POSIX_MAPPED_FILES >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_MAPPED_FILES__always, "_POSIX_MAPPED_FILES", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_MAPPED_FILES >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_MEMLOCK) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_MEMLOCK__value, "_POSIX_MEMLOCK", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_MEMLOCK == -1 || _POSIX_MEMLOCK >= 0
	EXPECT_TRUE(_POSIX_MEMLOCK == -1 || _POSIX_MEMLOCK >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_MEMLOCK_RANGE) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_MEMLOCK_RANGE__value, "_POSIX_MEMLOCK_RANGE", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_MEMLOCK_RANGE == -1 || _POSIX_MEMLOCK_RANGE >= 0
	EXPECT_TRUE(_POSIX_MEMLOCK_RANGE == -1 || _POSIX_MEMLOCK_RANGE >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_MEMORY_PROTECTION) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_MEMORY_PROTECTION__value, "_POSIX_MEMORY_PROTECTION", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_MEMORY_PROTECTION == -1 || _POSIX_MEMORY_PROTECTION >= 0
	EXPECT_TRUE(_POSIX_MEMORY_PROTECTION == -1 || _POSIX_MEMORY_PROTECTION >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_MEMORY_PROTECTION__always, "_POSIX_MEMORY_PROTECTION", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_MEMORY_PROTECTION >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_MESSAGE_PASSING) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_MESSAGE_PASSING__value, "_POSIX_MESSAGE_PASSING", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_MESSAGE_PASSING == -1 || _POSIX_MESSAGE_PASSING >= 0
	EXPECT_TRUE(_POSIX_MESSAGE_PASSING == -1 || _POSIX_MESSAGE_PASSING >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_MONOTONIC_CLOCK) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_MONOTONIC_CLOCK__value, "_POSIX_MONOTONIC_CLOCK", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_MONOTONIC_CLOCK == -1 || _POSIX_MONOTONIC_CLOCK >= 0
	EXPECT_TRUE(_POSIX_MONOTONIC_CLOCK == -1 || _POSIX_MONOTONIC_CLOCK >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2024 on). */
TEST_SYM(_POSIX_MONOTONIC_CLOCK__always, "_POSIX_MONOTONIC_CLOCK", always_the_value_of_the_edition, .since = "POSIX.1-2024",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_MONOTONIC_CLOCK >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_NO_TRUNC) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_NO_TRUNC__value, "_POSIX_NO_TRUNC", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_NO_TRUNC == -1 || _POSIX_NO_TRUNC >= 0
	EXPECT_TRUE(_POSIX_NO_TRUNC == -1 || _POSIX_NO_TRUNC >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(_POSIX_NO_TRUNC__not_minus_1, "_POSIX_NO_TRUNC", never_minus_1, .ref = "XBD <unistd.h>")
{
	EXPECT_NE(_POSIX_NO_TRUNC, -1);
}
# endif

# if defined(LC_REQ__POSIX_PRIORITIZED_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_PRIORITIZED_IO__value, "_POSIX_PRIORITIZED_IO", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_PRIORITIZED_IO == -1 || _POSIX_PRIORITIZED_IO >= 0
	EXPECT_TRUE(_POSIX_PRIORITIZED_IO == -1 || _POSIX_PRIORITIZED_IO >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_PRIORITY_SCHEDULING) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_PRIORITY_SCHEDULING__value, "_POSIX_PRIORITY_SCHEDULING", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_PRIORITY_SCHEDULING == -1 || _POSIX_PRIORITY_SCHEDULING >= 0
	EXPECT_TRUE(_POSIX_PRIORITY_SCHEDULING == -1 || _POSIX_PRIORITY_SCHEDULING >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_RAW_SOCKETS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_RAW_SOCKETS__value, "_POSIX_RAW_SOCKETS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_RAW_SOCKETS == -1 || _POSIX_RAW_SOCKETS >= 0
	EXPECT_TRUE(_POSIX_RAW_SOCKETS == -1 || _POSIX_RAW_SOCKETS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_READER_WRITER_LOCKS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_READER_WRITER_LOCKS__value, "_POSIX_READER_WRITER_LOCKS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_READER_WRITER_LOCKS == -1 || _POSIX_READER_WRITER_LOCKS >= 0
	EXPECT_TRUE(_POSIX_READER_WRITER_LOCKS == -1 || _POSIX_READER_WRITER_LOCKS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_READER_WRITER_LOCKS__always, "_POSIX_READER_WRITER_LOCKS", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_READER_WRITER_LOCKS >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_REALTIME_SIGNALS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_REALTIME_SIGNALS__value, "_POSIX_REALTIME_SIGNALS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_REALTIME_SIGNALS == -1 || _POSIX_REALTIME_SIGNALS >= 0
	EXPECT_TRUE(_POSIX_REALTIME_SIGNALS == -1 || _POSIX_REALTIME_SIGNALS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_REALTIME_SIGNALS__always, "_POSIX_REALTIME_SIGNALS", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_REALTIME_SIGNALS >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_REGEXP) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_REGEXP__value, "_POSIX_REGEXP", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_REGEXP == -1 || _POSIX_REGEXP >= 0
	EXPECT_TRUE(_POSIX_REGEXP == -1 || _POSIX_REGEXP >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(_POSIX_REGEXP__positive, "_POSIX_REGEXP", always_greater_than_zero, .ref = "XBD <unistd.h>")
{
	EXPECT_GT(_POSIX_REGEXP, 0);
}
# endif

# if defined(LC_REQ__POSIX_SAVED_IDS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_SAVED_IDS__value, "_POSIX_SAVED_IDS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_SAVED_IDS == -1 || _POSIX_SAVED_IDS >= 0
	EXPECT_TRUE(_POSIX_SAVED_IDS == -1 || _POSIX_SAVED_IDS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(_POSIX_SAVED_IDS__positive, "_POSIX_SAVED_IDS", always_greater_than_zero, .ref = "XBD <unistd.h>")
{
	EXPECT_GT(_POSIX_SAVED_IDS, 0);
}
# endif

# if defined(LC_REQ__POSIX_SEMAPHORES) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_SEMAPHORES__value, "_POSIX_SEMAPHORES", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_SEMAPHORES == -1 || _POSIX_SEMAPHORES >= 0
	EXPECT_TRUE(_POSIX_SEMAPHORES == -1 || _POSIX_SEMAPHORES >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_SEMAPHORES__always, "_POSIX_SEMAPHORES", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_SEMAPHORES >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_SHARED_MEMORY_OBJECTS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_SHARED_MEMORY_OBJECTS__value, "_POSIX_SHARED_MEMORY_OBJECTS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_SHARED_MEMORY_OBJECTS == -1 || _POSIX_SHARED_MEMORY_OBJECTS >= 0
	EXPECT_TRUE(_POSIX_SHARED_MEMORY_OBJECTS == -1 || _POSIX_SHARED_MEMORY_OBJECTS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_SHELL) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_SHELL__value, "_POSIX_SHELL", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_SHELL == -1 || _POSIX_SHELL >= 0
	EXPECT_TRUE(_POSIX_SHELL == -1 || _POSIX_SHELL >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(_POSIX_SHELL__positive, "_POSIX_SHELL", always_greater_than_zero, .ref = "XBD <unistd.h>")
{
	EXPECT_GT(_POSIX_SHELL, 0);
}
# endif

# if defined(LC_REQ__POSIX_SPAWN) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_SPAWN__value, "_POSIX_SPAWN", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_SPAWN == -1 || _POSIX_SPAWN >= 0
	EXPECT_TRUE(_POSIX_SPAWN == -1 || _POSIX_SPAWN >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_SPIN_LOCKS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_SPIN_LOCKS__value, "_POSIX_SPIN_LOCKS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_SPIN_LOCKS == -1 || _POSIX_SPIN_LOCKS >= 0
	EXPECT_TRUE(_POSIX_SPIN_LOCKS == -1 || _POSIX_SPIN_LOCKS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_SPIN_LOCKS__always, "_POSIX_SPIN_LOCKS", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_SPIN_LOCKS >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_SPORADIC_SERVER) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_SPORADIC_SERVER__value, "_POSIX_SPORADIC_SERVER", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_SPORADIC_SERVER == -1 || _POSIX_SPORADIC_SERVER >= 0
	EXPECT_TRUE(_POSIX_SPORADIC_SERVER == -1 || _POSIX_SPORADIC_SERVER >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_SYNCHRONIZED_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_SYNCHRONIZED_IO__value, "_POSIX_SYNCHRONIZED_IO", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_SYNCHRONIZED_IO == -1 || _POSIX_SYNCHRONIZED_IO >= 0
	EXPECT_TRUE(_POSIX_SYNCHRONIZED_IO == -1 || _POSIX_SYNCHRONIZED_IO >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREADS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREADS__value, "_POSIX_THREADS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREADS == -1 || _POSIX_THREADS >= 0
	EXPECT_TRUE(_POSIX_THREADS == -1 || _POSIX_THREADS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_THREADS__always, "_POSIX_THREADS", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_THREADS >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_ATTR_STACKADDR) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_ATTR_STACKADDR__value, "_POSIX_THREAD_ATTR_STACKADDR", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_ATTR_STACKADDR == -1 || _POSIX_THREAD_ATTR_STACKADDR >= 0
	EXPECT_TRUE(_POSIX_THREAD_ATTR_STACKADDR == -1 || _POSIX_THREAD_ATTR_STACKADDR >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_ATTR_STACKSIZE) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_ATTR_STACKSIZE__value, "_POSIX_THREAD_ATTR_STACKSIZE", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_ATTR_STACKSIZE == -1 || _POSIX_THREAD_ATTR_STACKSIZE >= 0
	EXPECT_TRUE(_POSIX_THREAD_ATTR_STACKSIZE == -1 || _POSIX_THREAD_ATTR_STACKSIZE >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_CPUTIME) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_CPUTIME__value, "_POSIX_THREAD_CPUTIME", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_CPUTIME == -1 || _POSIX_THREAD_CPUTIME >= 0
	EXPECT_TRUE(_POSIX_THREAD_CPUTIME == -1 || _POSIX_THREAD_CPUTIME >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_PRIORITY_SCHEDULING) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_PRIORITY_SCHEDULING__value, "_POSIX_THREAD_PRIORITY_SCHEDULING", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_PRIORITY_SCHEDULING == -1 || _POSIX_THREAD_PRIORITY_SCHEDULING >= 0
	EXPECT_TRUE(_POSIX_THREAD_PRIORITY_SCHEDULING == -1 || _POSIX_THREAD_PRIORITY_SCHEDULING >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_PRIO_INHERIT) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_PRIO_INHERIT__value, "_POSIX_THREAD_PRIO_INHERIT", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_PRIO_INHERIT == -1 || _POSIX_THREAD_PRIO_INHERIT >= 0
	EXPECT_TRUE(_POSIX_THREAD_PRIO_INHERIT == -1 || _POSIX_THREAD_PRIO_INHERIT >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_PRIO_PROTECT) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_PRIO_PROTECT__value, "_POSIX_THREAD_PRIO_PROTECT", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_PRIO_PROTECT == -1 || _POSIX_THREAD_PRIO_PROTECT >= 0
	EXPECT_TRUE(_POSIX_THREAD_PRIO_PROTECT == -1 || _POSIX_THREAD_PRIO_PROTECT >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_PROCESS_SHARED) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_PROCESS_SHARED__value, "_POSIX_THREAD_PROCESS_SHARED", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_PROCESS_SHARED == -1 || _POSIX_THREAD_PROCESS_SHARED >= 0
	EXPECT_TRUE(_POSIX_THREAD_PROCESS_SHARED == -1 || _POSIX_THREAD_PROCESS_SHARED >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_ROBUST_PRIO_INHERIT) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_ROBUST_PRIO_INHERIT__value, "_POSIX_THREAD_ROBUST_PRIO_INHERIT", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_ROBUST_PRIO_INHERIT == -1 || _POSIX_THREAD_ROBUST_PRIO_INHERIT >= 0
	EXPECT_TRUE(_POSIX_THREAD_ROBUST_PRIO_INHERIT == -1 || _POSIX_THREAD_ROBUST_PRIO_INHERIT >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_ROBUST_PRIO_PROTECT) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_ROBUST_PRIO_PROTECT__value, "_POSIX_THREAD_ROBUST_PRIO_PROTECT", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_ROBUST_PRIO_PROTECT == -1 || _POSIX_THREAD_ROBUST_PRIO_PROTECT >= 0
	EXPECT_TRUE(_POSIX_THREAD_ROBUST_PRIO_PROTECT == -1 || _POSIX_THREAD_ROBUST_PRIO_PROTECT >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_SAFE_FUNCTIONS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_SAFE_FUNCTIONS__value, "_POSIX_THREAD_SAFE_FUNCTIONS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_SAFE_FUNCTIONS == -1 || _POSIX_THREAD_SAFE_FUNCTIONS >= 0
	EXPECT_TRUE(_POSIX_THREAD_SAFE_FUNCTIONS == -1 || _POSIX_THREAD_SAFE_FUNCTIONS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_THREAD_SAFE_FUNCTIONS__always, "_POSIX_THREAD_SAFE_FUNCTIONS", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_SAFE_FUNCTIONS >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_THREAD_SPORADIC_SERVER) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_THREAD_SPORADIC_SERVER__value, "_POSIX_THREAD_SPORADIC_SERVER", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_THREAD_SPORADIC_SERVER == -1 || _POSIX_THREAD_SPORADIC_SERVER >= 0
	EXPECT_TRUE(_POSIX_THREAD_SPORADIC_SERVER == -1 || _POSIX_THREAD_SPORADIC_SERVER >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_TIMEOUTS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_TIMEOUTS__value, "_POSIX_TIMEOUTS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_TIMEOUTS == -1 || _POSIX_TIMEOUTS >= 0
	EXPECT_TRUE(_POSIX_TIMEOUTS == -1 || _POSIX_TIMEOUTS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_TIMEOUTS__always, "_POSIX_TIMEOUTS", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_TIMEOUTS >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_TIMERS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_TIMERS__value, "_POSIX_TIMERS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_TIMERS == -1 || _POSIX_TIMERS >= 0
	EXPECT_TRUE(_POSIX_TIMERS == -1 || _POSIX_TIMERS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* "This symbol shall always be set to the value" of the edition (POSIX.1-2008 on). */
TEST_SYM(_POSIX_TIMERS__always, "_POSIX_TIMERS", always_the_value_of_the_edition, .since = "POSIX.1-2008",
	.ref = "XBD <unistd.h>")
{
#  if _POSIX_TIMERS >= BITS_UNISTD_EDITION
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_TRACE) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_TRACE__value, "_POSIX_TRACE", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_TRACE == -1 || _POSIX_TRACE >= 0
	EXPECT_TRUE(_POSIX_TRACE == -1 || _POSIX_TRACE >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_TRACE_EVENT_FILTER) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_TRACE_EVENT_FILTER__value, "_POSIX_TRACE_EVENT_FILTER", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_TRACE_EVENT_FILTER == -1 || _POSIX_TRACE_EVENT_FILTER >= 0
	EXPECT_TRUE(_POSIX_TRACE_EVENT_FILTER == -1 || _POSIX_TRACE_EVENT_FILTER >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_TRACE_INHERIT) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_TRACE_INHERIT__value, "_POSIX_TRACE_INHERIT", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_TRACE_INHERIT == -1 || _POSIX_TRACE_INHERIT >= 0
	EXPECT_TRUE(_POSIX_TRACE_INHERIT == -1 || _POSIX_TRACE_INHERIT >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_TRACE_LOG) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_TRACE_LOG__value, "_POSIX_TRACE_LOG", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_TRACE_LOG == -1 || _POSIX_TRACE_LOG >= 0
	EXPECT_TRUE(_POSIX_TRACE_LOG == -1 || _POSIX_TRACE_LOG >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__POSIX_TYPED_MEMORY_OBJECTS) || defined(LC_DEFS_ALL)
TEST_SYM(_POSIX_TYPED_MEMORY_OBJECTS__value, "_POSIX_TYPED_MEMORY_OBJECTS", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _POSIX_TYPED_MEMORY_OBJECTS == -1 || _POSIX_TYPED_MEMORY_OBJECTS >= 0
	EXPECT_TRUE(_POSIX_TYPED_MEMORY_OBJECTS == -1 || _POSIX_TYPED_MEMORY_OBJECTS >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__XOPEN_CRYPT) || defined(LC_DEFS_ALL)
TEST_SYM(_XOPEN_CRYPT__value, "_XOPEN_CRYPT", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _XOPEN_CRYPT == -1 || _XOPEN_CRYPT >= 0
	EXPECT_TRUE(_XOPEN_CRYPT == -1 || _XOPEN_CRYPT >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}
# endif

# if defined(LC_REQ__XOPEN_ENH_I18N) || defined(LC_DEFS_ALL)
TEST_SYM(_XOPEN_ENH_I18N__value, "_XOPEN_ENH_I18N", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _XOPEN_ENH_I18N == -1 || _XOPEN_ENH_I18N >= 0
	EXPECT_TRUE(_XOPEN_ENH_I18N == -1 || _XOPEN_ENH_I18N >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(_XOPEN_ENH_I18N__not_minus_1, "_XOPEN_ENH_I18N", never_minus_1, .ref = "XBD <unistd.h>")
{
	EXPECT_NE(_XOPEN_ENH_I18N, -1);
}
# endif

# if defined(LC_REQ__XOPEN_SHM) || defined(LC_DEFS_ALL)
TEST_SYM(_XOPEN_SHM__value, "_XOPEN_SHM", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _XOPEN_SHM == -1 || _XOPEN_SHM >= 0
	EXPECT_TRUE(_XOPEN_SHM == -1 || _XOPEN_SHM >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(_XOPEN_SHM__not_minus_1, "_XOPEN_SHM", never_minus_1, .ref = "XBD <unistd.h>")
{
	EXPECT_NE(_XOPEN_SHM, -1);
}
# endif

# if defined(LC_REQ__XOPEN_UNIX) || defined(LC_DEFS_ALL)
TEST_SYM(_XOPEN_UNIX__value, "_XOPEN_UNIX", usable_in_if_with_a_value_of_minus_1_0_or_more, .ref = "XBD <unistd.h>")
{
#  if _XOPEN_UNIX == -1 || _XOPEN_UNIX >= 0
	EXPECT_TRUE(_XOPEN_UNIX == -1 || _XOPEN_UNIX >= 0);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* An XSI-conforming implementation sets it to a value other than -1 (XBD 2.1.4). */
TEST_SYM(_XOPEN_UNIX__not_minus_1, "_XOPEN_UNIX", never_minus_1, .ref = "XBD 2.1.4")
{
	EXPECT_NE(_XOPEN_UNIX, -1);
}
# endif


#endif
