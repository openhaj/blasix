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
 * @Updated: 2026/11/25 08:18:14 by Moutig
 *
 * Every system call of the framework goes through lc_syscall3 /
 * lc_syscall6, a few lines of inline asm per architecture, so that the
 * test side never calls the libc under test (not even its syscall()
 * wrapper or errno).
 *
 * Path syscalls use their *at() form with AT_FDCWD: aarch64 (like every
 * recent Linux port) has no open, mkdir or unlink, only openat,
 * mkdirat and unlinkat, and the *at() form works on x86_64 too. For
 * the same reason pipe, dup2 and fork go through pipe2, dup3 and
 * clone on both architectures.
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
# define LC_NR_MPROTECT		10
# define LC_NR_UNLINKAT		263
# define LC_NR_GETTID		186
# define LC_NR_TGKILL		234
# define LC_NR_RT_SIGACTION	13
# define LC_NR_RT_SIGRETURN	15
# define LC_NR_READ			0
# define LC_NR_LSEEK		8
# define LC_NR_PIPE2		293
# define LC_NR_DUP3			292
# define LC_NR_CLONE		56
# define LC_NR_WAIT4		61
# define LC_NR_KILL			62
# define LC_NR_GETPPID		110

/*
 * Signal return trampoline. The x86_64 kernel refuses to deliver a
 * signal to a handler installed without SA_RESTORER (it kills the
 * process with SIGSEGV instead), so lc_sysSigaction() always sets this.
 */
__asm__(
	".text\n"
	"\t.p2align 4\n"
	"\t.globl lc_sysSigRestorer\n"
	"\t.hidden lc_sysSigRestorer\n"
	"\t.type lc_sysSigRestorer, @function\n"
	"lc_sysSigRestorer:\n"
	"\tmovq $15, %rax\n"
	"\tsyscall\n"
	"\t.size lc_sysSigRestorer, . - lc_sysSigRestorer\n");

/** The trampoline above (rt_sigreturn = LC_NR_RT_SIGRETURN). */
void	lc_sysSigRestorer(void);

/** SA_RESTORER: sa_restorer is set. */
# define LC_SA_RESTORER		0x04000000UL
# define LC_SIG_RESTORER	lc_sysSigRestorer

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
# define LC_NR_MPROTECT		226
# define LC_NR_UNLINKAT		35
# define LC_NR_GETTID		178
# define LC_NR_TGKILL		131
# define LC_NR_RT_SIGACTION	134
# define LC_NR_READ			63
# define LC_NR_LSEEK		62
# define LC_NR_PIPE2		59
# define LC_NR_DUP3			24
# define LC_NR_CLONE		220
# define LC_NR_WAIT4		260
# define LC_NR_KILL			129
# define LC_NR_GETPPID		173

/* aarch64 returns through the vDSO trampoline: no SA_RESTORER needed. */
# define LC_SA_RESTORER		0UL
# define LC_SIG_RESTORER	((void (*)(void))0)

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

long	lc_sysMprotect(void *addr, unsigned long len, int prot)
{
	return (lc_syscall3(LC_NR_MPROTECT, (long)addr, (long)len, prot));
}

long	lc_sysUnlink(const char *path)
{
	return (lc_syscall6(LC_NR_UNLINKAT, LC_AT_FDCWD, (long)path, 0, 0, 0, 0));
}

long	lc_sysGettid(void)
{
	return (lc_syscall3(LC_NR_GETTID, 0, 0, 0));
}

long	lc_sysTgkill(long tgid, long tid, int sig)
{
	return (lc_syscall3(LC_NR_TGKILL, tgid, tid, sig));
}

/**
 * Layout the kernel expects from rt_sigaction (struct sigaction of
 * <asm/signal.h>, same on x86_64 and aarch64), not the libc's one.
 */
struct lc_kSigaction
{
	void			(*handler)(int);
	unsigned long	flags;
	void			(*restorer)(void);
	unsigned long	mask;		/* 64 signals: sigsetsize = 8 */
};

long	lc_sysSigaction(int sig, void (*handler)(int), unsigned long flags)
{
	struct lc_kSigaction	act;

	act.handler = handler;
	act.flags = flags | LC_SA_RESTORER;
	act.restorer = LC_SIG_RESTORER;
	act.mask = 0;
	return (lc_syscall6(LC_NR_RT_SIGACTION, sig, (long)&act, 0,
			(long)sizeof(act.mask), 0, 0));
}

long	lc_sysRead(int fd, void *buf, unsigned long n)
{
	return (lc_syscall3(LC_NR_READ, fd, (long)buf, (long)n));
}

long	lc_sysLseek(int fd, long off, int whence)
{
	return (lc_syscall3(LC_NR_LSEEK, fd, off, whence));
}

long	lc_sysPipe(int fds[2], int flags)
{
	return (lc_syscall3(LC_NR_PIPE2, (long)fds, flags, 0));
}

long	lc_sysDup2(int oldfd, int newfd)
{
	long	r;

	if (oldfd == newfd)
	{
		r = lc_sysFcntl(oldfd, LC_F_GETFD, 0);
		return (r < 0 ? r : newfd);
	}
	return (lc_syscall3(LC_NR_DUP3, oldfd, newfd, 0));
}

/** Signal sent to the parent when the child ends. */
#define LC_SIGCHLD	17

/*
 * clone(SIGCHLD, 0, ...) is fork(): no new stack (0 = share the
 * caller's, copied on write), no tid or tls pointer. x86_64 and aarch64
 * order the last three arguments differently; they are all 0 here.
 */
long	lc_sysFork(void)
{
	return (lc_syscall6(LC_NR_CLONE, LC_SIGCHLD, 0, 0, 0, 0, 0));
}

long	lc_sysWait4(long pid, int *status, int options)
{
	return (lc_syscall6(LC_NR_WAIT4, pid, (long)status, options, 0, 0, 0));
}

long	lc_sysKill(long pid, int sig)
{
	return (lc_syscall3(LC_NR_KILL, pid, sig, 0));
}

long	lc_sysGetppid(void)
{
	return (lc_syscall3(LC_NR_GETPPID, 0, 0, 0));
}
