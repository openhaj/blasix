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

# The coverage build (COV=1) is a copy of hajlibc's sources, built with
# coverage counters in the build directory: the hajlibc tree itself is
# never instrumented, so a normal run cannot link against a coverage
# build, even after an interrupted `make coverage`.
LC_COV_TREE		:= $(abspath $(OUT))/libc-cov
LC_COV_COPY		:= Makefile mk include src
ifeq ($(COV),1)
LC_LIBC_BUILT	:= $(LC_COV_TREE)/libhajc.a
else
LC_LIBC_BUILT	:= $(HAJ_LIB)
endif
LC_NM			:= $(if $(filter llvm-ar,$(notdir $(LC_AR))),llvm-nm,$(or $(shell command -v gcc-nm 2>/dev/null),nm))

LIBC_ARCHIVE	:= $(OUT)/libhajc.a
LIBC_CC			:= $(CC)
LIBC_CPPFLAGS	:= -nostdinc $(HAJ_CPPFLAGS)
LIBC_CFLAGS		:= $(HAJ_CFLAGS)
LIBC_LDFLAGS	:= $(HAJ_LDFLAGS)
LIBC_LIBS		:= $(LIBC_ARCHIVE) $(shell $(CC) -print-libgcc-file-name)
LIBC_DEPS		:= $(LIBC_ARCHIVE)

# A libhajc.a left with coverage counters (by hand, `make cov-build` in
# hajlibc, an older blasix) links nothing: every probe would say unlinked.
# It is rebuilt first.
ifeq ($(COV),1)
libc-prepare:
	@echo "  COV      building hajlibc with coverage counters ($(LC_COV_TREE))"
	@rm -rf $(LC_COV_TREE) && mkdir -p $(LC_COV_TREE)
	@cd $(HAJLIBC) && cp -a $(LC_COV_COPY) $(LC_COV_TREE)/
	@$(MAKE) --no-print-directory -s -C $(LC_COV_TREE) \
		HAJ_EXTRA_CFLAGS='$(LC_COV_LIBC_CFLAGS)' >/dev/null
else
libc-prepare:
	@echo "  MAKE     hajlibc ($(HAJLIBC))"
	@if [ -f $(HAJ_LIB) ] && $(LC_NM) $(HAJ_LIB) 2>/dev/null | grep -q __gcov_; then \
		echo "  MAKE     $(HAJ_LIB) has coverage counters: rebuilding it"; \
		$(MAKE) --no-print-directory -s -C $(HAJLIBC) fclean >/dev/null; fi
	@$(MAKE) --no-print-directory -s -C $(HAJLIBC) >/dev/null
endif

$(LIBC_ARCHIVE): $(LC_LIBC_BUILT)
	@mkdir -p $(@D)
	@cp $< $@
	@$(LC_AR) s $@

# ----- Coverage -----
# The system libgcov needs glibc, so it is not used: hajlibc is built
# with -fprofile-info-section and the framework (src/framework/cov.c) writes
# the .gcda itself; only libgcov's __gcov_info_to_gcda is linked. See
# cov.c for the details. The report covers every file compiled into the
# library (scripts/cov-report.sh), under the names of the original sources.

LIBC_COV_SUPPORTED	:= 1
LC_COV_LIBC_CFLAGS	:= -fprofile-arcs -ftest-coverage \
	-fprofile-info-section=gcov_info -O0 -g
ifeq ($(COV),1)
COV_LDFLAGS		:= -Wl,--undefined=__gcov_info_to_gcda \
	$(shell $(CC) -print-file-name=libgcov.a)
endif

LIBC_COV_PREPARE	= true
LIBC_COV_REPORT		= \
	scripts/cov-report.sh $(LC_COV_TREE) $(LC_COV_TREE)/objs \
		$(abspath $(OUT))/cov $(OUT)/coverage $(HAJLIBC); \
	rm -rf $(OUT)/bin $(OUT)/fw $(LIBC_ARCHIVE)
