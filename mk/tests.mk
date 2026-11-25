# tests.mk - framework, test units and probes, built against the libc
# under test (LIBC_* from mk/libc/$(LIBC).mk).
#
# -fno-builtin everywhere: the compiler must not fold strlen("") or
# memcpy into inline code, or the test would not test the libc.
# No _GNU_SOURCE: the feature-test macros come from the target only
# (LC_STD_CFLAGS); extension units also get LC_EXT_CFLAGS.

LIBC_BASE	= $(LIBC_CPPFLAGS) $(LIBC_CFLAGS)
FW_CFLAGS	= $(LIBC_BASE) -O2 -fno-builtin -ffreestanding -fno-stack-protector \
	$(WARNINGS) $(FW_DEFS)
# -I$(SUITES_DIR): units include "<cat>/<cat>_cases.h" and "bits/<symbol>.h"
# from any group folder.
TEST_CFLAGS	:= -O1 -g -fno-builtin $(WARNINGS) -I$(INC_DIR) -I$(SUITES_DIR)
PROBE_CFLAGS := -O0 -fno-builtin -Werror=implicit-function-declaration

# Set by the libc profile when COV=1.
COV_CFLAGS	?=
COV_LDFLAGS	?=
# Set by a profile whose libc is a static archive (see "Static libc").
LIBC_ARCHIVE	?=

# ----- Framework (built per libc, calls no libc function) -----

FW_OBJS		:= $(patsubst $(FW_DIR)/%.c,$(OUT)/fw/%.o,$(FW_SRCS))
ifeq ($(COV),1)
FW_COV_OBJ	:= $(patsubst $(FW_DIR)/%.c,$(OUT)/fw/%.o,$(FW_COV_SRC))
FW_OBJS		+= $(FW_COV_OBJ)
$(FW_COV_OBJ): FW_DEFS := -DLC_COV_DIR='"$(abspath $(OUT))/cov"'
endif

$(eval $(call c_objects,$(OUT)/fw,$(FW_DIR),LIBC_CC,FW_CFLAGS,CC))
# With a static archive the framework follows the libc headers through
# its .d: a new archive alone does not rebuild it (and every unit).
$(FW_OBJS): $(if $(LIBC_ARCHIVE),| $(LIBC_DEPS),$(LIBC_DEPS))

DEPS		+= $(FW_OBJS:.o=.d)

# ----- Test units -----
# A unit that does not compile or link (function missing in this libc)
# is not a build error: the binary is just absent, the compiler output
# is kept in <bin>.err and lcrun reports the unit as unavailable.
# <cat>/defs (types and constants) also gets -DLC_REQ_<name> for every
# type and macro the target requires (LC_DEFS_<cat>, spec.mk) that the
# header declares (its probe says ok; all of them with PROBE=0): each
# bits/ file it includes only compiles the cases of such a symbol, so a
# missing one (reported undeclared by its probe) does not take the
# others with it.

# The -DLC_REQ_<name> of category $(1), read from the probe results when
# the recipe runs (after the probes): part of the defs unit's command file,
# so a probe that runs again with the same result does not rebuild it.
lc_defs_req = $(foreach n,$(LC_DEFS_$(1)),$(if $(or $(filter-out 1,$(PROBE)),\
	$(filter ok,$(file <$(OUT)/probe/$(1)/$(n).status))),-DLC_REQ_$(n)))

# ----- Static libc: relink only what uses a changed member -----
# With LIBC_ARCHIVE (the profile's static archive), a unit or a link probe
# does not depend on the archive's date but on <link>.libc, a key of the
# archive members its linker map (<link>.map) says it pulled, plus the
# symbol table (lcrun --libc-key, src/runner/tools/archive.c). The key is
# rewritten only when it changes: a new archive relinks the units whose
# members changed. A link that failed on an undefined symbol depends on the
# whole archive (a new member may provide it); one that did not compile
# (or a probe that says undeclared) on none of it, only on the headers.

