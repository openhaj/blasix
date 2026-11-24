# <ctype.h> - character classification and case mapping.
#
# ISO C: 7.4 (C99, C11, C17; 7.4 in C23 with 7.4.2 classification and
# 7.4.3 case mapping): the same 14 functions in every edition, no macro,
# no type.
# POSIX: XBD <ctype.h>; XSH isalnum() ... toupper(), isascii(),
# toascii(), _tolower(), _toupper().
#   2001  the ISO C functions; XSI: isascii, toascii, macros _toupper,
#         _tolower
#   2008  CX: locale_t "as described in <locale.h>" and the *_l
#         functions; isascii, toascii, _toupper, _tolower become [OB XSI]
#   2017  same as 2008
#   2024  isascii, toascii, _toupper, _tolower are gone

header ctype.h

# Reserved: ISO C 7.26.2 (C99), 7.31.2 (C11, C17), 7.33.2 (C23); XSH 2.2.2.
reserved C99           is[a-z]* to[a-z]*
reserved POSIX.1-2001  *_t

# name          since          tags
isalnum         C99
isalpha         C99
isblank         C99
iscntrl         C99
isdigit         C99
isgraph         C99
islower         C99
isprint         C99
ispunct         C99
isspace         C99
isupper         C99
isxdigit        C99
tolower         C99
toupper         C99

isalnum_l       POSIX.1-2008
isalpha_l       POSIX.1-2008
isblank_l       POSIX.1-2008
iscntrl_l       POSIX.1-2008
isdigit_l       POSIX.1-2008
isgraph_l       POSIX.1-2008
islower_l       POSIX.1-2008
isprint_l       POSIX.1-2008
ispunct_l       POSIX.1-2008
isspace_l       POSIX.1-2008
isupper_l       POSIX.1-2008
isxdigit_l      POSIX.1-2008
tolower_l       POSIX.1-2008
toupper_l       POSIX.1-2008

isascii         POSIX.1-2001   XSI ob=POSIX.1-2008 removed=POSIX.1-2024
toascii         POSIX.1-2001   XSI ob=POSIX.1-2008 removed=POSIX.1-2024
_tolower        POSIX.1-2001   macro XSI ob=POSIX.1-2008 removed=POSIX.1-2024
_toupper        POSIX.1-2001   macro XSI ob=POSIX.1-2008 removed=POSIX.1-2024

# "shall define the locale_t type as described in <locale.h>" (2008 on)
locale_t        POSIX.1-2008   type same=locale.h
