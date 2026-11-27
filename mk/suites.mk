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
	inttypes \
	malloc \
	stdlib \
	string \
	strings \

# ----- bits (shared by several headers) -----

BITS_HDRS := \
	annex_k/constraint_handler_t.h \
	annex_k/errno_t.h \
	annex_k/lib_ext1.h \
	annex_k/rsize_t.h \
	fcntl/open_flags.h \
	inttypes/convert.h \
	inttypes/format.h \
	inttypes/imaxdiv_t.h \
	inttypes/pri.h \
	inttypes/scn.h \
	inttypes/wide.h \
	locale/locale_t.h \
	locale/locale_t_same.h \
	stddef/NULL.h \
	stddef/size_t.h \
	stddef/wchar_t.h \
	stdint/intmax_t.h \
	stdint/uintmax_t.h \
	stdlib/EXIT_FAILURE.h \
	stdlib/EXIT_SUCCESS.h \
	stdlib/MB_CUR_MAX.h \
	stdlib/RAND_MAX.h \
	stdlib/div_t.h \
	stdlib/ldiv_t.h \
	stdlib/lldiv_t.h \
	threads/ONCE_FLAG_INIT.h \
	threads/once_flag.h \
	version/__STDC_VERSION_INTTYPES_H__.h \
	version/__STDC_VERSION_STDLIB_H__.h \
	version/__STDC_VERSION_STRING_H__.h \
	wait/options.h \
	wait/status.h \

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

# ----- inttypes -----

inttypes_UNITS := \
	defs \
	func/imaxabs \
	func/imaxdiv \
	func/strtoimax \
	func/strtoumax \
	func/wcstoimax \
	func/wcstoumax \

inttypes_HDRS := \
	inttypes_cases.h \

# ----- stdlib -----

stdlib_UNITS := \
	convert/atoi \
	convert/atol \
	convert/atoll \
	convert/strtol \
	convert/strtoll \
	convert/strtoul \
	convert/strtoull \
	defs \
	memory/aligned_alloc \
	memory/calloc \
	memory/free \
	memory/free_aligned_sized \
	memory/free_sized \
	memory/malloc \
	memory/posix_memalign \
	memory/realloc \
	memory/reallocarr \
	memory/reallocarray \
	memory/reallocf \
	search/qsort \

stdlib_HDRS := \
	ato_int.h \
	stdlib_cases.h \
	stdlib_procs.h \
	strto_int.h \

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
	search/memrchr \
	search/strcasestr \
	search/strchr \
	search/strchrnul \
	search/strcspn \
	search/strpbrk \
	search/strrchr \
	search/strsep \
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

# ----- malloc -----

malloc_UNITS := \
	aligned/memalign \
	aligned/pvalloc \
	aligned/valloc \
	legacy/cfree \
	query/malloc_good_size \
	query/malloc_size \
	query/malloc_usable_size \

malloc_HDRS := \
	malloc_cases.h \
