# suites.mk - the test units, listed explicitly (no wildcard).
#
# One block per category (= spec/<category>.spec = src/suites/<category>/):
#   <category>_UNITS   the units, without .c (one per spec interface)
#   <category>_HDRS    the helpers shared by these units
# make new adds the unit here; make sources-check reports any file of
# src/suites/ or spec/ that is not listed.

CATEGORIES := \
	ctype \

# ----- ctype -----

ctype_UNITS := \
	_tolower \
	_toupper \
	isalnum \
	isalnum_l \
	isalpha \
	isalpha_l \
	isascii \
	isblank \
	isblank_l \
	iscntrl \
	iscntrl_l \
	isdigit \
	isdigit_l \
	isgraph \
	isgraph_l \
	islower \
	islower_l \
	isprint \
	isprint_l \
	ispunct \
	ispunct_l \
	isspace \
	isspace_l \
	isupper \
	isupper_l \
	isxdigit \
	isxdigit_l \
	locale_t \
	toascii \
	tolower \
	tolower_l \
	toupper \
	toupper_l \

ctype_HDRS := \
	ctype_cases.h \
