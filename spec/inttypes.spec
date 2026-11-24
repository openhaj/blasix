# <inttypes.h> - format conversion of integer types.
#
# ISO C: 7.8 (C99, C11, C17, C23). The header includes <stdint.h>.
#   C99   imaxdiv_t; imaxabs, imaxdiv, strtoimax, strtoumax, wcstoimax,
#         wcstoumax; the fprintf macros PRI{d,i,o,u,x,X}<T> and the
#         fscanf macros SCN{d,i,o,u,x}<T>, <T> one of N, LEASTN, FASTN
#         (N = 8, 16, 32, 64), MAX, PTR
#   C11   same as C99
#   C17   same as C11
#   C23   + __STDC_VERSION_INTTYPES_H__, PRIb<T>, SCNb<T>; PRIB<T> only
#         if the implementation supports %B (checked from the PRIb units
#         when defined); strtoimax ... accept the 0b prefix (base 0, 2)
# POSIX: XBD <inttypes.h>; XSH imaxabs() ... wcstoimax().
#   2001  the ISO C content (C99 base)
#   2008  same
#   2017  + wchar_t (CX, as described in <stddef.h>); an unsupported base
#         "shall" fail with EINVAL (was "may")
#   2024  same as 2017
#
# The macros of a type exist "for each type that the implementation
# provides in <stdint.h>". The exact-width N types (and intptr_t) are
# optional in ISO C; POSIX requires int8 ... int32 (int64 where the
# width exists) and XSI intptr_t. They are listed as required here:
# every supported target has them.
#
# Host deviations (glibc 2.44): <inttypes.h> does not define wchar_t
# (an internal __gwchar_t on purpose), against XBD POSIX.1-2017 and
# later: the wchar_t unit is unavailable, LAX=1 included.

header inttypes.h

# Reserved: ISO C 7.26.4 (C99), 7.31.5 (C11, C17), 7.33.6 (C23), and
# those of <stdint.h>, which it includes (7.26.8, 7.31.10, 7.33.14);
# XSH 2.2.2: PRI[Xa-z], SCN[Xa-z], "_t".
reserved C99           PRI[a-zX]* SCN[a-zX]* int*_t uint*_t
reserved C99           INT*_MAX INT*_MIN INT*_C UINT*_MAX UINT*_C
reserved C23           PRIB* SCNB* INT*_WIDTH UINT*_WIDTH str[a-z]* wcs[a-z]*
reserved POSIX.1-2001  *_t

# name                          since          tags
imaxdiv_t                       C99            type
# Used by every prototype of the header: visible through <stdint.h>.
intmax_t                        C99            type same=stdint.h
uintmax_t                       C99            type same=stdint.h
wchar_t                         POSIX.1-2017   type same=stddef.h
__STDC_VERSION_INTTYPES_H__     C23            macro

imaxabs                         C99
imaxdiv                         C99
strtoimax                       C99
strtoumax                       C99
wcstoimax                       C99
wcstoumax                       C99

# fprintf macros (7.8.1p2-3).
PRId8                           C99            macro
PRId16                          C99            macro
PRId32                          C99            macro
PRId64                          C99            macro
PRIdLEAST8                      C99            macro
PRIdLEAST16                     C99            macro
PRIdLEAST32                     C99            macro
PRIdLEAST64                     C99            macro
PRIdFAST8                       C99            macro
PRIdFAST16                      C99            macro
PRIdFAST32                      C99            macro
PRIdFAST64                      C99            macro
PRIdMAX                         C99            macro
PRIdPTR                         C99            macro
PRIi8                           C99            macro
PRIi16                          C99            macro
PRIi32                          C99            macro
PRIi64                          C99            macro
PRIiLEAST8                      C99            macro
PRIiLEAST16                     C99            macro
PRIiLEAST32                     C99            macro
PRIiLEAST64                     C99            macro
PRIiFAST8                       C99            macro
PRIiFAST16                      C99            macro
PRIiFAST32                      C99            macro
PRIiFAST64                      C99            macro
PRIiMAX                         C99            macro
PRIiPTR                         C99            macro
PRIo8                           C99            macro
PRIo16                          C99            macro
PRIo32                          C99            macro
PRIo64                          C99            macro
PRIoLEAST8                      C99            macro
PRIoLEAST16                     C99            macro
PRIoLEAST32                     C99            macro
PRIoLEAST64                     C99            macro
PRIoFAST8                       C99            macro
PRIoFAST16                      C99            macro
PRIoFAST32                      C99            macro
PRIoFAST64                      C99            macro
PRIoMAX                         C99            macro
PRIoPTR                         C99            macro
PRIu8                           C99            macro
PRIu16                          C99            macro
PRIu32                          C99            macro
PRIu64                          C99            macro
PRIuLEAST8                      C99            macro
PRIuLEAST16                     C99            macro
PRIuLEAST32                     C99            macro
PRIuLEAST64                     C99            macro
PRIuFAST8                       C99            macro
PRIuFAST16                      C99            macro
PRIuFAST32                      C99            macro
PRIuFAST64                      C99            macro
PRIuMAX                         C99            macro
PRIuPTR                         C99            macro
PRIx8                           C99            macro
PRIx16                          C99            macro
PRIx32                          C99            macro
PRIx64                          C99            macro
PRIxLEAST8                      C99            macro
PRIxLEAST16                     C99            macro
PRIxLEAST32                     C99            macro
PRIxLEAST64                     C99            macro
PRIxFAST8                       C99            macro
PRIxFAST16                      C99            macro
PRIxFAST32                      C99            macro
PRIxFAST64                      C99            macro
PRIxMAX                         C99            macro
PRIxPTR                         C99            macro
PRIX8                           C99            macro
PRIX16                          C99            macro
PRIX32                          C99            macro
PRIX64                          C99            macro
PRIXLEAST8                      C99            macro
PRIXLEAST16                     C99            macro
PRIXLEAST32                     C99            macro
PRIXLEAST64                     C99            macro
PRIXFAST8                       C99            macro
PRIXFAST16                      C99            macro
PRIXFAST32                      C99            macro
PRIXFAST64                      C99            macro
PRIXMAX                         C99            macro
PRIXPTR                         C99            macro