LC_LIBC_INDEX	:= $(if $(LIBC_ARCHIVE),$(OUT)/libc.index)

lc_index_gen	= $(RUNNER) --ar-index $(LIBC_ARCHIVE) > $(LC_LIBC_INDEX).tmp && \
	$(call lc_update,$(LC_LIBC_INDEX).tmp,$(LC_LIBC_INDEX))

# Shell: write $(1).libc, the key of link $(1); condition $(2) is true
# when the result does not depend on the library.
lc_libc_key		= { if $(2); then echo none > $(1).libc.tmp; else $(RUNNER) --libc-key \
	$(LC_LIBC_INDEX) $(LIBC_ARCHIVE) $(1).map > $(1).libc.tmp; fi && \
	$(call lc_update,$(1).libc.tmp,$(1).libc); }
lc_unit_nolib	= { [ ! -s $(1) ] && ! grep -Eqs 'undefined (reference to|symbol)' $(1).err; }
lc_probe_nolib	= [ "$$(cat $(1).status 2>/dev/null)" = undeclared ]

ifneq ($(LIBC_ARCHIVE),)
$(LC_LIBC_INDEX).stamp: $(LIBC_ARCHIVE) $(RUNNER)
	@$(lc_index_gen)
	@touch $@

$(LC_LIBC_INDEX): $(LC_LIBC_INDEX).stamp
	@test -f $@ || { $(lc_index_gen); }

$(OUT)/bin/%.libc.stamp: $(LC_LIBC_INDEX)
	@mkdir -p $(@D)
	@$(call lc_libc_key,$(OUT)/bin/$*,$(call lc_unit_nolib,$(OUT)/bin/$*))
	@touch $@

$(OUT)/bin/%.libc: $(OUT)/bin/%.libc.stamp
	@test -f $@ || $(call lc_libc_key,$(OUT)/bin/$*,$(call lc_unit_nolib,$(OUT)/bin/$*))

$(OUT)/probe/%.libc.stamp: $(LC_LIBC_INDEX)
	@mkdir -p $(@D)
	@$(call lc_libc_key,$(OUT)/probe/$*,$(call lc_probe_nolib,$(OUT)/probe/$*))
	@touch $@

$(OUT)/probe/%.libc: $(OUT)/probe/%.libc.stamp
	@test -f $@ || $(call lc_libc_key,$(OUT)/probe/$*,$(call lc_probe_nolib,$(OUT)/probe/$*))

# Only the declaration probes that link follow the library.
$(addprefix $(OUT)/probe/,$(addsuffix .status,$(filter $(LC_PROBES_LINK),$(PROBES)))): \
	$(OUT)/probe/%.status: $(OUT)/probe/%.libc
endif

# What a link depends on in the library besides its key: the archive's
# date when there is no LIBC_ARCHIVE (LIBC_DEPS is all make knows), else
# only that it exists.
LC_LIBC_DEP		:= $(if $(LIBC_ARCHIVE),| $(LIBC_DEPS),$(LIBC_DEPS))

# Everything a unit's binary depends on besides files: compiler, flags,
# libraries. Written to <bin>.cmd only when it changes (lc_write_cmd), so
# a new unit, a spec edit or another target's flags do not rebuild the
# units whose command line stayed the same.
lc_unit_cmd	= $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(LAX_CFLAGS) \
	$(if $(filter $(1),$(LC_EXT_UNITS)),$(LC_EXT_CFLAGS)) \
	$(if $(filter %/defs,$(1)),$(call lc_defs_req,$(firstword $(subst /, ,$(1))))) \
	$(TEST_CFLAGS) $(COV_CFLAGS) $(LIBC_LDFLAGS) $(COV_LDFLAGS) $(LIBC_LIBS)

$(OUT)/bin/%.cmd: FORCE
	$(call lc_write_cmd,$@,$(call lc_unit_cmd,$*))

