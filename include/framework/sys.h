/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file sys.h
 * @brief Raw Linux syscalls used by the framework (x86_64, aarch64).
 * @Created: 2026/11/23 04:18:40 by Moutig
 * @Updated: 2026/11/24 05:13:17 by Moutig
 *
 * The framework must not depend on the libc under test: these are the
 * only "system" functions it uses. They return what the kernel returns
 * (a negative -errno on failure, no errno variable) and are private to
 * the framework: tests use the lc_* helpers of lc.h instead.
 *
 * Adding an architecture = adding its syscall numbers and its
 * lc_syscall3 / lc_syscall6 asm in sys.c.
 */

#ifndef LC_SYS_H
# define LC_SYS_H

/**
 * @brief write(2).
 *
 * @param fd  File descriptor.
 * @param buf Bytes to write.
 * @param n   Number of bytes.
 * @return Bytes written, or -errno.
 */
long	lc_sysWrite(int fd, const void *buf, unsigned long n);

/**
 * @brief getpid(2).
 *
 * @return Process id of the caller.
 */
long	lc_sysGetpid(void);

/**
 * @brief fcntl(2), integer argument only.
 *
 * @param fd  File descriptor.
 * @param cmd Command (LC_F_GETFD, ...).
 * @param arg Argument of the command.
 * @return Result of the command, or -errno (-EBADF: @p fd is closed).
 */
long	lc_sysFcntl(int fd, int cmd, long arg);

/**
 * @brief exit_group(2): end every thread of the process.
 *
 * @param status Exit status.
 */
void	lc_sysExit(int status) __attribute__((noreturn));

/**
 * @brief open(2), through openat(AT_FDCWD, ...).
 *
 * @param path  Path of the file.
 * @param flags LC_O_* flags.
 * @param mode  Permissions when the file is created.
 * @return New file descriptor, or -errno.
 */
long	lc_sysOpen(const char *path, int flags, int mode);

/**
 * @brief mkdir(2), through mkdirat(AT_FDCWD, ...).
 *
 * @param path Path of the directory.
 * @param mode Permissions.
 * @return 0, or -errno (-EEXIST is not an error for the callers).
 */
long	lc_sysMkdir(const char *path, int mode);

/**
 * @brief close(2).
 *
 * @param fd File descriptor.
 * @return 0, or -errno.
 */
long	lc_sysClose(int fd);

/**
 * @brief Anonymous private read/write mapping, the framework's allocator.
 *
 * @param len Size in bytes.
 * @return Start of the mapping, or NULL on failure.
 */
void	*lc_sysMmap(unsigned long len);

/**
 * @brief mprotect(2).
 *
 * @param addr Start of the range (page aligned).
 * @param len  Size in bytes.
 * @param prot PROT_* bits (0: no access).
 * @return 0, or -errno.
 */
long	lc_sysMprotect(void *addr, unsigned long len, int prot);

/**
 * @brief unlink(2), through unlinkat(AT_FDCWD, ...).
 *
 * @param path Path of the file.
 * @return 0, or -errno.
 */
long	lc_sysUnlink(const char *path);

/* ----- Constants (Linux values, same on x86_64 and aarch64) ----- */

/** fcntl command: get the descriptor flags (used to probe fd 3). */
# define LC_F_GETFD		1

/** open flag: write only. */
# define LC_O_WRONLY	01

/** open flag: create the file if it does not exist. */
# define LC_O_CREAT		0100

/** open flag: truncate the file to length 0. */
# define LC_O_TRUNC		01000

#endif /* LC_SYS_H */
