# run.mk - running the tests, comparing libcs, spec tools, coverage.
#
# Options: TEST='glob' V=1 Q=1 REPEAT=n TIMEOUT=s JOBS=n PROBE=0 CACHE=0
#
# Result cache ($(OUT)/cache.tsv): a test whose binary, timeout and
# run-time libc (LIBC_RUNTIME) did not change since its last run is not
# run again (src/runner/run/cache.c). CACHE=0 runs every test. Never
# used for coverage: the tests must run to write their counters.

CACHE		?= 1
LIBC_RUNTIME	?=
lc_empty	:=
lc_space	:= $(lc_empty) $(lc_empty)
LC_CACHE	:= $(if $(filter 1,$(CACHE)),$(if $(filter 1,$(COV)),,$(OUT)/cache.tsv))

LCFLAGS		:= $(LCTARGET) $(LCPATHS) -L $(LIBC) -j $(JOBS)
LCFLAGS		+= $(if $(filter 1,$(PROBE)),-P $(OUT)/probe)
LCFLAGS		+= $(if $(CAT),-c $(CAT)) $(if $(FUNC),-f $(FUNC))
LCFLAGS		+= $(if $(TEST),-t '$(TEST)') $(if $(filter 1,$(V)),-v)
LCFLAGS		+= $(if $(filter 1,$(Q)),-q) $(if $(REPEAT),-r $(REPEAT))
LCFLAGS		+= $(if $(TIMEOUT),-T $(TIMEOUT))
LCFLAGS		+= $(if $(LC_CACHE),-C $(LC_CACHE))
LCFLAGS		+= $(if $(strip $(LIBC_RUNTIME)),-K $(subst $(lc_space),$(comma),$(strip $(LIBC_RUNTIME))))

.PHONY: run run-all list compare spec-list spec-check matrix todo options \
	standards coverage

run: build
	@$(RUNNER) $(LCFLAGS) -o $(OUT)/results.tsv -J $(OUT)/results.json $(OUT)/bin

# Every standard in turn, with its default options: one result line each.
run-all: $(RUNNER)
	@for s in $(STANDARDS); do \
		printf '%-11s ' "$$s"; \
		$(MAKE) --no-print-directory run STD=$$s Q=1 2>&1 | tail -1; \
	done

list: build
	@$(RUNNER) $(LCFLAGS) -l $(OUT)/bin

A ?= host
B ?= hajlibc
compare: $(RUNNER)
	-@$(MAKE) --no-print-directory run LIBC=$(A) Q=1
	-@$(MAKE) --no-print-directory run LIBC=$(B) Q=1
	@$(RUNNER) --compare $(BUILD_DIR)/$(A)/$(TARGET)/results.tsv \
		$(BUILD_DIR)/$(B)/$(TARGET)/results.tsv

# ----- Spec tools -----

spec-list: $(RUNNER)
	@$(RUNNER) $(LCTARGET) $(LCPATHS) $(if $(filter 1,$(V)),-v) --spec-list

spec-check: $(RUNNER) sources-check
	@$(RUNNER) $(LCTARGET) $(LCPATHS) --spec-check

# Progress: tested / required per header in each of the 8 standards.
matrix: $(RUNNER)
	@$(RUNNER) $(LCTARGET) $(LCPATHS) $(if $(CAT),-c $(CAT)) --spec-matrix

# What the target requires and no unit tests yet.
todo: $(RUNNER)
	@$(RUNNER) $(LCTARGET) $(LCPATHS) $(if $(CAT),-c $(CAT)) --spec-todo

options: $(RUNNER)
	@$(RUNNER) --options

standards: $(RUNNER)
	@$(RUNNER) --standards

# ----- Coverage (the profile says how: LIBC_COV_*) -----

coverage:
ifeq ($(LIBC_COV_SUPPORTED),1)
	@$(LIBC_COV_PREPARE)
	@rm -rf $(OUT)/bin $(OUT)/fw $(OUT)/cov $(OUT)/probe $(LIBC_DEPS)
	-@$(MAKE) --no-print-directory run COV=1
	@$(LIBC_COV_REPORT)
else
	@echo "coverage: not supported for LIBC=$(LIBC) (needs an instrumented libc build)"
	@exit 1
endif
