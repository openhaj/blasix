# <unistd.h> - standard symbolic constants and types.
#
# ISO C: none (not an ISO C header: no ISO C target requires it).
# POSIX: XBD <unistd.h>; XSH access() ... write(), XSH 2.2.2.
#   2001  _POSIX_VERSION 200112L; the option constants (_POSIX_THREADS,
#         _POSIX_TIMERS, ... under their option); the _CS_, _PC_, _SC_
#         names; F_OK ... X_OK, SEEK_*, *_FILENO, _POSIX_VDISABLE; size_t,
#         ssize_t, uid_t, gid_t, off_t, pid_t, useconds_t, intptr_t;
#         optarg, opterr, optind, optopt; the functions access ... write;
#         XSI: ctermid, fchdir, getpgid, getsid, lchown,
#         lockf, nice, pread, pwrite, setreuid, swab, sync, truncate, ...;
#         XSI LEGACY or OB: getwd, ualarm, usleep, vfork; TSF: getlogin_r,
#         ttyname_r; Encryption Option Group (XSI): crypt, encrypt
#   2008  _POSIX_VERSION 200809L; the options AIO, BAR, CS, MF, MPR, RTS,
#         SEM, SPI, THR, TMO, TMR, TSF move to the base (their constants
#         "shall always be" 200809L); + faccessat, fchownat, fexecve,
#         linkat, readlinkat, symlinkat, unlinkat, _CS_POSIX_V7_*,
#         _SC_V7_*, _PC_TIMESTAMP_RESOLUTION, ...; fchdir, getpgid, getsid,
#         lchown, pread, pwrite, truncate, ctermid move to the base;
#         getwd, ualarm, usleep, vfork, useconds_t, _XOPEN_LEGACY removed;
#         setpgrp, the trace and V6 names obsolescent; SEEK_* and NULL "as
#         described in" <stdio.h> and <stddef.h>
#   2017  same interfaces as 2008; ctermid() may also be declared, "as
#         defined in <stdio.h>" (no longer required); F_LOCK ... F_ULOCK
#         shaded XSI as lockf() (unshaded before: taken as XSI throughout)
#   2024  _POSIX_VERSION 202405L; + _Fork, dup3, getentropy, pipe2,
#         posix_close, POSIX_CLOSE_RESTART, SEEK_DATA, SEEK_HOLE,
#         O_CLOEXEC, O_CLOFORK (as in <fcntl.h>), _CS_POSIX_V8_*, _SC_V8_*,
#         _SC_NPROCESSORS_*, _SC_NSIG, ...; XSI getresgid, getresuid,
#         setresgid, setresuid; _POSIX_MONOTONIC_CLOCK in the base; the
#         trace, STREAMS and V6 names, setpgrp removed; encrypt and the V7
#         names obsolescent
#
# _XOPEN_UNIX (an XSI system "shall set" it to a value other than -1,
# XBD 2.1.4), _XOPEN_ENH_I18N and _XOPEN_SHM ("always") are required with
# XSI; the other XSI option groups are optional: _XOPEN_CRYPT with
# CRYPT, _XOPEN_REALTIME, _XOPEN_REALTIME_THREADS, _XOPEN_STREAMS (and
# _XOPEN_LEGACY, 2001) not listed.
#
# Not listed (not required): the constants that "if defined" may be in
# the header without an option (_POSIX2_CHAR_TERM, _POSIX2_LOCALEDEF, _POSIX2_SYMLINKS, _POSIX_ASYNC_IO, _POSIX_FALLOC, _POSIX_PRIO_IO, _POSIX_SYNC_IO, _POSIX_TIMESTAMP_RESOLUTION, _POSIX_V6_ILP32_OFF32, _POSIX_V6_ILP32_OFFBIG, _POSIX_V6_LP64_OFF64, _POSIX_V6_LPBIG_OFFBIG, _POSIX_V7_ILP32_OFF32, _POSIX_V7_ILP32_OFFBIG, _POSIX_V7_LP64_OFF64, _POSIX_V7_LPBIG_OFFBIG, _POSIX_V8_ILP32_OFF32, _POSIX_V8_ILP32_OFFBIG, _POSIX_V8_LP64_OFF64, _POSIX_V8_LPBIG_OFFBIG): their
# names start with an underscore and may be visible anyway; the constants
# of the shell and utilities options CD, FD, FR, SD, UP, BE, UU (_POSIX2_C_DEV, _POSIX2_FORT_DEV, _POSIX2_FORT_RUN, _POSIX2_PBS, _POSIX2_PBS_ACCOUNTING, _POSIX2_PBS_CHECKPOINT, _POSIX2_PBS_LOCATE, _POSIX2_PBS_MESSAGE, _POSIX2_PBS_TRACK, _POSIX2_SW_DEV, _POSIX2_UPE, _XOPEN_UUCP),
# options lcrun does not model; _CS_XBS5_* and _XBS5_* (2001, LEGACY,
# "reserved for compatibility").
# The option constants are tagged with their option: a target requires
# them only with that option (OPTS=...), and the leading underscore lets
# them stay visible otherwise.
#
# Host deviations (glibc 2.44):
#   - _POSIX_VERSION, _POSIX2_VERSION, _POSIX2_C_BIND and the constants
#     that "shall always be set to the value" of the edition are 200809L
#     under _POSIX_C_SOURCE=202405L, not 202405L (glibc claims POSIX.1-2008):
#     their always_the_value_of_the_edition cases fail in POSIX.1-2024;
#   - fsync and fdatasync are visible without FSC / SIO, getlogin_r and
#     ttyname_r without TSF (POSIX.1-2001), setpgrp in POSIX.1-2024 where it
#     is removed: leaks (XSH 2.2.1);
#   - not declared: ctermid (in <stdio.h> only), crypt and encrypt (no
#     Encryption Option Group: correct, CRYPT off by default),
#     _PC_TIMESTAMP_RESOLUTION, _SC_XOPEN_UUCP, _CS_POSIX_V7_THREADS_CFLAGS
#     and _LDFLAGS (2008, 2017);
#   - sysconf(_SC_HOST_NAME_MAX) is 64, below _POSIX_HOST_NAME_MAX (255,
#     XBD <limits.h>); _POSIX_THREAD_ROBUST_PRIO_INHERIT is 200809L in the
#     header but sysconf(_SC_THREAD_ROBUST_PRIO_INHERIT) is -1 (XBD 2.1.6:
#     a constant greater than 0 means always supported at run time);
#   - getentropy() of more than GETENTROPY_MAX bytes fails with EIO, not
#     EINVAL (XSH getentropy(), tested with LAX=1: not declared in strict
#     mode);
#   - unlink() of a directory fails with EISDIR, the Linux errno, not
#     EPERM (XSH unlink() ERRORS): unistd/unlink/directory_gives_eperm
#     fails (with any libc that passes the kernel's errno through);
#   - POSIX.1-2024: none of its additions is declared under
#     _POSIX_C_SOURCE=202405L (_Fork, dup3, getentropy, pipe2, posix_close,
#     the XSI getres*id / setres*id, POSIX_CLOSE_RESTART, SEEK_DATA,
#     SEEK_HOLE, O_CLOEXEC, O_CLOFORK, the V8 names, _PC_FALLOC,
#     _PC_TEXTDOMAIN_MAX, _SC_DEVICE_CONTROL, _SC_NSIG).