# fscanf macros (7.8.1p4-5).
SCNd8                           C99            macro
SCNd16                          C99            macro
SCNd32                          C99            macro
SCNd64                          C99            macro
SCNdLEAST8                      C99            macro
SCNdLEAST16                     C99            macro
SCNdLEAST32                     C99            macro
SCNdLEAST64                     C99            macro
SCNdFAST8                       C99            macro
SCNdFAST16                      C99            macro
SCNdFAST32                      C99            macro
SCNdFAST64                      C99            macro
SCNdMAX                         C99            macro
SCNdPTR                         C99            macro
SCNi8                           C99            macro
SCNi16                          C99            macro
SCNi32                          C99            macro
SCNi64                          C99            macro
SCNiLEAST8                      C99            macro
SCNiLEAST16                     C99            macro
SCNiLEAST32                     C99            macro
SCNiLEAST64                     C99            macro
SCNiFAST8                       C99            macro
SCNiFAST16                      C99            macro
SCNiFAST32                      C99            macro
SCNiFAST64                      C99            macro
SCNiMAX                         C99            macro
SCNiPTR                         C99            macro
SCNo8                           C99            macro
SCNo16                          C99            macro
SCNo32                          C99            macro
SCNo64                          C99            macro
SCNoLEAST8                      C99            macro
SCNoLEAST16                     C99            macro
SCNoLEAST32                     C99            macro
SCNoLEAST64                     C99            macro
SCNoFAST8                       C99            macro
SCNoFAST16                      C99            macro
SCNoFAST32                      C99            macro
SCNoFAST64                      C99            macro
SCNoMAX                         C99            macro
SCNoPTR                         C99            macro
SCNu8                           C99            macro
SCNu16                          C99            macro
SCNu32                          C99            macro
SCNu64                          C99            macro
SCNuLEAST8                      C99            macro
SCNuLEAST16                     C99            macro
SCNuLEAST32                     C99            macro
SCNuLEAST64                     C99            macro
SCNuFAST8                       C99            macro
SCNuFAST16                      C99            macro
SCNuFAST32                      C99            macro
SCNuFAST64                      C99            macro
SCNuMAX                         C99            macro
SCNuPTR                         C99            macro
SCNx8                           C99            macro
SCNx16                          C99            macro
SCNx32                          C99            macro
SCNx64                          C99            macro
SCNxLEAST8                      C99            macro
SCNxLEAST16                     C99            macro
SCNxLEAST32                     C99            macro
SCNxLEAST64                     C99            macro
SCNxFAST8                       C99            macro
SCNxFAST16                      C99            macro
SCNxFAST32                      C99            macro
SCNxFAST64                      C99            macro
SCNxMAX                         C99            macro
SCNxPTR                         C99            macro

# C23 binary conversions (7.8.1p3, p6).
PRIb8                           C23            macro
PRIb16                          C23            macro
PRIb32                          C23            macro
PRIb64                          C23            macro
PRIbLEAST8                      C23            macro
PRIbLEAST16                     C23            macro
PRIbLEAST32                     C23            macro
PRIbLEAST64                     C23            macro
PRIbFAST8                       C23            macro
PRIbFAST16                      C23            macro
PRIbFAST32                      C23            macro
PRIbFAST64                      C23            macro
PRIbMAX                         C23            macro
PRIbPTR                         C23            macro
SCNb8                           C23            macro
SCNb16                          C23            macro
SCNb32                          C23            macro
SCNb64                          C23            macro
SCNbLEAST8                      C23            macro
SCNbLEAST16                     C23            macro
SCNbLEAST32                     C23            macro
SCNbLEAST64                     C23            macro
SCNbFAST8                       C23            macro
SCNbFAST16                      C23            macro
SCNbFAST32                      C23            macro
SCNbFAST64                      C23            macro
SCNbMAX                         C23            macro
SCNbPTR                         C23            macro
