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
# Clang: llvm-ar, else llvm-ar-NN (Debian/Ubuntu name it after the
# version), else ar with LLVMgold.so, as hajlibc does (mk/cross.mk).
ifeq ($(_LC_IS_CLANG),1)
_LC_CLANG_VER	:= $(shell $(CC) -dumpversion 2>/dev/null | cut -d. -f1)
_LC_LLVM_GOLD	:= $(shell p=$$($(CC) -print-file-name=LLVMgold.so 2>/dev/null); \
	[ -f "$$p" ] && echo "$$p")
LC_AR			:= $(or $(shell command -v llvm-ar 2>/dev/null),$(shell command -v llvm-ar-$(_LC_CLANG_VER) 2>/dev/null),ar$(if $(_LC_LLVM_GOLD), --plugin $(_LC_LLVM_GOLD)))
LC_NM			:= $(or $(shell command -v llvm-nm 2>/dev/null),$(shell command -v llvm-nm-$(_LC_CLANG_VER) 2>/dev/null),nm$(if $(_LC_LLVM_GOLD), --plugin $(_LC_LLVM_GOLD)))
else
# gcc-NN-ar matches gcc-NN's LTO plugin; plain gcc-ar may be another version.
# Named after CC only when CC's name says gcc (gcc-14 -> gcc-ar-14,
# x86_64-linux-gnu-gcc -> x86_64-linux-gnu-gcc-ar): for CC=cc the
# substitution would give cc itself. Then gcc-ar-NN after CC's version
# (cc -> gcc-12 on Debian), then gcc-ar, then ar.
_LC_CC_NAME		:= $(notdir $(firstword $(CC)))
_LC_GCC_VER		:= $(shell $(CC) -dumpversion 2>/dev/null | cut -d. -f1)
lc_gcc_tool		= $(or \
	$(if $(findstring gcc,$(_LC_CC_NAME)),$(shell command -v $(subst gcc,gcc-$(1),$(_LC_CC_NAME)) 2>/dev/null)), \
	$(if $(_LC_GCC_VER),$(shell command -v gcc-$(1)-$(_LC_GCC_VER) 2>/dev/null)), \
	$(shell command -v gcc-$(1) 2>/dev/null),$(1))
LC_AR			:= $(strip $(call lc_gcc_tool,ar))
LC_NM			:= $(strip $(call lc_gcc_tool,nm))
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

# Static: a new archive relinks only what pulls a changed member (tests.mk).
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

#
# -fprofile-info-section and __gcov_info_to_gcda need GCC 12 or later:
# clang and older GCCs have neither. The whole coverage build (libc,
# framework, tests) uses LC_COV_CC: CC itself when it is GCC >= 12,
# otherwise the newest gcc-NN >= 12 installed; COV_CC=... chooses it.
# gcov and gcov-tool must match its version (gcov-NN when they exist).
lc_gcc_ok		= $(shell v=$$($(1) -dumpversion 2>/dev/null) && \
	! $(1) --version 2>/dev/null | grep -qi clang && \
	[ "$${v%%.*}" -ge 12 ] 2>/dev/null && echo 1)
ifdef COV_CC
LC_COV_CC		:= $(if $(call lc_gcc_ok,$(COV_CC)),$(COV_CC))
else
LC_COV_CC		:= $(if $(call lc_gcc_ok,$(CC)),$(CC),$(firstword \
	$(foreach v,16 15 14 13 12,$(if $(call lc_gcc_ok,gcc-$(v)),gcc-$(v)))))
endif
LC_COV_VER		:= $(if $(LC_COV_CC),$(shell $(LC_COV_CC) -dumpversion | cut -d. -f1))
LC_COV_GCOV		:= $(or $(shell command -v gcov-$(LC_COV_VER) 2>/dev/null),gcov)
LC_COV_GCOV_TOOL	:= $(or $(shell command -v gcov-tool-$(LC_COV_VER) 2>/dev/null),gcov-tool)

LIBC_COV_SUPPORTED	:= $(if $(LC_COV_CC),1,0)
LIBC_COV_WHY		:= needs GCC >= 12, CC=$(CC) is not: set COV_CC=gcc-NN
LIBC_COV_MAKEFLAGS	:= CC=$(LC_COV_CC)
LC_COV_LIBC_CFLAGS	:= -fprofile-arcs -ftest-coverage \
	-fprofile-info-section=gcov_info -O0 -g -fno-lto
ifeq ($(COV),1)
COV_LDFLAGS		:= -Wl,--undefined=__gcov_info_to_gcda \
	$(shell $(CC) -print-file-name=libgcov.a)
endif

LIBC_COV_PREPARE	= true
LIBC_COV_REPORT		= \
	GCOV=$(LC_COV_GCOV) GCOV_TOOL=$(LC_COV_GCOV_TOOL) \
	scripts/cov-report.sh $(LC_COV_TREE) $(LC_COV_TREE)/objs \
		$(abspath $(OUT))/cov $(abspath $(OUT))/coverage $(HAJLIBC); \
	rm -rf $(OUT)/bin $(OUT)/fw $(LIBC_ARCHIVE)