header unistd.h

# Reserved: XSH 2.2.2 (SEEK_ for <unistd.h>; posix_, POSIX_ for any
# header; _CS_, _PC_, _SC_ and _POSIX_ are reserved anyway: leading
# underscore). ctermid and pthread_atfork may be declared too ("may also
# include the prototype": pthread_atfork from 2001, ctermid from 2017;
# 2024: <unistd.h> may make visible all of <stdio.h>).
reserved POSIX.1-2001  *_t SEEK_* posix_* POSIX_* pthread_atfork
reserved POSIX.1-2017  ctermid

# ----- Version test macros -----
# name                      since          tags
_POSIX2_VERSION                     POSIX.1-2001   macro
_POSIX_VERSION                      POSIX.1-2001   macro
_XOPEN_VERSION                      POSIX.1-2001   macro XSI

# ----- Constants for options and option groups -----
_POSIX2_C_BIND                      POSIX.1-2001   macro
_POSIX_ADVISORY_INFO                POSIX.1-2001   macro ADV
_POSIX_ASYNCHRONOUS_IO              POSIX.1-2001   macro AIO base=POSIX.1-2008
_POSIX_BARRIERS                     POSIX.1-2001   macro BAR base=POSIX.1-2008
_POSIX_CHOWN_RESTRICTED             POSIX.1-2008   macro
_POSIX_CLOCK_SELECTION              POSIX.1-2001   macro CS base=POSIX.1-2008
_POSIX_CPUTIME                      POSIX.1-2001   macro CPT
_POSIX_DEVICE_CONTROL               POSIX.1-2024   macro DC
_POSIX_FSYNC                        POSIX.1-2001   macro FSC
_POSIX_IPV6                         POSIX.1-2017   macro IP6
_POSIX_JOB_CONTROL                  POSIX.1-2001   macro
_POSIX_MAPPED_FILES                 POSIX.1-2001   macro MF base=POSIX.1-2008
_POSIX_MEMLOCK                      POSIX.1-2001   macro ML
_POSIX_MEMLOCK_RANGE                POSIX.1-2001   macro MLR
_POSIX_MEMORY_PROTECTION            POSIX.1-2001   macro MPR base=POSIX.1-2008
_POSIX_MESSAGE_PASSING              POSIX.1-2001   macro MSG
_POSIX_MONOTONIC_CLOCK              POSIX.1-2001   macro MON base=POSIX.1-2024
_POSIX_NO_TRUNC                     POSIX.1-2008   macro
_POSIX_PRIORITIZED_IO               POSIX.1-2001   macro PIO
_POSIX_PRIORITY_SCHEDULING          POSIX.1-2001   macro PS
_POSIX_RAW_SOCKETS                  POSIX.1-2001   macro RS
_POSIX_READER_WRITER_LOCKS          POSIX.1-2001   macro THR base=POSIX.1-2008
_POSIX_REALTIME_SIGNALS             POSIX.1-2001   macro RTS base=POSIX.1-2008
_POSIX_REGEXP                       POSIX.1-2001   macro
_POSIX_SAVED_IDS                    POSIX.1-2001   macro
_POSIX_SEMAPHORES                   POSIX.1-2001   macro SEM base=POSIX.1-2008
_POSIX_SHARED_MEMORY_OBJECTS        POSIX.1-2001   macro SHM
_POSIX_SHELL                        POSIX.1-2001   macro
_POSIX_SPAWN                        POSIX.1-2001   macro SPN
_POSIX_SPIN_LOCKS                   POSIX.1-2001   macro SPI base=POSIX.1-2008
_POSIX_SPORADIC_SERVER              POSIX.1-2001   macro SS
_POSIX_SYNCHRONIZED_IO              POSIX.1-2001   macro SIO
_POSIX_THREADS                      POSIX.1-2001   macro THR base=POSIX.1-2008
_POSIX_THREAD_ATTR_STACKADDR        POSIX.1-2001   macro TSA
_POSIX_THREAD_ATTR_STACKSIZE        POSIX.1-2001   macro TSS
_POSIX_THREAD_CPUTIME               POSIX.1-2001   macro TCT
_POSIX_THREAD_PRIORITY_SCHEDULING   POSIX.1-2001   macro TPS
_POSIX_THREAD_PRIO_INHERIT          POSIX.1-2001   macro TPI
_POSIX_THREAD_PRIO_PROTECT          POSIX.1-2001   macro TPP
_POSIX_THREAD_PROCESS_SHARED        POSIX.1-2001   macro TSH
_POSIX_THREAD_ROBUST_PRIO_INHERIT   POSIX.1-2008   macro RPI
_POSIX_THREAD_ROBUST_PRIO_PROTECT   POSIX.1-2008   macro RPP
_POSIX_THREAD_SAFE_FUNCTIONS        POSIX.1-2001   macro TSF base=POSIX.1-2008
_POSIX_THREAD_SPORADIC_SERVER       POSIX.1-2001   macro TSP
_POSIX_TIMEOUTS                     POSIX.1-2001   macro TMO base=POSIX.1-2008
_POSIX_TIMERS                       POSIX.1-2001   macro TMR base=POSIX.1-2008
_POSIX_TRACE                        POSIX.1-2001   macro TRC ob=POSIX.1-2008 removed=POSIX.1-2024
_POSIX_TRACE_EVENT_FILTER           POSIX.1-2001   macro TEF ob=POSIX.1-2008 removed=POSIX.1-2024
_POSIX_TRACE_INHERIT                POSIX.1-2001   macro TRI ob=POSIX.1-2008 removed=POSIX.1-2024
_POSIX_TRACE_LOG                    POSIX.1-2001   macro TRL ob=POSIX.1-2008 removed=POSIX.1-2024
_POSIX_TYPED_MEMORY_OBJECTS         POSIX.1-2001   macro TYM
_XOPEN_CRYPT                        POSIX.1-2001   macro CRYPT
_XOPEN_ENH_I18N                     POSIX.1-2001   macro XSI
_XOPEN_SHM                          POSIX.1-2001   macro XSI
_XOPEN_UNIX                         POSIX.1-2001   macro XSI

