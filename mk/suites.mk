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
	fcntl \
	inttypes \
	malloc \
	pthread \
	stdlib \
	string \
	strings \
	sys_stat \
	unistd \

# ----- bits (shared by several headers) -----

BITS_HDRS := \
	annex_k/constraint_handler_t.h \
	annex_k/errno_t.h \
	annex_k/lib_ext1.h \
	annex_k/rsize_t.h \
	fcntl/at.h \
	fcntl/commands.h \
	fcntl/fadv.h \
	fcntl/fd_flags.h \
	fcntl/locks.h \
	fcntl/open_flags.h \
	fcntl/owner.h \
	inttypes/convert.h \
	inttypes/format.h \
	inttypes/imaxdiv_t.h \
	inttypes/pri.h \
	inttypes/scn.h \
	inttypes/wide.h \
	locale/locale_t.h \
	locale/locale_t_same.h \
	pthread/attr.h \
	pthread/barrier.h \
	pthread/cancel.h \
	pthread/initializers.h \
	pthread/mutex.h \
	pthread/prio.h \
	pthread/process.h \
	stddef/NULL.h \
	stddef/size_t.h \
	stddef/wchar_t.h \
	stdint/intmax_t.h \
	stdint/intptr_t.h \
	stdint/uintmax_t.h \
	stdio/seek.h \
	stdlib/EXIT_FAILURE.h \
	stdlib/EXIT_SUCCESS.h \
	stdlib/MB_CUR_MAX.h \
	stdlib/RAND_MAX.h \
	stdlib/div_t.h \
	stdlib/ldiv_t.h \
	stdlib/lldiv_t.h \
	sys_stat/file_types.h \
	sys_stat/mode_bits.h \
	sys_types/blkcnt_t.h \
	sys_types/blksize_t.h \
	sys_types/dev_t.h \
	sys_types/gid_t.h \
	sys_types/ino_t.h \
	sys_types/mode_t.h \
	sys_types/nlink_t.h \
	sys_types/off_t.h \
	sys_types/pid_t.h \
	sys_types/pthread_types.h \
	sys_types/ssize_t.h \
	sys_types/time_t.h \
	sys_types/uid_t.h \
	sys_types/useconds_t.h \
	threads/ONCE_FLAG_INIT.h \
	threads/once_flag.h \
	unistd/access.h \
	unistd/confstr.h \
	unistd/fileno.h \
	unistd/lockf.h \
	unistd/misc.h \
	unistd/options.h \
	unistd/pathconf.h \
	unistd/sysconf.h \
	unistd/version.h \
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

# ----- pthread -----

pthread_UNITS := \
	defs \

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
	env/getenv \
	env/putenv \
	env/setenv \
	env/unsetenv \
	exit/_Exit \
	exit/abort \
	exit/atexit \
	exit/exit \
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
	path/realpath \
	search/qsort \

stdlib_HDRS := \
	ato_int.h \
	env_cases.h \
	exit_cases.h \
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

# ----- unistd -----

unistd_UNITS := \
	conf/sysconf \
	defs \
	dir/getcwd \
	ext/getpagesize \
	ext/gettid \
	fd/dup2 \
	fd/pipe \
	ids/getgid \
	ids/getpid \
	ids/getppid \
	ids/getuid \
	io/close \
	io/isatty \
	io/lseek \
	io/read \
	io/write \
	link/readlink \
	link/unlink \
	misc/getentropy \
	process/_exit \
	process/fork \
	process/usleep \

unistd_HDRS := \
	unistd_cases.h \

# ----- fcntl -----

fcntl_UNITS := \
	control/fcntl \
	defs \
	open/creat \
	open/open \
	open/openat \

fcntl_HDRS := \
	fcntl_cases.h \

# ----- sys_stat -----

sys_stat_UNITS := \
	defs \
	stat/fstat \
	type/S_ISBLK \
	type/S_ISCHR \
	type/S_ISDIR \
	type/S_ISFIFO \
	type/S_ISLNK \
	type/S_ISREG \
	type/S_ISSOCK \
	type/S_TYPEISMQ \
	type/S_TYPEISSEM \
	type/S_TYPEISSHM \
	type/S_TYPEISTMO \

sys_stat_HDRS := \
	sys_stat_cases.h \
