# suites.mk - the test units, listed explicitly (no wildcard).
#
# One block per category (= spec/<category>.spec = src/suites/<category>/):
#   <category>_UNITS   the units, <group>/<function> without .c: a group is
#                      a folder of the category (copy, search, ...), the
#                      unit is still <category>/<function> for lcrun;
#                      defs: the types and constants (one unit, cases of
#                      bits/ run in the context of the header)
#   <category>_HDRS    the helpers shared by these units (category root)
# BITS_HDRS: src/suites/bits/<origin>/, the cases of every type and
# constant (size_t, NULL, locale_t, PRId8, ...), included by the defs.c of
# each header that defines them.
# make new adds the unit here; make sources-check reports any file of
# src/suites/ or spec/ that is not listed.

CATEGORIES := \
	ctype \
	string \
	strings \

# ----- bits (shared by several headers) -----

BITS_HDRS := \
	annex_k/errno_t.h \
	annex_k/lib_ext1.h \
	annex_k/rsize_t.h \
	locale/locale_t.h \
	locale/locale_t_same.h \
	stddef/NULL.h \
	stddef/size_t.h \
	stddef/wchar_t.h \
	stdint/intmax_t.h \
	stdint/uintmax_t.h \
	version/__STDC_VERSION_INTTYPES_H__.h \
	version/__STDC_VERSION_STRING_H__.h \

# ----- ctype -----

ctype_UNITS := \
	defs \
	case/tolower \
	case/toupper \
	classify/isalnum \
	classify/isalpha \
	classify/isblank \
	classify/iscntrl \
	classify/isdigit \
	classify/isgraph \
	classify/islower \
	classify/isprint \
	classify/ispunct \
	classify/isspace \
	classify/isupper \
	classify/isxdigit \
	locale/isalnum_l \
	locale/isalpha_l \
	locale/isblank_l \
	locale/iscntrl_l \
	locale/isdigit_l \
	locale/isgraph_l \
	locale/islower_l \
	locale/isprint_l \
	locale/ispunct_l \
	locale/isspace_l \
	locale/isupper_l \
	locale/isxdigit_l \
	locale/tolower_l \
	locale/toupper_l \
	xsi/_tolower \
	xsi/_toupper \
	xsi/isascii \
	xsi/toascii \

ctype_HDRS := \
	ctype_cases.h \

# ----- string -----

string_UNITS := \
	defs \
	annex_k/memcpy_s \
	annex_k/memmove_s \
	annex_k/memset_s \
	annex_k/strcat_s \
	annex_k/strcpy_s \
	annex_k/strerror_s \
	annex_k/strerrorlen_s \
	annex_k/strncat_s \
	annex_k/strncpy_s \
	annex_k/strnlen_s \
	annex_k/strtok_s \
	compare/memcmp \
	compare/strcmp \
	compare/strcoll \
	compare/strcoll_l \
	compare/strncmp \
	compare/strxfrm \
	compare/strxfrm_l \
	concat/strcat \
	concat/strlcat \
	concat/strncat \
	copy/memccpy \
	copy/memcpy \
	copy/memmove \
	copy/stpcpy \
	copy/stpncpy \
	copy/strcpy \
	copy/strdup \
	copy/strlcpy \
	copy/strncpy \
	copy/strndup \
	misc/memset \
	misc/memset_explicit \
	misc/strerror \
	misc/strerror_l \
	misc/strerror_r \
	misc/strlen \
	misc/strnlen \
	misc/strsignal \
	search/memchr \
	search/memmem \
	search/strchr \
	search/strcspn \
	search/strpbrk \
	search/strrchr \
	search/strspn \
	search/strstr \
	search/strtok \
	search/strtok_r \

string_HDRS := \
	string_cases.h \

# ----- strings -----

strings_UNITS := \
	defs \
	compare/strcasecmp \
	compare/strcasecmp_l \
	compare/strncasecmp \
	compare/strncasecmp_l \
	bitops/ffs \
	bitops/ffsl \
	bitops/ffsll \
	legacy/bcmp \
	legacy/bcopy \
	legacy/bzero \
	legacy/index \
	legacy/rindex \

strings_HDRS := \
	strings_cases.h \