# ----- Constants for functions -----
NULL                                POSIX.1-2001   macro
SEEK_SET                            POSIX.1-2001   macro same=stdio.h
SEEK_CUR                            POSIX.1-2001   macro same=stdio.h
SEEK_END                            POSIX.1-2001   macro same=stdio.h
SEEK_DATA                           POSIX.1-2024   macro
SEEK_HOLE                           POSIX.1-2024   macro
O_CLOEXEC                           POSIX.1-2024   macro same=fcntl.h
O_CLOFORK                           POSIX.1-2024   macro same=fcntl.h
# F_LOCK ... F_ULOCK: unshaded in 2001 and 2008, XSI from 2017 (TC2), as lockf().
F_LOCK                              POSIX.1-2001   macro XSI
F_OK                                POSIX.1-2001   macro
F_TEST                              POSIX.1-2001   macro XSI
F_TLOCK                             POSIX.1-2001   macro XSI
F_ULOCK                             POSIX.1-2001   macro XSI
POSIX_CLOSE_RESTART                 POSIX.1-2024   macro
R_OK                                POSIX.1-2001   macro
STDERR_FILENO                       POSIX.1-2001   macro
STDIN_FILENO                        POSIX.1-2001   macro
STDOUT_FILENO                       POSIX.1-2001   macro
W_OK                                POSIX.1-2001   macro
X_OK                                POSIX.1-2001   macro
_POSIX_VDISABLE                     POSIX.1-2001   macro

