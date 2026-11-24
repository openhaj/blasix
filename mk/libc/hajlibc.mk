# hajlibc.mk - hajlibc, through its public build interface (mk/hajlib.mk).
#
#   make run LIBC=hajlibc HAJLIBC=/path/to/hajlibc
#
# Default HAJLIBC: ../.. when blasix is a submodule of hajlibc
# (hajlibc/tests/suite), otherwise ../hajlibc next to this repository.

ifeq ($(shell test -f ../../mk/hajlib.mk && echo 1),1)
HAJLIBC			?= $(abspath ../..)
else
HAJLIBC			?= $(abspath ../hajlibc)
endif

HAJ_ROOT		:= $(HAJLIBC)
include $(HAJLIBC)/mk/hajlib.mk

# The archive is copied and re-indexed with the archiver matching the
# compiler's LTO objects (gcc-ar for GCC, llvm-ar for clang): an index
# written with the wrong LTO plugin is empty and nothing would link.
_LC_IS_CLANG	:= $(shell $(CC) --version 2>/dev/null | grep -qi clang && echo 1)
ifeq ($(_LC_IS_CLANG),1)
LC_AR			:= $(or $(shell command -v llvm-ar 2>/dev/null),ar)
else
LC_AR			:= $(or $(shell command -v gcc-ar 2>/dev/null),ar)
endif

LIBC_ARCHIVE	:= $(OUT)/libhajc.a
LIBC_CC			:= $(CC)
LIBC_CPPFLAGS	:= -nostdinc $(HAJ_CPPFLAGS)
LIBC_CFLAGS		:= $(HAJ_CFLAGS)
LIBC_LDFLAGS	:= $(HAJ_LDFLAGS)
LIBC_LIBS		:= $(LIBC_ARCHIVE) $(shell $(CC) -print-libgcc-file-name)
LIBC_DEPS		:= $(LIBC_ARCHIVE)

libc-prepare:
	@echo "  MAKE     hajlibc ($(HAJLIBC))"
	@$(MAKE) --no-print-directory -s -C $(HAJLIBC) >/dev/null

$(LIBC_ARCHIVE): $(HAJ_LIB)
	@mkdir -p $(@D)
	@cp $< $@
	@$(LC_AR) s $@

# ----- Coverage -----
# The system libgcov needs glibc, so it is not used: hajlibc is built
# with -fprofile-info-section and the framework (src/framework/cov.c) writes
# the .gcda itself; only libgcov's __gcov_info_to_gcda is linked. See
# cov.c for the details. The library is rebuilt without instrumentation
# at the end of `make coverage`.

LIBC_COV_SUPPORTED	:= 1
LC_COV_LIBC_CFLAGS	:= -fprofile-arcs -ftest-coverage \
	-fprofile-info-section=gcov_info -O0 -g
ifeq ($(COV),1)
COV_LDFLAGS		:= -Wl,--undefined=__gcov_info_to_gcda \
	$(shell $(CC) -print-file-name=libgcov.a)
endif

LIBC_COV_PREPARE	= \
	echo "  COV      rebuilding hajlibc with coverage counters"; \
	$(MAKE) --no-print-directory -s -C $(HAJLIBC) fclean >/dev/null; \
	$(MAKE) --no-print-directory -s -C $(HAJLIBC) \
		HAJ_EXTRA_CFLAGS='$(LC_COV_LIBC_CFLAGS)' >/dev/null

LIBC_COV_REPORT		= \
	scripts/cov-report.sh $(HAJLIBC)/src $(HAJLIBC)/objs/src \
		$(abspath $(OUT))/cov $(OUT)/coverage; \
	echo "  COV      rebuilding hajlibc without instrumentation"; \
	$(MAKE) --no-print-directory -s -C $(HAJLIBC) fclean >/dev/null; \
	$(MAKE) --no-print-directory -s -C $(HAJLIBC) >/dev/null; \
	rm -rf $(OUT)/bin $(OUT)/fw $(LIBC_ARCHIVE)
