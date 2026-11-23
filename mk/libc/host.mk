# host.mk - the system libc (glibc, musl, ...): one libc under test like any other.
#
# A libc profile defines:
#   LIBC_CC          compiler driver
#   LIBC_CPPFLAGS    include paths
#   LIBC_CFLAGS      flags every TU needs to build against this libc
#                    (no -std / feature macros: the target sets them)
#   LIBC_LDFLAGS     link flags
#   LIBC_LIBS        libraries, after the objects
#   LIBC_DEPS        files the test binaries depend on (rebuild trigger)
#   libc-prepare     recipe run before building (e.g. build the libc)
#   LIBC_COV_SUPPORTED / LIBC_COV_PREPARE / LIBC_COV_REPORT (optional)

LIBC_CC			:= $(HOST_CC)
LIBC_CPPFLAGS	:=
LIBC_CFLAGS		:=
LIBC_LDFLAGS	:=
# glibc keeps <math.h> / <complex.h> in libm and (before 2.34) threads in
# libpthread; with musl both are in libc and the flags are harmless.
LIBC_LIBS		:= -lpthread -lm
LIBC_DEPS		:=

libc-prepare:
	@true

LIBC_COV_SUPPORTED := 0
