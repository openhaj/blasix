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
 * @Updated: 2026/11/28 12:41:20 by Moutig
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
 * @brief Anonymous shared read/write mapping: still shared after fork.
 *
 * @param len Size in bytes.
 * @return Start of the mapping, or NULL on failure.
 */
void	*lc_sysMmapShared(unsigned long len);

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

/**
 * @brief rmdir(2), through unlinkat(AT_FDCWD, ..., AT_REMOVEDIR).
 *
 * @param path Path of the (empty) directory.
 * @return 0, or -errno.
 */
long	lc_sysRmdir(const char *path);

/**
 * @brief symlink(2), through symlinkat(target, AT_FDCWD, path).
 *
 * @param target What the link contains (not checked).
 * @param path   Path of the link to create.
 * @return 0, or -errno.
 */
long	lc_sysSymlink(const char *target, const char *path);

/**
 * @brief chmod(2), through fchmodat(AT_FDCWD, ...).
 *
 * @param path Path of the file.
 * @param mode Permission bits.
 * @return 0, or -errno.
 */
long	lc_sysChmod(const char *path, int mode);

/**
 * @brief chdir(2).
 *
 * @param path New current directory.
 * @return 0, or -errno.
 */
long	lc_sysChdir(const char *path);

/**
 * @brief getcwd(2): the current directory, as the kernel resolves it.
 *
 * @param buf  Destination.
 * @param size Size of @p buf.
 * @return Length stored, terminating NUL included, or -errno.
 */
long	lc_sysGetcwd(char *buf, unsigned long size);

/**
 * @brief geteuid(2).
 *
 * @return Effective user id.
 */
long	lc_sysGeteuid(void);

/**
 * @brief gettid(2).
 *
 * @return Thread id of the caller.
 */
long	lc_sysGettid(void);

/**
 * @brief tgkill(2): send a signal to one thread.
 *
 * @param tgid Process (thread group) id.
 * @param tid  Thread id.
 * @param sig  Signal number.
 * @return 0, or -errno.
 */
long	lc_sysTgkill(long tgid, long tid, int sig);

/**
 * @brief rt_sigaction(2): install a handler, empty mask.
 *
 * Sets the restorer the kernel requires on x86_64 (SA_RESTORER);
 * the previous action is not returned.
 *
 * @param sig     Signal number.
 * @param handler Handler (one int argument).
 * @param flags   LC_SA_* flags.
 * @return 0, or -errno (-EINVAL for SIGKILL and SIGSTOP).
 */
long	lc_sysSigaction(int sig, void (*handler)(int), unsigned long flags);

/**
 * @brief rt_sigprocmask(SIG_BLOCK): add one signal to the blocked ones.
 *
 * @param sig Signal number (1 ... 64).
 * @return 0, or -errno.
 */
long	lc_sysBlockSignal(int sig);

/**
 * @brief read(2).
 *
 * @param fd  File descriptor.
 * @param buf Where to store the bytes.
 * @param n   Size of @p buf.
 * @return Bytes read (0 at end of file), or -errno.
 */
long	lc_sysRead(int fd, void *buf, unsigned long n);

/**
 * @brief lseek(2).
 *
 * @param fd     File descriptor.
 * @param off    Offset.
 * @param whence LC_SEEK_SET, LC_SEEK_CUR or LC_SEEK_END.
 * @return New offset from the start of the file, or -errno.
 */
long	lc_sysLseek(int fd, long off, int whence);

/**
 * @brief pipe(2), through pipe2(2).
 *
 * @param fds   Receives the read end in fds[0], the write end in fds[1].
 * @param flags 0 or LC_O_CLOEXEC.
 * @return 0, or -errno.
 */
long	lc_sysPipe(int fds[2], int flags);

/**
 * @brief dup2(2), through dup3(2).
 *
 * dup3 refuses @p oldfd == @p newfd (-EINVAL): like dup2, this case
 * returns @p newfd if it is open, -EBADF otherwise.
 *
 * @param oldfd Descriptor to duplicate.
 * @param newfd Number of the copy (closed first if open).
 * @return @p newfd, or -errno.
 */
long	lc_sysDup2(int oldfd, int newfd);

/**
 * @brief fork(2), through clone(SIGCHLD).
 *
 * The libc under test is not told: its atfork handlers do not run and
 * whatever it caches about the thread (tid, locks held by other
 * threads) stays as in the parent. The child should only use lc_*
 * helpers and the function under test, then end with lc_sysExit().
 *
 * @return Child pid in the parent, 0 in the child, or -errno.
 */
long	lc_sysFork(void);

/**
 * @brief wait4(2), without resource usage.
 *
 * @param pid     Child to wait for (-1: any child).
 * @param status  Receives the raw wait status (may be NULL).
 * @param options 0 or LC_WNOHANG.
 * @return Pid of the child, 0 (LC_WNOHANG, still running), or -errno.
 */
long	lc_sysWait4(long pid, int *status, int options);

/**
 * @brief kill(2).
 *
 * @param pid Process to signal.
 * @param sig Signal number (0: only check that @p pid exists).
 * @return 0, or -errno.
 */
long	lc_sysKill(long pid, int sig);

/**
 * @brief getppid(2).
 *
 * @return Process id of the parent.
 */
long	lc_sysGetppid(void);

/**
 * @brief getuid(2), getgid(2), getegid(2): real user ID, real and
 *        effective group IDs (lc_sysGeteuid gives the effective user ID).
 *
 * @return The ID.
 */
long	lc_sysGetuid(void);
long	lc_sysGetgid(void);
long	lc_sysGetegid(void);

