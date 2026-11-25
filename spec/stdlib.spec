# <stdlib.h> - general utilities.
#
# ISO C: 7.20 (C99), 7.22 (C11, C17), 7.24 (C23); Annex K.3.6 (C11 on,
# bounds-checking interfaces, only with __STDC_WANT_LIB_EXT1__ = 1).
#   C99   size_t, wchar_t, div_t, ldiv_t, lldiv_t, NULL, EXIT_FAILURE,
#         EXIT_SUCCESS, RAND_MAX, MB_CUR_MAX and the 34 functions atof ...
#         wcstombs
#   C11   + aligned_alloc, at_quick_exit, quick_exit; Annex K (LIB_EXT1):
#         errno_t, rsize_t, constraint_handler_t and 9 functions; realloc
#         with a size of zero becomes obsolescent (7.31.12p2)
#   C17   same as C11
#   C23   + strfromd, strfromf, strfroml, free_sized, free_aligned_sized,
#         memalignment, the macro __STDC_VERSION_STDLIB_H__, and once_flag,
#         ONCE_FLAG_INIT, call_once "described in 7.28" (also in
#         <threads.h>); strtodN and strfromdN with decimal floating point
#         (DFP, 7.24.1.4, 7.24.1.6); bsearch becomes generic in the const
#         qualification (7.24.5.1); realloc with a size of zero is
#         undefined (7.24.3.7p3)
# POSIX: XBD <stdlib.h>; XSH _Exit() ... wctomb().
#   2001  the ISO C (C99) interfaces; CX: setenv, unsetenv; XSI: a64l,
#         drand48 ... unlockpt, getsubopt, mkstemp, realpath, and the
#         <sys/wait.h> macros WNOHANG ... WTERMSIG; XSI LEGACY: ecvt,
#         fcvt, gcvt, mktemp; TSF: rand_r; ADV: posix_memalign
#   2008  + mkdtemp (CX); getsubopt, mkstemp, rand_r and the W* macros
#         move to the base (CX), rand_r obsolescent; the LEGACY ecvt,
#         fcvt, gcvt, mktemp are removed
#   2017  same interfaces as 2008; MB_CUR_MAX is 1 in the POSIX locale
#   2024  built on C17: + aligned_alloc, at_quick_exit, quick_exit;
#         + mkostemp, qsort_r, reallocarray, secure_getenv, WCOREDUMP (CX),
#         ptsname_r (XSI), the <fcntl.h> constants O_APPEND, O_CLOEXEC,
#         O_CLOFORK, O_SYNC (CX), O_DSYNC, O_RSYNC (SIO), O_NOCTTY, O_RDWR
#         (XSI); realpath moves to the base; rand_r is removed; setkey
#         becomes obsolescent; EXIT_FAILURE is between 1 and 125
# posix_memalign stays ADV in every edition (OPTS=XSI,ADV to require it);
# posix_* is reserved for POSIX (XSH 2.2.2), so it may stay visible.
#
# NULL has no same=stddef.h (see spec/string.spec). once_flag,
# ONCE_FLAG_INIT and call_once have no same=threads.h: <threads.h> is
# optional (__STDC_NO_THREADS__), <stdlib.h> must declare them anyway. The
# W* macros that take a status are not integer constants: no same= (the
# probe compares values), their cases are shared in bits/wait/.
#
# Host deviations (glibc 2.44):
#   - POSIX.1-2024 additions hidden under _POSIX_C_SOURCE=202405L (XSH
#     2.2.1): mkostemp, ptsname_r, qsort_r, secure_getenv only with
#     _GNU_SOURCE, reallocarray with _DEFAULT_SOURCE (tested with LAX=1);
#     WCOREDUMP and the O_* constants are not in <stdlib.h> at all;
#   - setkey is not declared at all (removed from the headers in 2.28);
#   - rand_r is visible whatever the options: a leak when TSF is off
#     (POSIX.1-2001) and in POSIX.1-2024, where it is removed (XSH 2.2.1);
#   - no Annex K: with OPTS=LIB_EXT1 its interfaces are undeclared and the
#     K.3.1.1p4 reject probes are accepted.
#   - realloc(p, 0) frees p and returns a null pointer without setting
#     errno to EINVAL (XSH realloc() RETURN VALUE, POSIX.1-2024 [OB]):
#     stdlib/realloc/zero_size_is_null_with_einval_or_a_new_object fails.

