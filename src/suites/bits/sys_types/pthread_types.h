/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_types.h
 * @brief Cases of the pthread_*_t types, for every header that defines them.
 * @Created: 2026/11/28 17:36:06 by Moutig
 * @Updated: 2026/11/28 17:36:36 by Moutig
 *
 * XBD <sys/types.h>: the pthread_*_t types are the exceptions to "all of
 * the types shall be defined as arithmetic types"; they are object types,
 * and there are no defined comparison or assignment operators for them,
 * except pthread_key_t, pthread_once_t and pthread_t, which are not in
 * that list. Nothing else can be checked without calling a function.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_PTHREAD_TYPES_H
# define BITS_SYS_TYPES_PTHREAD_TYPES_H

# include "framework/lc.h"

/*
 * Declared by the header, and a complete object type (sizeof, arrays).
 * Through a pointer: the type may be qualified (volatile int), which the
 * lvalue conversion of _Generic would drop.
 */
# define BITS_PTHREAD_TYPE(T)															\
	TEST_SYM(T##__declared, #T, declared_by_the_header, .ref = "XBD <sys/types.h>")	\
	{																				\
		static T	x;																\
																					\
		EXPECT_TYPE(&x, T *);														\
	}																				\
																					\
	TEST_SYM(T##__complete, #T, is_a_complete_object_type, .ref = "XBD <sys/types.h>")	\
	{																				\
		static T	a[2];															\
																					\
		EXPECT_UGT(sizeof(T), 0);													\
		EXPECT_UEQ(sizeof(a), 2 * sizeof(T));										\
	}

/*
 * Assignment is defined: the type is not in the list of XBD <sys/types.h>.
 * Compiling the assignment is the test (the bytes are not compared: a
 * structure may have padding).
 */
# define BITS_PTHREAD_ASSIGNABLE(T)													\
	TEST_SYM(T##__assignable, #T, can_be_assigned, .ref = "XBD <sys/types.h>")		\
	{																				\
		static T	a;																\
		T			b;																\
																					\
		b = a;																		\
		EXPECT_TYPE(&b, T *);														\
	}

# if defined(LC_REQ_pthread_attr_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_attr_t)
# endif

# if defined(LC_REQ_pthread_barrier_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_barrier_t)
# endif

# if defined(LC_REQ_pthread_barrierattr_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_barrierattr_t)
# endif

# if defined(LC_REQ_pthread_cond_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_cond_t)
# endif

# if defined(LC_REQ_pthread_condattr_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_condattr_t)
# endif

# if defined(LC_REQ_pthread_key_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_key_t)
BITS_PTHREAD_ASSIGNABLE(pthread_key_t)
# endif

# if defined(LC_REQ_pthread_mutex_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_mutex_t)
# endif

# if defined(LC_REQ_pthread_mutexattr_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_mutexattr_t)
# endif

# if defined(LC_REQ_pthread_once_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_once_t)
BITS_PTHREAD_ASSIGNABLE(pthread_once_t)
# endif

# if defined(LC_REQ_pthread_rwlock_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_rwlock_t)
# endif

# if defined(LC_REQ_pthread_rwlockattr_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_rwlockattr_t)
# endif

# if defined(LC_REQ_pthread_spinlock_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_spinlock_t)
# endif

# if defined(LC_REQ_pthread_t) || defined(LC_DEFS_ALL)
BITS_PTHREAD_TYPE(pthread_t)
BITS_PTHREAD_ASSIGNABLE(pthread_t)
# endif

#endif
