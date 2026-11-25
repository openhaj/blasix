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
#   LIBC_RUNTIME     (optional) the shared libraries the binaries load at
#                    run time: if one changes, the result cache is dropped
#                    (a static libc is in the binaries: leave it empty)
#   libc-prepare     recipe run before building (e.g. build the libc)
#   LIBC_LAX_CFLAGS  (optional) lax mode only, LAX=1: macros that make the
#                    headers expose everything they have, so that a function
#                    a strict target hides is still tested
#   LIBC_COV_SUPPORTED / LIBC_COV_PREPARE / LIBC_COV_REPORT (optional):
#                    coverage support, see mk/libc/hajlibc.mk; with COV=1
#                    libc-prepare builds the instrumented libc

LIBC_CC			:= $(HOST_CC)
LIBC_CPPFLAGS	:=
LIBC_CFLAGS		:=
LIBC_LDFLAGS	:=
# glibc keeps <math.h> / <complex.h> in libm and (before 2.34) threads in
# libpthread; with musl both are in libc and the flags are harmless.
LIBC_LIBS		:= -lpthread -lm
LIBC_DEPS		:=
# The shared libc the tests run with (a system update must rerun them).
LIBC_RUNTIME	:= $(foreach l,libc.so.6 libm.so.6 libpthread.so.0 \
	ld-linux-x86-64.so.2 ld-linux-aarch64.so.1 libc.musl-x86_64.so.1,\
	$(realpath $(shell $(LIBC_CC) -print-file-name=$(l))))
# glibc and musl: _DEFAULT_SOURCE exposes POSIX.1-2024 additions such as
# memmem, strlcpy, ffsl that they only show as extensions
LIBC_LAX_CFLAGS	:= -D_DEFAULT_SOURCE

libc-prepare:
	@true

LIBC_COV_SUPPORTED := 0
