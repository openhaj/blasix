# blasix - conformance tests for C libraries, driven by the spec.
#
# Layout:
#   spec/            what each standard requires, one file per header
#   src/suites/      one test file per function: <header>/<group>/<name>.c;
#                    <header>/defs.c: its types and constants, whose cases
#                    are in bits/<origin>/<symbol>.h
#   src/framework/   test-side engine (no libc dependency, raw syscalls)
#   src/runner/      lcrun, built with the host libc
#   include/         headers of the framework and of lcrun
#   mk/              build modules; mk/libc/: one profile per libc
#   data/            texts of the 8 standards (not versioned, data/README.md)
#   templates/       test.c, defs.c, spec.spec (make new)
#   plan.md          order in which the headers are specified and tested
#
# Target (what the libc is checked against):
#   STD=posix-2024   c99 c11 c17 c23 posix-2001 posix-2008 posix-2017 posix-2024
#   OPTS=XSI         option groups, comma list (make options): POSIX ones
#                    (default XSI on a POSIX target) and ISO C features
#                    (THREADS, COMPLEX, ATOMICS, LIB_EXT1, ...)
#   EXT=0            1: also the extensions (GNU, BSD) listed in spec/
#   LAX=0            1: lax mode, what the headers must hide is not checked
#                    and units also get the profile's LIBC_LAX_CFLAGS
#                    (default strict: hidden probes, "leak" column)
#
# Usage:
#   make run                         build + probe + run (LIBC=host)
#   make run LIBC=hajlibc            against hajlibc (HAJLIBC=path)
#   make run STD=c11 OPTS=THREADS    another target
#   make run CAT=string FUNC=strlen  only some units (comma lists)
#   make run TEST='*/strlen/*' V=1   glob on cat/func/name, verbose
#   make run REPEAT=20               flakiness (each test run 20 times)
#   make run-all                     the 8 standards, one line each
#   make compare A=host B=hajlibc    same target, two libcs, what differs
#   make coverage LIBC=hajlibc       line coverage of the libc sources
#
# Spec and progress:
#   make spec-list                   interfaces the target requires (V=1: all)
#   make spec-check                  lint spec/ and src/suites/
#   make matrix [CAT=a,b]            tested / required per header, 8 standards
#   make todo [CAT=a,b]              required by the target, no test yet
#   make standards / options         the standards, the option groups
#   make new CAT=string GROUP=search FUNC=strchr
#                                    create src/suites/string/search/strchr.c, list it
#                                    in mk/suites.mk
#                                    (SINCE=V adds the spec entry, HEADER=x/y.h)
#   make clean / fclean              remove build/$(LIBC) / build/
#
# Tooling:
#   make init                        enable the git hooks
#   make headers-check / headers-add license headers
#   make sources-check               every file listed in mk/sources.mk, mk/suites.mk
#   make compdb / tidy / format      clangd database, clang-tidy, clang-format
#
# Options: SHOW_FLAGS=1 (full command lines), JOBS=n (parallel build jobs and
# tests; make -jN sets the build jobs too)

.DEFAULT_GOAL := all

include mk/config.mk
include mk/sources.mk
include mk/build.mk
include mk/runner.mk
include mk/target.mk
include mk/tests.mk
include mk/run.mk
include mk/tooling.mk

.PHONY: all build new clean fclean help libc-prepare

all: build

# The units and probes are only known once spec.mk exists, hence the
# sub-make. With nothing selected it is skipped (an empty goal list
# would rebuild the default goal, forever). It runs JOBS jobs in
# parallel, unless make was given its own -jN (then its jobserver is
# shared). A progress bar counts the units and probes (terminal only).
BUILD_GOALS	= $(strip $(BINS) $(PROBE_OUT))
build: $(RUNNER) libc-prepare
	$(if $(BUILD_GOALS),@mkdir -p $(OUT) && : > $(OUT)/.progress && \
		$(MAKE) --no-print-directory -s $(if $(filter -j%,$(MAKEFLAGS)),,-j$(JOBS)) \
		LC_PROGRESS=$(OUT)/.progress LC_TOTAL=$(words $(BUILD_GOALS)) $(BUILD_GOALS); \
		st=$$?; $(lc_clear); exit $$st)

new:
	@scripts/new-test.sh "$(CAT)" "$(FUNC)" "$(GROUP)" "$(SINCE)" "$(HEADER)"

clean:
	rm -rf $(BUILD_DIR)/$(LIBC)

fclean:
	rm -rf $(BUILD_DIR)

help:
	@sed -n '2,/^$$/p' Makefile | sed 's/^# \{0,1\}//'

-include $(DEPS)
