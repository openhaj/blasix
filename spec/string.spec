# <string.h> - string handling.
#
# ISO C: 7.21 (C99), 7.24 (C11, C17), 7.26 (C23); Annex K.3.7 (C11 on,
# bounds-checking interfaces, only with __STDC_WANT_LIB_EXT1__ = 1).
#   C99   size_t, NULL and the 22 functions memcpy ... strlen
#   C11   + Annex K (LIB_EXT1): errno_t, rsize_t and 11 *_s functions
#   C17   same as C11
#   C23   + memccpy, strdup, strndup, memset_explicit and the macro
#         __STDC_VERSION_STRING_H__; memchr, strchr, strpbrk, strrchr and
#         strstr become generic in the const qualification (7.26.5.1)
# POSIX: XBD <string.h>; XSH memccpy() ... strxfrm_l().
#   2001  the ISO C functions; XSI: memccpy, strdup; TSF: strerror_r,
#         strtok_r
#   2008  CX: locale_t "as described in <locale.h>", stpcpy, stpncpy,
#         strcoll_l, strerror_l, strndup, strnlen, strsignal, strxfrm_l;
#         strdup, strerror_r and strtok_r move to the base
#   2017  same interfaces as 2008
#   2024  + memmem, strlcat, strlcpy (CX); memccpy is still XSI
# Annex K.3.1.1p4: __STDC_WANT_LIB_EXT1__ defined differently for two
# inclusions must be refused (reject probes, with OPTS=LIB_EXT1).
#
# NULL has no same=stddef.h: it may expand to a pointer constant, which
# the macro probe (an integer constant expression) cannot compare; its
# cases are shared (src/suites/bits/NULL.h).
#
# Host deviations (glibc 2.44): memmem, strlcat, strlcpy are only
# declared with _DEFAULT_SOURCE, not with _POSIX_C_SOURCE=202405L (XSH
# 2.2.1): undeclared in strict mode, tested with LAX=1.

header string.h

# Reserved: ISO C 7.26.11 (C99), 7.31.13 (C11, C17), 7.33.17 (C23); XSH
# 2.2.2. Annex K follows K.3.1.1p1 instead (want macro 0: all hidden).
reserved C99           str[a-z]* mem[a-z]* wcs[a-z]*
reserved POSIX.1-2001  *_t

# name                      since          tags
NULL                        C99            macro
size_t                      C99            type same=stddef.h

memchr                      C99
memcmp                      C99
memcpy                      C99
memmove                     C99
memset                      C99
strcat                      C99
strchr                      C99
strcmp                      C99
strcoll                     C99
strcpy                      C99
strcspn                     C99
strerror                    C99
strlen                      C99
strncat                     C99
strncmp                     C99
strncpy                     C99
strpbrk                     C99
strrchr                     C99
strspn                      C99
strstr                      C99
strtok                      C99
strxfrm                     C99

# C23 adds them to ISO C; POSIX had them before (other ladder).
memccpy                     C23
memccpy                     POSIX.1-2001   XSI
strdup                      C23
strdup                      POSIX.1-2001   XSI base=POSIX.1-2008
strndup                     C23
strndup                     POSIX.1-2008
memset_explicit             C23
__STDC_VERSION_STRING_H__   C23            macro

strerror_r                  POSIX.1-2001   TSF base=POSIX.1-2008
strtok_r                    POSIX.1-2001   TSF base=POSIX.1-2008

stpcpy                      POSIX.1-2008
stpncpy                     POSIX.1-2008
strcoll_l                   POSIX.1-2008
strerror_l                  POSIX.1-2008
strnlen                     POSIX.1-2008
strsignal                   POSIX.1-2008
strxfrm_l                   POSIX.1-2008
# "shall define the locale_t type as described in <locale.h>" (2008 on)
locale_t                    POSIX.1-2008   type same=locale.h

memmem                      POSIX.1-2024
strlcat                     POSIX.1-2024
strlcpy                     POSIX.1-2024

# Annex K (K.3.7): errno_t is int (also in <errno.h>), rsize_t is
# size_t (also in <stddef.h>).
errno_t                     C11            type LIB_EXT1 same=errno.h
rsize_t                     C11            type LIB_EXT1 same=stddef.h
memcpy_s                    C11            LIB_EXT1
memmove_s                   C11            LIB_EXT1
memset_s                    C11            LIB_EXT1
strcat_s                    C11            LIB_EXT1
strcpy_s                    C11            LIB_EXT1
strerror_s                  C11            LIB_EXT1
strerrorlen_s               C11            LIB_EXT1
strncat_s                   C11            LIB_EXT1
strncpy_s                   C11            LIB_EXT1
strnlen_s                   C11            LIB_EXT1
strtok_s                    C11            LIB_EXT1
