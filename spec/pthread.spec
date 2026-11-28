# <pthread.h> - threads.
#
# ISO C: none (not an ISO C header; ISO C threads are <threads.h>).
# POSIX: XBD <pthread.h>, XBD <sys/types.h> (the pthread_*_t types); XSH
# pthread_*().
#   2001  the whole header is THR; inside it: BAR (barriers,
#         PTHREAD_BARRIER_SERIAL_THREAD), SPI (spin locks), TSH (pshared
#         attributes), TPS (scheduling, PTHREAD_SCOPE_*), TPP / TPI
#         (priority protocols), TSA / TSS (stack attributes), CS (condattr
#         clock), TMO (timed locks), TCT (getcpuclockid), XSI (mutex types,
#         guardsize, get/setconcurrency); TSA OB: get/setstackaddr
#   2008  THR, BAR, SPI, CS, TMO and the XSI mutex types and guardsize move
#         to the base; + PTHREAD_MUTEX_ROBUST, PTHREAD_MUTEX_STALLED,
#         PTHREAD_RWLOCK_INITIALIZER, pthread_mutex_consistent,
#         pthread_mutexattr_get/setrobust; PTHREAD_EXPLICIT_SCHED and
#         PTHREAD_INHERIT_SCHED become TPS; RPI / RPP join TPI / TPP (the
#         protocol attribute is MC1, which this edition defines as TPP, TPI,
#         RPP or RPI); OB XSI: get/setconcurrency; get/setstackaddr removed
#   2017  pthread_cleanup_push and pthread_cleanup_pop: functions, macros
#         or both
#   2024  + PTHREAD_NULL, pthread_cond_clockwait, pthread_mutex_clocklock,
#         pthread_rwlock_clockrdlock, pthread_rwlock_clockwrlock;
#         pthread_exit is _Noreturn; OB: pthread_atfork; get/setconcurrency
#         removed
# Every edition: <pthread.h> makes the symbols of <sched.h> and <time.h>
# visible (their own specs; struct sched_param, struct timespec and
# clockid_t are not listed here).
#
# One option per line (make options). The THR lines are base=POSIX.1-2008:
# without THR, POSIX.1-2001 requires nothing of the header. Where the text
# names two options, the line keeps the one that is never stricter than
# the text when only it is selected, and says so.
#
# Host deviations (glibc 2.44):
#   - undeclared: PTHREAD_MUTEX_DEFAULT, _ERRORCHECK, _NORMAL, _RECURSIVE,
#     _ROBUST, _STALLED and PTHREAD_PRIO_INHERIT, _NONE, _PROTECT:
#     enumerators only, never #defined (a symbolic constant is an
#     object-like macro, XBD 3.363; 3.372 in 2024);
#   - undeclared: PTHREAD_NULL (2024), not defined;
#   - undeclared: pthread_cond_clockwait, pthread_mutex_clocklock,
#     pthread_rwlock_clockrdlock, pthread_rwlock_clockwrlock (2024): only
#     with _GNU_SOURCE, hidden in strict and lax modes;
#   - pthread_mutex_trylock on a robust mutex made not recoverable:
#     ENOTRECOVERABLE, but the mutex stays taken (another thread's trylock
#     gives EBUSY, its lock blocks), whereas every lock "shall fail with
#     ENOTRECOVERABLE" (XSH pthread_mutexattr_getrobust()).

header pthread.h

# Reserved: XSH 2.2.2 (pthread_ and PTHREAD_ for <pthread.h>; *_t for any
# header).
reserved POSIX.1-2001  *_t pthread_* PTHREAD_*

# ----- Types (as described in <sys/types.h>) -----
# name                          since          tags
pthread_attr_t                  POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_barrier_t               POSIX.1-2001   type BAR base=POSIX.1-2008 same=sys/types.h
pthread_barrierattr_t           POSIX.1-2001   type BAR base=POSIX.1-2008 same=sys/types.h
pthread_cond_t                  POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_condattr_t              POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_key_t                   POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_mutex_t                 POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_mutexattr_t             POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_once_t                  POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_rwlock_t                POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_rwlockattr_t            POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h
pthread_spinlock_t              POSIX.1-2001   type SPI base=POSIX.1-2008 same=sys/types.h
pthread_t                       POSIX.1-2001   type THR base=POSIX.1-2008 same=sys/types.h