# confstr() names
_CS_PATH                            POSIX.1-2001   macro
_CS_POSIX_V6_ILP32_OFF32_CFLAGS     POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_ILP32_OFF32_LDFLAGS    POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_ILP32_OFF32_LIBS       POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS    POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS   POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_ILP32_OFFBIG_LIBS      POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_LP64_OFF64_CFLAGS      POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_LP64_OFF64_LDFLAGS     POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_LP64_OFF64_LIBS        POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS    POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS   POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_LPBIG_OFFBIG_LIBS      POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS  POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_POSIX_V7_ILP32_OFF32_CFLAGS     POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_ILP32_OFF32_LDFLAGS    POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_ILP32_OFF32_LIBS       POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS    POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS   POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_ILP32_OFFBIG_LIBS      POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_LP64_OFF64_CFLAGS      POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_LP64_OFF64_LDFLAGS     POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_LP64_OFF64_LIBS        POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS    POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS   POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_LPBIG_OFFBIG_LIBS      POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_THREADS_CFLAGS         POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_THREADS_LDFLAGS        POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V7_WIDTH_RESTRICTED_ENVS  POSIX.1-2008   macro ob=POSIX.1-2024
_CS_POSIX_V8_ILP32_OFF32_CFLAGS     POSIX.1-2024   macro
_CS_POSIX_V8_ILP32_OFF32_LDFLAGS    POSIX.1-2024   macro
_CS_POSIX_V8_ILP32_OFF32_LIBS       POSIX.1-2024   macro
_CS_POSIX_V8_ILP32_OFFBIG_CFLAGS    POSIX.1-2024   macro
_CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS   POSIX.1-2024   macro
_CS_POSIX_V8_ILP32_OFFBIG_LIBS      POSIX.1-2024   macro
_CS_POSIX_V8_LP64_OFF64_CFLAGS      POSIX.1-2024   macro
_CS_POSIX_V8_LP64_OFF64_LDFLAGS     POSIX.1-2024   macro
_CS_POSIX_V8_LP64_OFF64_LIBS        POSIX.1-2024   macro
_CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS    POSIX.1-2024   macro
_CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS   POSIX.1-2024   macro
_CS_POSIX_V8_LPBIG_OFFBIG_LIBS      POSIX.1-2024   macro
_CS_POSIX_V8_THREADS_CFLAGS         POSIX.1-2024   macro
_CS_POSIX_V8_THREADS_LDFLAGS        POSIX.1-2024   macro
_CS_POSIX_V8_WIDTH_RESTRICTED_ENVS  POSIX.1-2024   macro
_CS_V6_ENV                          POSIX.1-2008   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_CS_V7_ENV                          POSIX.1-2008   macro ob=POSIX.1-2024
_CS_V8_ENV                          POSIX.1-2024   macro