/**
 * @brief ioctl(2), one pointer argument.
 *
 * @param fd  File descriptor.
 * @param req Request (Linux value).
 * @param arg Argument.
 * @return The result of the request, or -errno.
 */
long	lc_sysIoctl(int fd, unsigned long req, void *arg);

/**
 * @brief umask(2).
 *
 * @param mask New file mode creation mask.
 * @return The previous mask.
 */
long	lc_sysUmask(int mask);

/**
 * @brief statx(2), the basic statistics: the same layout on x86_64 and
 *        aarch64 (struct statx, 256 bytes; offsets in LC_STX_*).
 *
 * @param dirfd LC_AT_FDCWD, or the descriptor itself with path "" and
 *              LC_AT_EMPTY_PATH.
 * @param path  Path.
 * @param flags 0, LC_AT_EMPTY_PATH, LC_AT_SYMLINK_NOFOLLOW.
 * @param buf   At least 256 bytes, 8-aligned.
 * @return 0, or -errno.
 */
long	lc_sysStatx(int dirfd, const char *path, int flags, void *buf);

/** statx(2): dirfd for the current directory, flags, field offsets. */
# define LC_AT_FDCWD			(-100)
# define LC_AT_SYMLINK_NOFOLLOW	0x100
# define LC_AT_EMPTY_PATH		0x1000
# define LC_STX_NLINK		16	/* u32 */
# define LC_STX_UID			20	/* u32 */
# define LC_STX_GID			24	/* u32 */
# define LC_STX_MODE		28	/* u16 */
# define LC_STX_INO			32	/* u64 */
# define LC_STX_SIZE		40	/* u64 */
# define LC_STX_MTIME		112	/* s64 seconds, u32 nanoseconds */
# define LC_STX_RDEV_MAJOR	128	/* u32 */
# define LC_STX_DEV_MAJOR	136	/* u32 */

/**
 * @brief prctl(2), one integer argument.
 *
 * @param op  LC_PR_* operation.
 * @param arg Argument of the operation.
 * @return Result of the operation, or -errno.
 */
long	lc_sysPrctl(int op, unsigned long arg);

/** Kernel struct timespec (64-bit: two longs). */
struct lc_timespec
{
	long	sec;
	long	nsec;
};

/**
 * @brief clock_gettime(2), always the syscall (not the vDSO).
 *
 * @param clk LC_CLOCK_REALTIME or LC_CLOCK_MONOTONIC.
 * @param ts  Receives the time.
 * @return 0, or -errno.
 */
long	lc_sysClockGettime(int clk, struct lc_timespec *ts);

/**
 * @brief clock_nanosleep(2).
 *
 * @param clk   Clock to sleep on.
 * @param flags 0 (relative) or LC_TIMER_ABSTIME.
 * @param req   Duration, or end time with LC_TIMER_ABSTIME.
 * @param rem   Receives what is left when interrupted (may be NULL).
 * @return 0, or -errno (-EINTR: a handler ran).
 */
long	lc_sysClockNanosleep(int clk, int flags, const struct lc_timespec *req,
			struct lc_timespec *rem);

/* ----- Constants (Linux values, same on x86_64 and aarch64) ----- */

/** fcntl command: get the descriptor flags (used to probe fd 3). */
# define LC_F_GETFD		1

/** fcntl commands: get and set the file status flags (LC_O_APPEND, LC_O_NONBLOCK). */
# define LC_F_GETFL		3
# define LC_F_SETFL		4

/** fcntl command: lowest free descriptor >= arg, close-on-exec set. */
# define LC_F_DUPFD_CLOEXEC	1030

/** open flag: read only. */
# define LC_O_RDONLY	00

/** open flag: write only. */
# define LC_O_WRONLY	01

/** open flag: read and write. */
# define LC_O_RDWR		02

/** open flag: create the file if it does not exist. */
# define LC_O_CREAT		0100

/** open flag: truncate the file to length 0. */
# define LC_O_TRUNC		01000

/** open flag: every write goes to the end of the file. */
# define LC_O_APPEND	02000

/** open / pipe / fcntl flag: non-blocking I/O. */
# define LC_O_NONBLOCK	04000

/** open / pipe flag: close the descriptor on exec. */
# define LC_O_CLOEXEC	02000000

/** lseek: offset from the start of the file. */
# define LC_SEEK_SET	0

/** lseek: offset from the current position. */
# define LC_SEEK_CUR	1

/** lseek: offset from the end of the file. */
# define LC_SEEK_END	2

/** wait4 option: return 0 at once if the child is still running. */
# define LC_WNOHANG		1

/** prctl: signal received when the parent dies. */
# define LC_PR_SET_PDEATHSIG	1

/** Clocks of clock_gettime / clock_nanosleep. */
# define LC_CLOCK_REALTIME	0
# define LC_CLOCK_MONOTONIC	1

/** clock_nanosleep flag: @p req is an absolute time. */
# define LC_TIMER_ABSTIME	1

/** Signal numbers (same on x86_64 and aarch64). */
# define LC_SIGKILL		9

/** SIGABRT (6 on x86_64 and aarch64). */
# define LC_SIGABRT		6

/** Signal dispositions of sigaction: default, ignore. */
# define LC_SIG_DFL		((void (*)(int))0)
# define LC_SIG_IGN		((void (*)(int))1)

/** sigaction flag: back to SIG_DFL when the handler is entered. */
# define LC_SA_RESETHAND	0x80000000UL

/** sigaction flag: do not block the signal inside its handler. */
# define LC_SA_NODEFER		0x40000000UL

#endif /* LC_SYS_H */