# A defs unit waits for the probes of its symbols (through its command
# file, which holds what they found).
$(foreach c,$(CATEGORIES),$(eval $(OUT)/bin/$(c)/defs.cmd: \
	$(if $(filter 1,$(PROBE)),$(addprefix $(OUT)/probe/$(c)/,$(addsuffix .status,$(LC_DEFS_$(c)))))))

# The headers a unit includes come from its .d (-MMD): a change to a
# *_cases.h or a bits/ file rebuilds the units that include it, not all.
# A unit that did not build leaves an empty, non-executable file: it is
# not tried again until one of its inputs changes (lcrun reports it
# unavailable, with its .err). With LIBC_ARCHIVE the link also writes its
# map and the key of the members it pulled (<bin>.libc); the binary is
# then made the newest of the two.
$(OUT)/bin/%: $(SUITES_DIR)/%.c $(OUT)/bin/%.cmd $(FW_OBJS) \
		$(if $(LIBC_ARCHIVE),$(OUT)/bin/%.libc) $(LC_LIBC_DEP)
	@mkdir -p $(@D)
	$(AT)rm -f $@.map; \
	if $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(LAX_CFLAGS) \
		$(if $(filter $*,$(LC_EXT_UNITS)),$(LC_EXT_CFLAGS)) \
		$(if $(filter %/defs,$*),$(call lc_defs_req,$(firstword $(subst /, ,$*)))) \
		$(TEST_CFLAGS) $(COV_CFLAGS) -MMD -MP -MF $@.d -MT $@ \
		-DLC_CATEGORY='"$(firstword $(subst /, ,$*))"' \
		-DLC_FUNCTION='"$(lastword $(subst /, ,$*))"' \
		$< $(FW_OBJS) $(LIBC_LDFLAGS) $(COV_LDFLAGS) $(LIBC_LIBS) \
		$(if $(LIBC_ARCHIVE),-Wl$(comma)-Map=$@.map) -o $@ 2> $@.err; \
	then rm -f $@.err; $(call lc_tick,$*,  TEST     $*); \
	else rm -f $@ $@.map; : > $@; $(call lc_clear); echo "  N/A      $*  (see $@.err)"; \
		$(call lc_tick,$*); fi$(if $(LIBC_ARCHIVE),; \
	$(call lc_libc_key,$@,$(call lc_unit_nolib,$@)) && touch $@.libc.stamp && touch -c $@)

# ----- Probes: is every required interface declared and defined? -----
# undeclared: the probe does not compile (header misses it);
# unlinked:   it compiles but does not link: undefined symbol (library
#             misses it);
# broken:     it compiles but does not link for another reason (compiler
#             crash in LTO, archive built for coverage, ...): see the .err;
# ok:         both. Macros and types are only compiled.

#
# The probe sources are generated in a new directory and copied to probe-src
# only where they changed (scripts/sync-tree.sh): a probe runs again when
# its source, a header it includes (.d) or its flags (probe/flags.cmd)
# change. Declaration probes of functions and variables are also linked,
# so they follow the library too (the members they pull with
# LIBC_ARCHIVE, else LIBC_DEPS); hidden and reject probes are only compiled.

# Each generation has its own directory: when several probe sources are
# missing, make may regenerate them in parallel.
lc_probe_gen = mkdir -p $(OUT) && new=$$(mktemp -d $(OUT)/probe-src.new.XXXXXX) && \
	{ $(RUNNER) $(LCTARGET) $(LCPATHS) --spec-probes $$new 2>/dev/null || \
		{ rm -rf $$new; false; }; } && \
	scripts/sync-tree.sh $$new $(OUT)/probe-src

$(OUT)/probe-src.stamp: $(RUNNER) $(SPECS) $(SPEC_MK)
	@$(lc_probe_gen)
	@touch $@

$(OUT)/probe-src/%.c: $(OUT)/probe-src.stamp
	@test -f $@ || { $(lc_probe_gen); }

lc_probe_cmd = $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(LAX_CFLAGS) $(PROBE_CFLAGS) \
	$(LIBC_LDFLAGS) $(COV_LDFLAGS) $(LIBC_LIBS)