# pathconf() names
_PC_2_SYMLINKS                      POSIX.1-2001   macro
_PC_ALLOC_SIZE_MIN                  POSIX.1-2001   macro
_PC_ASYNC_IO                        POSIX.1-2001   macro
_PC_CHOWN_RESTRICTED                POSIX.1-2001   macro
_PC_FALLOC                          POSIX.1-2024   macro
_PC_FILESIZEBITS                    POSIX.1-2001   macro
_PC_LINK_MAX                        POSIX.1-2001   macro
_PC_MAX_CANON                       POSIX.1-2001   macro
_PC_MAX_INPUT                       POSIX.1-2001   macro
_PC_NAME_MAX                        POSIX.1-2001   macro
_PC_NO_TRUNC                        POSIX.1-2001   macro
_PC_PATH_MAX                        POSIX.1-2001   macro
_PC_PIPE_BUF                        POSIX.1-2001   macro
_PC_PRIO_IO                         POSIX.1-2001   macro
_PC_REC_INCR_XFER_SIZE              POSIX.1-2001   macro
_PC_REC_MAX_XFER_SIZE               POSIX.1-2008   macro
_PC_REC_MIN_XFER_SIZE               POSIX.1-2001   macro
_PC_REC_XFER_ALIGN                  POSIX.1-2001   macro
_PC_SYMLINK_MAX                     POSIX.1-2001   macro
_PC_SYNC_IO                         POSIX.1-2001   macro
_PC_TEXTDOMAIN_MAX                  POSIX.1-2024   macro
_PC_TIMESTAMP_RESOLUTION            POSIX.1-2008   macro
_PC_VDISABLE                        POSIX.1-2001   macro