# ----- Symbolic constants -----
PTHREAD_BARRIER_SERIAL_THREAD   POSIX.1-2001   macro BAR base=POSIX.1-2008
PTHREAD_CANCEL_ASYNCHRONOUS     POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_CANCEL_ENABLE           POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_CANCEL_DEFERRED         POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_CANCEL_DISABLE          POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_CANCELED                POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_CREATE_DETACHED         POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_CREATE_JOINABLE         POSIX.1-2001   macro THR base=POSIX.1-2008
# EXPLICIT / INHERIT: THR in 2001, TPS from 2008. TPS throughout (2001
# without TPS does not require them: too lax there).
PTHREAD_EXPLICIT_SCHED          POSIX.1-2001   macro TPS
PTHREAD_INHERIT_SCHED           POSIX.1-2001   macro TPS
# Mutex types: XSI in 2001, base from 2008.
PTHREAD_MUTEX_DEFAULT           POSIX.1-2001   macro XSI base=POSIX.1-2008
PTHREAD_MUTEX_ERRORCHECK        POSIX.1-2001   macro XSI base=POSIX.1-2008
PTHREAD_MUTEX_NORMAL            POSIX.1-2001   macro XSI base=POSIX.1-2008
PTHREAD_MUTEX_RECURSIVE         POSIX.1-2001   macro XSI base=POSIX.1-2008
PTHREAD_MUTEX_ROBUST            POSIX.1-2008   macro
PTHREAD_MUTEX_STALLED           POSIX.1-2008   macro
PTHREAD_ONCE_INIT               POSIX.1-2001   macro THR base=POSIX.1-2008
# Priority protocols. INHERIT: TPI, then RPI or TPI (2008). NONE: TPP or
# TPI, then MC1 (2008: TPP, TPI, RPP or RPI). PROTECT: TPP, then RPP or
# TPP (2008). One option each: with only the other option selected, they
# are not required (too lax there).
PTHREAD_PRIO_INHERIT            POSIX.1-2001   macro TPI
PTHREAD_PRIO_NONE               POSIX.1-2001   macro TPP
PTHREAD_PRIO_PROTECT            POSIX.1-2001   macro TPP
PTHREAD_PROCESS_SHARED          POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_PROCESS_PRIVATE         POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_SCOPE_PROCESS           POSIX.1-2001   macro TPS
PTHREAD_SCOPE_SYSTEM            POSIX.1-2001   macro TPS

# ----- Initializers (compile-time constant expressions) -----
PTHREAD_COND_INITIALIZER        POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_MUTEX_INITIALIZER       POSIX.1-2001   macro THR base=POSIX.1-2008
PTHREAD_RWLOCK_INITIALIZER      POSIX.1-2008   macro
# Never equal to the thread ID of an existing thread (2024).
PTHREAD_NULL                    POSIX.1-2024   macro

# ----- Fork handlers (OB in 2024) -----
pthread_atfork                  POSIX.1-2001   THR base=POSIX.1-2008 ob=POSIX.1-2024