header stdlib.h

# Reserved: ISO C 7.26.10 (C99), 7.31.12 (C11, C17), 7.33.16 (C23); XSH
# 2.2.2 (str[a-z] for <stdlib.h>; posix_ and POSIX_ for any header). Annex
# K follows K.3.1.1p1 instead (want macro 0: all hidden).
reserved C99           str[a-z]*
reserved C23           wcs[a-z]*
reserved POSIX.1-2001  *_t posix_* POSIX_*

# ----- Types and macros (7.24p3-4) -----
# name                      since          tags
size_t                      C99            type same=stddef.h
wchar_t                     C99            type same=stddef.h
div_t                       C99            type
ldiv_t                      C99            type
lldiv_t                     C99            type
NULL                        C99            macro
EXIT_FAILURE                C99            macro
EXIT_SUCCESS                C99            macro
RAND_MAX                    C99            macro
MB_CUR_MAX                  C99            macro
__STDC_VERSION_STDLIB_H__   C23            macro
once_flag                   C23            type
ONCE_FLAG_INIT              C23            macro
call_once                   C23

# ----- Numeric conversion (7.24.1) -----
atof                        C99
atoi                        C99
atol                        C99
atoll                       C99
strtod                      C99
strtof                      C99
strtold                     C99
strtol                      C99
strtoll                     C99
strtoul                     C99
strtoull                    C99
strfromd                    C23
strfromf                    C23
strfroml                    C23
# Decimal floating point (7.24.1.4, 7.24.1.6): __STDC_IEC_60559_DFP__.
strfromd32                  C23            DFP
strfromd64                  C23            DFP
strfromd128                 C23            DFP
strtod32                    C23            DFP
strtod64                    C23            DFP
strtod128                   C23            DFP

# ----- Pseudo-random sequences (7.24.2) -----
rand                        C99
srand                       C99

# ----- Memory management (7.24.3), alignment (7.24.9) -----
calloc                      C99
free                        C99
malloc                      C99
realloc                     C99
aligned_alloc               C11
free_sized                  C23
free_aligned_sized          C23
memalignment                C23

# ----- Communication with the environment (7.24.4) -----
abort                       C99
atexit                      C99
exit                        C99
_Exit                       C99
getenv                      C99
system                      C99
at_quick_exit               C11
quick_exit                  C11

# ----- Searching and sorting (7.24.5) -----
bsearch                     C99
qsort                       C99

# ----- Integer arithmetic (7.24.6) -----
abs                         C99
labs                        C99
llabs                       C99
div                         C99
ldiv                        C99
lldiv                       C99

# ----- Multibyte characters and strings (7.24.7, 7.24.8) -----
mblen                       C99
mbtowc                      C99
wctomb                      C99
mbstowcs                    C99
wcstombs                    C99

# ----- POSIX: environment, files, memory (CX) -----
setenv                      POSIX.1-2001
unsetenv                    POSIX.1-2001
getsubopt                   POSIX.1-2001   XSI base=POSIX.1-2008
mkstemp                     POSIX.1-2001   XSI base=POSIX.1-2008
realpath                    POSIX.1-2001   XSI base=POSIX.1-2024
mkdtemp                     POSIX.1-2008
mkostemp                    POSIX.1-2024
qsort_r                     POSIX.1-2024
reallocarray                POSIX.1-2024
secure_getenv               POSIX.1-2024
# Advisory Information in every edition (XBD <stdlib.h>, [ADV]).
posix_memalign              POSIX.1-2001   ADV
# Thread-Safe Functions, base and obsolescent in 2008, removed in 2024
# (Austin Group Defect 1330).
rand_r                      POSIX.1-2001   TSF base=POSIX.1-2008 ob=POSIX.1-2008 removed=POSIX.1-2024