$(OUT)/probe/flags.cmd: FORCE
	$(call lc_write_cmd,$@,$(lc_probe_cmd))

# Kept between runs: make would delete them as intermediate files.
.PRECIOUS: $(OUT)/bin/%.cmd $(OUT)/probe-src/%.c $(OUT)/bin/%.libc \
	$(OUT)/bin/%.libc.stamp $(OUT)/probe/%.libc $(OUT)/probe/%.libc.stamp

DEPS		+= $(addsuffix .d,$(BINS)) \
	$(addprefix $(OUT)/probe/,$(addsuffix .d,$(PROBES)) \
		$(addsuffix .hid.d,$(HIDDEN)) $(addsuffix .rej.d,$(REJECT)))

$(OUT)/probe/%.status: $(OUT)/probe-src/%.c $(OUT)/probe/flags.cmd $(LC_LIBC_DEP)
	@mkdir -p $(@D)
	$(AT)base=$(@:.status=); rm -f $$base.map; \
	if ! $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(LAX_CFLAGS) $(PROBE_CFLAGS) \
		-MMD -MP -MF $$base.d -MT $@ \
		-c $(OUT)/probe-src/$*.c -o $$base.o 2> $$base.err; then \
		echo undeclared > $@; \
	elif [ -n "$(filter $*,$(LC_PROBES_LINK))" ] && ! $(LIBC_CC) $$base.o \
		$(LIBC_LDFLAGS) $(COV_LDFLAGS) $(LIBC_LIBS) \
		$(if $(LIBC_ARCHIVE),-Wl$(comma)-Map=$$base.map) -o $$base 2>> $$base.err; then \
		rm -f $$base.map; \
		if grep -Eq 'undefined (reference to|symbol)' $$base.err; \
		then echo unlinked; else echo broken; fi > $@; \
	else echo ok > $@; fi; \
	$(if $(and $(LIBC_ARCHIVE),$(filter $*,$(LC_PROBES_LINK))),\
		$(call lc_libc_key,$$base,$(call lc_probe_nolib,$$base)) && \
		touch $$base.libc.stamp && touch -c $@;) \
	$(call lc_tick,probe $*)

# ----- Hidden probes (strict mode): must the header hide it? -----
# Built in the target's context only (no LAX_CFLAGS, no extension macro):
# it compiles exactly when the header makes the name visible anyway.
# leaked: visible although the target forbids it; hidden: as required.

$(OUT)/probe/%.hidden: $(OUT)/probe-src/%.hidden.c $(OUT)/probe/flags.cmd
	@mkdir -p $(@D)
	$(AT)base=$(@:.hidden=).hid; \
	if $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(PROBE_CFLAGS) \
		-MMD -MP -MF $$base.d -MT $@ \
		-c $(OUT)/probe-src/$*.hidden.c -o $$base.o 2> $$base.err; then \
		echo leaked > $@; \
	else echo hidden > $@; fi; rm -f $$base.o; $(call lc_tick,hidden $*)

# ----- Reject probes: translation units the implementation must refuse -----
# e.g. K.3.1.1p4, __STDC_WANT_LIB_EXT1__ changed between two inclusions:
# "a diagnostic as if a preprocessor error directive were used".
# rejected: refused, as required; accepted: compiled, diagnostic missing.

$(OUT)/probe/%.reject: $(OUT)/probe-src/%.reject.c $(OUT)/probe/flags.cmd
	@mkdir -p $(@D)
	$(AT)base=$(@:.reject=).rej; \
	if $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(LAX_CFLAGS) $(PROBE_CFLAGS) \
		-MMD -MP -MF $$base.d -MT $@ \
		-c $(OUT)/probe-src/$*.reject.c -o $$base.o 2> $$base.err; then \
		echo accepted > $@; \
	else echo rejected > $@; fi; rm -f $$base.o; $(call lc_tick,reject $*)