# ----- Thread attributes -----
pthread_attr_destroy            POSIX.1-2001   THR base=POSIX.1-2008
pthread_attr_init               POSIX.1-2001   THR base=POSIX.1-2008
pthread_attr_getdetachstate     POSIX.1-2001   THR base=POSIX.1-2008
pthread_attr_setdetachstate     POSIX.1-2001   THR base=POSIX.1-2008
pthread_attr_getguardsize       POSIX.1-2001   XSI base=POSIX.1-2008
pthread_attr_setguardsize       POSIX.1-2001   XSI base=POSIX.1-2008
pthread_attr_getinheritsched    POSIX.1-2001   TPS
pthread_attr_setinheritsched    POSIX.1-2001   TPS
pthread_attr_getschedparam      POSIX.1-2001   THR base=POSIX.1-2008
pthread_attr_setschedparam      POSIX.1-2001   THR base=POSIX.1-2008
pthread_attr_getschedpolicy     POSIX.1-2001   TPS
pthread_attr_setschedpolicy     POSIX.1-2001   TPS
pthread_attr_getscope           POSIX.1-2001   TPS
pthread_attr_setscope           POSIX.1-2001   TPS
# get/setstack: TSA and TSS (both). TSA alone here: with TSA but not TSS
# they are required (too strict there).
pthread_attr_getstack           POSIX.1-2001   TSA
pthread_attr_setstack           POSIX.1-2001   TSA
pthread_attr_getstackaddr       POSIX.1-2001   TSA ob=POSIX.1-2001 removed=POSIX.1-2008
pthread_attr_setstackaddr       POSIX.1-2001   TSA ob=POSIX.1-2001 removed=POSIX.1-2008
pthread_attr_getstacksize       POSIX.1-2001   TSS
pthread_attr_setstacksize       POSIX.1-2001   TSS

# ----- Barriers -----
pthread_barrier_destroy         POSIX.1-2001   BAR base=POSIX.1-2008
pthread_barrier_init            POSIX.1-2001   BAR base=POSIX.1-2008
pthread_barrier_wait            POSIX.1-2001   BAR base=POSIX.1-2008
pthread_barrierattr_destroy     POSIX.1-2001   BAR base=POSIX.1-2008
pthread_barrierattr_init        POSIX.1-2001   BAR base=POSIX.1-2008
# pshared: BAR and TSH in 2001, TSH from 2008. TSH throughout (2001 with
# TSH but not BAR requires them: too strict there).
pthread_barrierattr_getpshared  POSIX.1-2001   TSH
pthread_barrierattr_setpshared  POSIX.1-2001   TSH

# ----- Cancellation -----
pthread_cancel                  POSIX.1-2001   THR base=POSIX.1-2008
pthread_setcancelstate          POSIX.1-2001   THR base=POSIX.1-2008
pthread_setcanceltype           POSIX.1-2001   THR base=POSIX.1-2008
pthread_testcancel              POSIX.1-2001   THR base=POSIX.1-2008
# Functions in 2001 and 2008, which XSH allows to be macros (they must be
# used in pairs in one lexical scope); functions, macros or both from 2017.
pthread_cleanup_pop             POSIX.1-2001   either THR base=POSIX.1-2008
pthread_cleanup_push            POSIX.1-2001   either THR base=POSIX.1-2008

# ----- Condition variables -----
pthread_cond_broadcast          POSIX.1-2001   THR base=POSIX.1-2008
pthread_cond_clockwait          POSIX.1-2024
pthread_cond_destroy            POSIX.1-2001   THR base=POSIX.1-2008
pthread_cond_init               POSIX.1-2001   THR base=POSIX.1-2008
pthread_cond_signal             POSIX.1-2001   THR base=POSIX.1-2008
pthread_cond_timedwait          POSIX.1-2001   THR base=POSIX.1-2008
pthread_cond_wait               POSIX.1-2001   THR base=POSIX.1-2008
pthread_condattr_destroy        POSIX.1-2001   THR base=POSIX.1-2008
pthread_condattr_init           POSIX.1-2001   THR base=POSIX.1-2008
pthread_condattr_getclock       POSIX.1-2001   CS base=POSIX.1-2008
pthread_condattr_setclock       POSIX.1-2001   CS base=POSIX.1-2008
pthread_condattr_getpshared     POSIX.1-2001   TSH
pthread_condattr_setpshared     POSIX.1-2001   TSH

# ----- Thread management -----
pthread_create                  POSIX.1-2001   THR base=POSIX.1-2008
pthread_detach                  POSIX.1-2001   THR base=POSIX.1-2008
pthread_equal                   POSIX.1-2001   THR base=POSIX.1-2008
pthread_exit                    POSIX.1-2001   THR base=POSIX.1-2008
pthread_join                    POSIX.1-2001   THR base=POSIX.1-2008
pthread_self                    POSIX.1-2001   THR base=POSIX.1-2008
pthread_getconcurrency          POSIX.1-2001   XSI ob=POSIX.1-2008 removed=POSIX.1-2024
pthread_setconcurrency          POSIX.1-2001   XSI ob=POSIX.1-2008 removed=POSIX.1-2024
pthread_getcpuclockid           POSIX.1-2001   TCT

