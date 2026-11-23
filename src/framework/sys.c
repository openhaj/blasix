/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file sys.c
 * @brief Raw Linux syscalls for the framework.
 * @Created: 2026/11/23 04:42:37 by Moutig
 * @Updated: 2026/11/24 10:30:00 by Moutig
 *
 * Every system call of the framework goes through lc_syscall3 /
 * lc_syscall6, a few lines of inline asm per architecture, so that the
 * test side never calls the libc under test (not even its syscall()
 * wrapper or errno).
 *
 * Path syscalls use their *at() form with AT_FDCWD: aarch64 (like every
 * recent Linux port) has no open, mkdir or unlink, only openat,
 * mkdirat and unlinkat, and the *at() form works on x86_64 too.
 *
 * To add an architecture: its syscall numbers (LC_NR_*, from the
 * kernel's unistd.h for that arch) and the two lc_syscallN functions
 * (calling convention: number and argument registers, instruction,
 * clobbers).
 */

#include "framework/sys.h"

#if !defined(__linux__)
# error "blasix: only Linux is supported for now"
#endif

#if defined(__x86_64__)

/* ----- x86_64: syscall, number in rax, args in rdi rsi rdx r10 r8 r9 ----- */

# define LC_NR_WRITE		1
# define LC_NR_GETPID		39
# define LC_NR_FCNTL		72
# define LC_NR_EXIT_GROUP	231
# define LC_NR_OPENAT		257
# define LC_NR_MKDIRAT		258
# define LC_NR_CLOSE		3
# define LC_NR_MMAP			9
# define LC_NR_UNLINKAT		263

/**
 * @brief System call with up to 3 arguments.
 *
 * The syscall instruction clobbers rcx (return address) and r11
 * (rflags).
 *
 * @param nr Syscall number.
 * @param a  First argument.
 * @param b  Second argument.
 * @param c  Third argument.
 * @return What the kernel returns (-errno on failure).
 */
static long	lc_syscall3(long nr, long a, long b, long c)
{
	long	ret;

	__asm__ volatile ("syscall"
		: "=a" (ret)
		: "a" (nr), "D" (a), "S" (b), "d" (c)
		: "rcx", "r11", "memory");
	return (ret);
}

/**
 * @brief System call with up to 6 arguments.
 *
 * The 4th argument goes in r10 (not rcx as in the function call ABI),
 * the 5th and 6th in r8 and r9: they have no constraint letter, hence
 * the register variables.
 *
 * @param nr Syscall number.
 * @param a  First argument.
 * @param b  Second argument.
 * @param c  Third argument.
 * @param d  Fourth argument.
 * @param e  Fifth argument.
 * @param f  Sixth argument.
 * @return What the kernel returns (-errno on failure).
 */
static long	lc_syscall6(long nr, long a, long b, long c, long d, long e, long f)
{
	long			ret;
	register long	r10 __asm__("r10") = d;
	register long	r8 __asm__("r8") = e;
	register long	r9 __asm__("r9") = f;

	__asm__ volatile ("syscall"
		: "=a" (ret)
		: "a" (nr), "D" (a), "S" (b), "d" (c), "r" (r10), "r" (r8), "r" (r9)
		: "rcx", "r11", "memory");
	return (ret);
}

#elif defined(__aarch64__)

/* ----- aarch64: svc #0, number in x8, args in x0 ... x5, result in x0 ----- */

# define LC_NR_WRITE		64
# define LC_NR_GETPID		172
# define LC_NR_FCNTL		25
# define LC_NR_EXIT_GROUP	94
# define LC_NR_OPENAT		56
# define LC_NR_MKDIRAT		34
# define LC_NR_CLOSE		57
# define LC_NR_MMAP			222
# define LC_NR_UNLINKAT		35

/**
 * @brief System call with up to 3 arguments.
 *
 * @param nr Syscall number.
 * @param a  First argument.
 * @param b  Second argument.
 * @param c  Third argument.
 * @return What the kernel returns (-errno on failure).
 */
static long	lc_syscall3(long nr, long a, long b, long c)
{
	register long	x8 __asm__("x8") = nr;
	register long	x0 __asm__("x0") = a;
	register long	x1 __asm__("x1") = b;
	register long	x2 __asm__("x2") = c;

	__asm__ volatile ("svc #0"
		: "+r" (x0)
		: "r" (x8), "r" (x1), "r" (x2)
		: "memory", "cc");
	return (x0);
}

/**
 * @brief System call with up to 6 arguments.
 *
 * @param nr Syscall number.
 * @param a  First argument.
 * @param b  Second argument.
 * @param c  Third argument.
 * @param d  Fourth argument.
 * @param e  Fifth argument.
 * @param f  Sixth argument.
 * @return What the kernel returns (-errno on failure).
 */
static long	lc_syscall6(long nr, long a, long b, long c, long d, long e, long f)
{
	register long	x8 __asm__("x8") = nr;
	register long	x0 __asm__("x0") = a;
	register long	x1 __asm__("x1") = b;
	register long	x2 __asm__("x2") = c;
	register long	x3 __asm__("x3") = d;
	register long	x4 __asm__("x4") = e;
	register long	x5 __asm__("x5") = f;

	__asm__ volatile ("svc #0"
		: "+r" (x0)
		: "r" (x8), "r" (x1), "r" (x2), "r" (x3), "r" (x4), "r" (x5)
		: "memory", "cc");
	return (x0);
}

#else
# error "blasix: unsupported architecture (add its syscalls in sys.c)"
#endif

/* ----- Wrappers (documented in sys.h) ----- */

long	lc_sysWrite(int fd, const void *buf, unsigned long n)
{
	return (lc_syscall3(LC_NR_WRITE, fd, (long)buf, (long)n));
}

long	lc_sysGetpid(void)
{
	return (lc_syscall3(LC_NR_GETPID, 0, 0, 0));
}

long	lc_sysFcntl(int fd, int cmd, long arg)
{
	return (lc_syscall3(LC_NR_FCNTL, fd, cmd, arg));
}

/* exit_group does not return; the loop tells the compiler so. */
void	lc_sysExit(int status)
{
	for (;;)
		lc_syscall3(LC_NR_EXIT_GROUP, status, 0, 0);
}

/** "Relative to the current directory" for the *at() syscalls. */
#define LC_AT_FDCWD	(-100)

long	lc_sysOpen(const char *path, int flags, int mode)
{
	return (lc_syscall6(LC_NR_OPENAT, LC_AT_FDCWD, (long)path, flags, mode, 0, 0));
}

long	lc_sysMkdir(const char *path, int mode)
{
	return (lc_syscall6(LC_NR_MKDIRAT, LC_AT_FDCWD, (long)path, mode, 0, 0, 0));
}

long	lc_sysClose(int fd)
{
	return (lc_syscall3(LC_NR_CLOSE, fd, 0, 0));
}

void	*lc_sysMmap(unsigned long len)
{
	long	r;

	/* PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, no fd */
	r = lc_syscall6(LC_NR_MMAP, 0, (long)len, 3, 0x22, -1, 0);
	/* mmap returns an address, or -errno in [-4095, -1] */
	if (r < 0 && r > -4096)
		return (0);
	return ((void *)r);
}

long	lc_sysUnlink(const char *path)
{
	return (lc_syscall6(LC_NR_UNLINKAT, LC_AT_FDCWD, (long)path, 0, 0, 0, 0));
}