# ----- POSIX: XSI -----
a64l                        POSIX.1-2001   XSI
l64a                        POSIX.1-2001   XSI
drand48                     POSIX.1-2001   XSI
erand48                     POSIX.1-2001   XSI
jrand48                     POSIX.1-2001   XSI
lcong48                     POSIX.1-2001   XSI
lrand48                     POSIX.1-2001   XSI
mrand48                     POSIX.1-2001   XSI
nrand48                     POSIX.1-2001   XSI
seed48                      POSIX.1-2001   XSI
srand48                     POSIX.1-2001   XSI
initstate                   POSIX.1-2001   XSI
random                      POSIX.1-2001   XSI
setstate                    POSIX.1-2001   XSI
srandom                     POSIX.1-2001   XSI
grantpt                     POSIX.1-2001   XSI
posix_openpt                POSIX.1-2001   XSI
ptsname                     POSIX.1-2001   XSI
unlockpt                    POSIX.1-2001   XSI
ptsname_r                   POSIX.1-2024   XSI
putenv                      POSIX.1-2001   XSI
# Obsolescent in 2024 (Austin Group Defect 1192).
setkey                      POSIX.1-2001   XSI ob=POSIX.1-2024
# LEGACY in 2001, removed in 2008 (XBD <stdlib.h>, Issue 7).
ecvt                        POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
fcvt                        POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
gcvt                        POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
mktemp                      POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008

# ----- POSIX: as described in <sys/wait.h> (XSI in 2001, CX from 2008) -----
WNOHANG                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/wait.h
WUNTRACED                   POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/wait.h
WEXITSTATUS                 POSIX.1-2001   macro XSI base=POSIX.1-2008
WIFEXITED                   POSIX.1-2001   macro XSI base=POSIX.1-2008
WIFSIGNALED                 POSIX.1-2001   macro XSI base=POSIX.1-2008
WIFSTOPPED                  POSIX.1-2001   macro XSI base=POSIX.1-2008
WSTOPSIG                    POSIX.1-2001   macro XSI base=POSIX.1-2008
WTERMSIG                    POSIX.1-2001   macro XSI base=POSIX.1-2008
# Austin Group Defect 1141.
WCOREDUMP                   POSIX.1-2024   macro

# ----- POSIX.1-2024: as described in <fcntl.h> (Defects 593, 1350) -----
O_APPEND                    POSIX.1-2024   macro same=fcntl.h
O_CLOEXEC                   POSIX.1-2024   macro same=fcntl.h
O_CLOFORK                   POSIX.1-2024   macro same=fcntl.h
O_SYNC                      POSIX.1-2024   macro same=fcntl.h
O_DSYNC                     POSIX.1-2024   macro SIO same=fcntl.h
O_RSYNC                     POSIX.1-2024   macro SIO same=fcntl.h
O_NOCTTY                    POSIX.1-2024   macro XSI same=fcntl.h
O_RDWR                      POSIX.1-2024   macro XSI same=fcntl.h

# ----- Annex K (K.3.6): errno_t is int (also in <errno.h>), rsize_t is
# size_t (also in <stddef.h>) -----
errno_t                     C11            type LIB_EXT1 same=errno.h
rsize_t                     C11            type LIB_EXT1 same=stddef.h
constraint_handler_t        C11            type LIB_EXT1
set_constraint_handler_s    C11            LIB_EXT1
abort_handler_s             C11            LIB_EXT1
ignore_handler_s            C11            LIB_EXT1
getenv_s                    C11            LIB_EXT1
bsearch_s                   C11            LIB_EXT1
qsort_s                     C11            LIB_EXT1
wctomb_s                    C11            LIB_EXT1
mbstowcs_s                  C11            LIB_EXT1
wcstombs_s                  C11            LIB_EXT1