# ----- Scheduling -----
pthread_getschedparam           POSIX.1-2001   TPS
pthread_setschedparam           POSIX.1-2001   TPS
pthread_setschedprio            POSIX.1-2001   TPS

# ----- Thread-specific data -----
pthread_getspecific             POSIX.1-2001   THR base=POSIX.1-2008
pthread_key_create              POSIX.1-2001   THR base=POSIX.1-2008
pthread_key_delete              POSIX.1-2001   THR base=POSIX.1-2008
pthread_setspecific             POSIX.1-2001   THR base=POSIX.1-2008

# ----- Mutexes -----
pthread_mutex_clocklock         POSIX.1-2024
pthread_mutex_consistent        POSIX.1-2008
pthread_mutex_destroy           POSIX.1-2001   THR base=POSIX.1-2008
pthread_mutex_init              POSIX.1-2001   THR base=POSIX.1-2008
pthread_mutex_lock              POSIX.1-2001   THR base=POSIX.1-2008
pthread_mutex_timedlock         POSIX.1-2001   TMO base=POSIX.1-2008
pthread_mutex_trylock           POSIX.1-2001   THR base=POSIX.1-2008
pthread_mutex_unlock            POSIX.1-2001   THR base=POSIX.1-2008
# Priority ceiling: TPP, then RPP or TPP (2008).
pthread_mutex_getprioceiling    POSIX.1-2001   TPP
pthread_mutex_setprioceiling    POSIX.1-2001   TPP
pthread_mutexattr_destroy       POSIX.1-2001   THR base=POSIX.1-2008
pthread_mutexattr_init          POSIX.1-2001   THR base=POSIX.1-2008
pthread_mutexattr_getprioceiling POSIX.1-2001  TPP
pthread_mutexattr_setprioceiling POSIX.1-2001  TPP
# Protocol: TPP or TPI, then MC1 (2008: TPP, TPI, RPP or RPI). TPP here.
pthread_mutexattr_getprotocol   POSIX.1-2001   TPP
pthread_mutexattr_setprotocol   POSIX.1-2001   TPP
pthread_mutexattr_getpshared    POSIX.1-2001   TSH
pthread_mutexattr_setpshared    POSIX.1-2001   TSH
pthread_mutexattr_getrobust     POSIX.1-2008
pthread_mutexattr_setrobust     POSIX.1-2008
pthread_mutexattr_gettype       POSIX.1-2001   XSI base=POSIX.1-2008
pthread_mutexattr_settype       POSIX.1-2001   XSI base=POSIX.1-2008

# ----- Dynamic package initialization -----
pthread_once                    POSIX.1-2001   THR base=POSIX.1-2008

# ----- Read-write locks -----
pthread_rwlock_clockrdlock      POSIX.1-2024
pthread_rwlock_clockwrlock      POSIX.1-2024
pthread_rwlock_destroy          POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlock_init             POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlock_rdlock           POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlock_timedrdlock      POSIX.1-2001   TMO base=POSIX.1-2008
pthread_rwlock_timedwrlock      POSIX.1-2001   TMO base=POSIX.1-2008
pthread_rwlock_tryrdlock        POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlock_trywrlock        POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlock_unlock           POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlock_wrlock           POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlockattr_destroy      POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlockattr_init         POSIX.1-2001   THR base=POSIX.1-2008
pthread_rwlockattr_getpshared   POSIX.1-2001   TSH
pthread_rwlockattr_setpshared   POSIX.1-2001   TSH

# ----- Spin locks -----
pthread_spin_destroy            POSIX.1-2001   SPI base=POSIX.1-2008
pthread_spin_init               POSIX.1-2001   SPI base=POSIX.1-2008
pthread_spin_lock               POSIX.1-2001   SPI base=POSIX.1-2008
pthread_spin_trylock            POSIX.1-2001   SPI base=POSIX.1-2008
pthread_spin_unlock             POSIX.1-2001   SPI base=POSIX.1-2008