# sysconf() names
_SC_2_CHAR_TERM                     POSIX.1-2001   macro
_SC_2_C_BIND                        POSIX.1-2001   macro
_SC_2_C_DEV                         POSIX.1-2001   macro
_SC_2_FORT_DEV                      POSIX.1-2001   macro removed=POSIX.1-2024
_SC_2_FORT_RUN                      POSIX.1-2001   macro
_SC_2_LOCALEDEF                     POSIX.1-2001   macro
_SC_2_PBS                           POSIX.1-2001   macro removed=POSIX.1-2024
_SC_2_PBS_ACCOUNTING                POSIX.1-2001   macro removed=POSIX.1-2024
_SC_2_PBS_CHECKPOINT                POSIX.1-2001   macro removed=POSIX.1-2024
_SC_2_PBS_LOCATE                    POSIX.1-2001   macro removed=POSIX.1-2024
_SC_2_PBS_MESSAGE                   POSIX.1-2001   macro removed=POSIX.1-2024
_SC_2_PBS_TRACK                     POSIX.1-2001   macro removed=POSIX.1-2024
_SC_2_SW_DEV                        POSIX.1-2001   macro
_SC_2_UPE                           POSIX.1-2001   macro
_SC_2_VERSION                       POSIX.1-2001   macro
_SC_ADVISORY_INFO                   POSIX.1-2001   macro
_SC_AIO_LISTIO_MAX                  POSIX.1-2001   macro
_SC_AIO_MAX                         POSIX.1-2001   macro
_SC_AIO_PRIO_DELTA_MAX              POSIX.1-2001   macro
_SC_ARG_MAX                         POSIX.1-2001   macro
_SC_ASYNCHRONOUS_IO                 POSIX.1-2001   macro
_SC_ATEXIT_MAX                      POSIX.1-2001   macro
_SC_BARRIERS                        POSIX.1-2001   macro
_SC_BC_BASE_MAX                     POSIX.1-2001   macro
_SC_BC_DIM_MAX                      POSIX.1-2001   macro
_SC_BC_SCALE_MAX                    POSIX.1-2001   macro
_SC_BC_STRING_MAX                   POSIX.1-2001   macro
_SC_CHILD_MAX                       POSIX.1-2001   macro
_SC_CLK_TCK                         POSIX.1-2001   macro
_SC_CLOCK_SELECTION                 POSIX.1-2001   macro
_SC_COLL_WEIGHTS_MAX                POSIX.1-2001   macro
_SC_CPUTIME                         POSIX.1-2001   macro
_SC_DELAYTIMER_MAX                  POSIX.1-2001   macro
_SC_DEVICE_CONTROL                  POSIX.1-2024   macro
_SC_EXPR_NEST_MAX                   POSIX.1-2001   macro
_SC_FSYNC                           POSIX.1-2001   macro
_SC_GETGR_R_SIZE_MAX                POSIX.1-2001   macro
_SC_GETPW_R_SIZE_MAX                POSIX.1-2001   macro
_SC_HOST_NAME_MAX                   POSIX.1-2001   macro
_SC_IOV_MAX                         POSIX.1-2001   macro
_SC_IPV6                            POSIX.1-2001   macro
_SC_JOB_CONTROL                     POSIX.1-2001   macro
_SC_LINE_MAX                        POSIX.1-2001   macro
_SC_LOGIN_NAME_MAX                  POSIX.1-2001   macro
_SC_MAPPED_FILES                    POSIX.1-2001   macro
_SC_MEMLOCK                         POSIX.1-2001   macro
_SC_MEMLOCK_RANGE                   POSIX.1-2001   macro
_SC_MEMORY_PROTECTION               POSIX.1-2001   macro
_SC_MESSAGE_PASSING                 POSIX.1-2001   macro
_SC_MONOTONIC_CLOCK                 POSIX.1-2001   macro
_SC_MQ_OPEN_MAX                     POSIX.1-2001   macro
_SC_MQ_PRIO_MAX                     POSIX.1-2001   macro
_SC_NGROUPS_MAX                     POSIX.1-2001   macro
_SC_NPROCESSORS_CONF                POSIX.1-2024   macro
_SC_NPROCESSORS_ONLN                POSIX.1-2024   macro
_SC_NSIG                            POSIX.1-2024   macro
_SC_OPEN_MAX                        POSIX.1-2001   macro
_SC_PAGESIZE                        POSIX.1-2001   macro
_SC_PAGE_SIZE                       POSIX.1-2001   macro
_SC_PRIORITIZED_IO                  POSIX.1-2001   macro
_SC_PRIORITY_SCHEDULING             POSIX.1-2001   macro
_SC_RAW_SOCKETS                     POSIX.1-2001   macro
_SC_READER_WRITER_LOCKS             POSIX.1-2001   macro
_SC_REALTIME_SIGNALS                POSIX.1-2001   macro
_SC_REGEXP                          POSIX.1-2001   macro
_SC_RE_DUP_MAX                      POSIX.1-2001   macro
_SC_RTSIG_MAX                       POSIX.1-2001   macro
_SC_SAVED_IDS                       POSIX.1-2001   macro
_SC_SEMAPHORES                      POSIX.1-2001   macro
_SC_SEM_NSEMS_MAX                   POSIX.1-2001   macro
_SC_SEM_VALUE_MAX                   POSIX.1-2001   macro
_SC_SHARED_MEMORY_OBJECTS           POSIX.1-2001   macro
_SC_SHELL                           POSIX.1-2001   macro
_SC_SIGQUEUE_MAX                    POSIX.1-2001   macro
_SC_SPAWN                           POSIX.1-2001   macro
_SC_SPIN_LOCKS                      POSIX.1-2001   macro
_SC_SPORADIC_SERVER                 POSIX.1-2001   macro
_SC_SS_REPL_MAX                     POSIX.1-2001   macro
_SC_STREAM_MAX                      POSIX.1-2001   macro
_SC_SYMLOOP_MAX                     POSIX.1-2001   macro
_SC_SYNCHRONIZED_IO                 POSIX.1-2001   macro
_SC_THREADS                         POSIX.1-2001   macro
_SC_THREAD_ATTR_STACKADDR           POSIX.1-2001   macro
_SC_THREAD_ATTR_STACKSIZE           POSIX.1-2001   macro
_SC_THREAD_CPUTIME                  POSIX.1-2001   macro
_SC_THREAD_DESTRUCTOR_ITERATIONS    POSIX.1-2001   macro
_SC_THREAD_KEYS_MAX                 POSIX.1-2001   macro
_SC_THREAD_PRIORITY_SCHEDULING      POSIX.1-2001   macro
_SC_THREAD_PRIO_INHERIT             POSIX.1-2001   macro
_SC_THREAD_PRIO_PROTECT             POSIX.1-2001   macro
_SC_THREAD_PROCESS_SHARED           POSIX.1-2001   macro
_SC_THREAD_ROBUST_PRIO_INHERIT      POSIX.1-2008   macro
_SC_THREAD_ROBUST_PRIO_PROTECT      POSIX.1-2008   macro
_SC_THREAD_SAFE_FUNCTIONS           POSIX.1-2001   macro
_SC_THREAD_SPORADIC_SERVER          POSIX.1-2001   macro
_SC_THREAD_STACK_MIN                POSIX.1-2001   macro
_SC_THREAD_THREADS_MAX              POSIX.1-2001   macro
_SC_TIMEOUTS                        POSIX.1-2001   macro
_SC_TIMERS                          POSIX.1-2001   macro
_SC_TIMER_MAX                       POSIX.1-2001   macro
_SC_TRACE                           POSIX.1-2001   macro removed=POSIX.1-2024
_SC_TRACE_EVENT_FILTER              POSIX.1-2001   macro removed=POSIX.1-2024
_SC_TRACE_EVENT_NAME_MAX            POSIX.1-2001   macro removed=POSIX.1-2024
_SC_TRACE_INHERIT                   POSIX.1-2001   macro removed=POSIX.1-2024
_SC_TRACE_LOG                       POSIX.1-2001   macro removed=POSIX.1-2024
_SC_TRACE_NAME_MAX                  POSIX.1-2001   macro removed=POSIX.1-2024
_SC_TRACE_SYS_MAX                   POSIX.1-2001   macro removed=POSIX.1-2024
_SC_TRACE_USER_EVENT_MAX            POSIX.1-2001   macro removed=POSIX.1-2024
_SC_TTY_NAME_MAX                    POSIX.1-2001   macro
_SC_TYPED_MEMORY_OBJECTS            POSIX.1-2001   macro
_SC_TZNAME_MAX                      POSIX.1-2001   macro
_SC_V6_ILP32_OFF32                  POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_SC_V6_ILP32_OFFBIG                 POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_SC_V6_LP64_OFF64                   POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_SC_V6_LPBIG_OFFBIG                 POSIX.1-2001   macro ob=POSIX.1-2008 removed=POSIX.1-2024
_SC_V7_ILP32_OFF32                  POSIX.1-2008   macro ob=POSIX.1-2024
_SC_V7_ILP32_OFFBIG                 POSIX.1-2008   macro ob=POSIX.1-2024
_SC_V7_LP64_OFF64                   POSIX.1-2008   macro ob=POSIX.1-2024
_SC_V7_LPBIG_OFFBIG                 POSIX.1-2008   macro ob=POSIX.1-2024
_SC_V8_ILP32_OFF32                  POSIX.1-2024   macro
_SC_V8_ILP32_OFFBIG                 POSIX.1-2024   macro
_SC_V8_LP64_OFF64                   POSIX.1-2024   macro
_SC_V8_LPBIG_OFFBIG                 POSIX.1-2024   macro
_SC_VERSION                         POSIX.1-2001   macro
_SC_XOPEN_CRYPT                     POSIX.1-2001   macro
_SC_XOPEN_ENH_I18N                  POSIX.1-2001   macro
_SC_XOPEN_LEGACY                    POSIX.1-2001   macro removed=POSIX.1-2008
_SC_XOPEN_REALTIME                  POSIX.1-2001   macro
_SC_XOPEN_REALTIME_THREADS          POSIX.1-2001   macro
_SC_XOPEN_SHM                       POSIX.1-2001   macro
_SC_XOPEN_STREAMS                   POSIX.1-2001   macro removed=POSIX.1-2024
_SC_XOPEN_UNIX                      POSIX.1-2001   macro
_SC_XOPEN_UUCP                      POSIX.1-2008   macro
_SC_XOPEN_VERSION                   POSIX.1-2001   macro

