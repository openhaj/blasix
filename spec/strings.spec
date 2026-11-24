# <strings.h> - string operations (POSIX only, no ISO C counterpart).
#
# POSIX: XBD <strings.h>; XSH strcasecmp(), ffs(), ffsl() and, in
# POSIX.1-2001, bcmp(), bcopy(), bzero(), index(), rindex().
#   2001  the whole header is XSI: ffs, strcasecmp, strncasecmp, size_t;
#         bcmp, bcopy, bzero, index, rindex are LEGACY
#   2008  the LEGACY functions are gone; strcasecmp, strncasecmp move to
#         the base, with the new strcasecmp_l, strncasecmp_l and locale_t
#         (CX); ffs stays XSI; size_t "as described in <sys/types.h>"
#   2017  same interfaces; the POSIX-locale rule (compare as lowercase)
#         now covers the *_l functions too
#   2024  + ffsl, ffsll (XSI); errno unchanged on valid input
#
# Host deviations (glibc 2.44): ffsl, ffsll are only declared with
# _DEFAULT_SOURCE, not with _XOPEN_SOURCE=800 (XSH 2.2.1): undeclared in
# strict mode, tested with LAX=1.

header strings.h

# Reserved: XSH 2.2.2 only ("_t" in any header): not an ISO C header (on
# an ISO C target, or POSIX.1-2001 without XSI, nothing is checked).
reserved POSIX.1-2001  *_t

# name          since          tags
size_t          POSIX.1-2001   type XSI base=POSIX.1-2008 same=stddef.h,sys/types.h
locale_t        POSIX.1-2008   type same=locale.h

ffs             POSIX.1-2001   XSI
ffsl            POSIX.1-2024   XSI
ffsll           POSIX.1-2024   XSI

strcasecmp      POSIX.1-2001   XSI base=POSIX.1-2008
strncasecmp     POSIX.1-2001   XSI base=POSIX.1-2008
strcasecmp_l    POSIX.1-2008
strncasecmp_l   POSIX.1-2008

# LEGACY in POSIX.1-2001 (XSI), removed in POSIX.1-2008.
bcmp            POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
bcopy           POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
bzero           POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
index           POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
rindex          POSIX.1-2001   XSI ob=POSIX.1-2001 removed=POSIX.1-2008