# ----- Types (as described in <sys/types.h>, intptr_t in <stdint.h>) -----
size_t                              POSIX.1-2001   type same=stddef.h
ssize_t                             POSIX.1-2001   type same=sys/types.h
uid_t                               POSIX.1-2001   type same=sys/types.h
gid_t                               POSIX.1-2001   type same=sys/types.h
off_t                               POSIX.1-2001   type same=sys/types.h
pid_t                               POSIX.1-2001   type same=sys/types.h
useconds_t                          POSIX.1-2001   type removed=POSIX.1-2008 same=sys/types.h
intptr_t                            POSIX.1-2001   type same=stdint.h

# ----- External variables (getopt) -----
optarg                              POSIX.1-2001   var
opterr                              POSIX.1-2001   var
optind                              POSIX.1-2001   var
optopt                              POSIX.1-2001   var

# ----- Functions -----
access                              POSIX.1-2001
alarm                               POSIX.1-2001
chdir                               POSIX.1-2001
chown                               POSIX.1-2001
close                               POSIX.1-2001
confstr                             POSIX.1-2001
dup                                 POSIX.1-2001
dup2                                POSIX.1-2001
execl                               POSIX.1-2001
execle                              POSIX.1-2001
execlp                              POSIX.1-2001
execv                               POSIX.1-2001
execve                              POSIX.1-2001
execvp                              POSIX.1-2001
_exit                               POSIX.1-2001
fchown                              POSIX.1-2001
fork                                POSIX.1-2001
fpathconf                           POSIX.1-2001
ftruncate                           POSIX.1-2001
getcwd                              POSIX.1-2001
getegid                             POSIX.1-2001
geteuid                             POSIX.1-2001
getgid                              POSIX.1-2001
getgroups                           POSIX.1-2001
gethostname                         POSIX.1-2001
getlogin                            POSIX.1-2001
getopt                              POSIX.1-2001
getpgrp                             POSIX.1-2001
getpid                              POSIX.1-2001
getppid                             POSIX.1-2001
getuid                              POSIX.1-2001
isatty                              POSIX.1-2001
link                                POSIX.1-2001
lseek                               POSIX.1-2001
pathconf                            POSIX.1-2001
pause                               POSIX.1-2001
pipe                                POSIX.1-2001
read                                POSIX.1-2001
readlink                            POSIX.1-2001
rmdir                               POSIX.1-2001
setegid                             POSIX.1-2001
seteuid                             POSIX.1-2001
setgid                              POSIX.1-2001
setpgid                             POSIX.1-2001
setsid                              POSIX.1-2001
setuid                              POSIX.1-2001
sleep                               POSIX.1-2001
symlink                             POSIX.1-2001
sysconf                             POSIX.1-2001
tcgetpgrp                           POSIX.1-2001
tcsetpgrp                           POSIX.1-2001
ttyname                             POSIX.1-2001
unlink                              POSIX.1-2001
write                               POSIX.1-2001
# 2008 (the *at functions, fexecve)
faccessat                           POSIX.1-2008
fchownat                            POSIX.1-2008
fexecve                             POSIX.1-2008
linkat                              POSIX.1-2008
readlinkat                          POSIX.1-2008
symlinkat                           POSIX.1-2008
unlinkat                            POSIX.1-2008
# 2024
_Fork                               POSIX.1-2024
dup3                                POSIX.1-2024
getentropy                          POSIX.1-2024
pipe2                               POSIX.1-2024
posix_close                         POSIX.1-2024
# Thread-Safe Functions in 2001 (XSH margins), base from 2008
getlogin_r                          POSIX.1-2001   TSF base=POSIX.1-2008
ttyname_r                           POSIX.1-2001   TSF base=POSIX.1-2008
# File Synchronization, Synchronized Input and Output (every edition)
fsync                               POSIX.1-2001   FSC
fdatasync                           POSIX.1-2001   SIO
# XSI, base from 2008
fchdir                              POSIX.1-2001   XSI base=POSIX.1-2008
getpgid                             POSIX.1-2001   XSI base=POSIX.1-2008
getsid                              POSIX.1-2001   XSI base=POSIX.1-2008
lchown                              POSIX.1-2001   XSI base=POSIX.1-2008
pread                               POSIX.1-2001   XSI base=POSIX.1-2008
pwrite                              POSIX.1-2001   XSI base=POSIX.1-2008
truncate                            POSIX.1-2001   XSI base=POSIX.1-2008
# XSI, base in 2008, out of the list from 2017 ("may also include ... as in <stdio.h>")
ctermid                             POSIX.1-2001   XSI base=POSIX.1-2008 removed=POSIX.1-2017
# The X/Open Encryption Option Group (XBD 2.1.5.2, "CRYPT"): an XSI system
# may lack it, so it has an option of its own (OPTS=XSI,CRYPT)
crypt                               POSIX.1-2001   CRYPT
gethostid                           POSIX.1-2001   XSI
lockf                               POSIX.1-2001   XSI
nice                                POSIX.1-2001   XSI
setregid                            POSIX.1-2001   XSI
setreuid                            POSIX.1-2001   XSI
swab                                POSIX.1-2001   XSI
sync                                POSIX.1-2001   XSI
encrypt                             POSIX.1-2001   CRYPT ob=POSIX.1-2024
setpgrp                             POSIX.1-2001   XSI ob=POSIX.1-2008 removed=POSIX.1-2024
getresgid                           POSIX.1-2024   XSI
getresuid                           POSIX.1-2024   XSI
setresgid                           POSIX.1-2024   XSI
setresuid                           POSIX.1-2024   XSI
# XSI LEGACY (getwd) or obsolescent in 2001, removed in 2008
getwd                               POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
ualarm                              POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
usleep                              POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
vfork                               POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008

# ----- Extensions (EXT=1): in no standard -----
# getpagesize: 4.2BSD, LEGACY in SUSv2 and gone from POSIX.1-2001 (BSD
# getpagesize(3), glibc manual); gettid: Linux, glibc 2.30 gettid(2).
getpagesize                 EXT            ext=BSD
gettid                      EXT            ext=GNU
