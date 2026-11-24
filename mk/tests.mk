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

# ----- Framework (built per libc, calls no libc function) -----

FW_OBJS		:= $(patsubst $(FW_DIR)/%.c,$(OUT)/fw/%.o,$(FW_SRCS))
ifeq ($(COV),1)
FW_COV_OBJ	:= $(patsubst $(FW_DIR)/%.c,$(OUT)/fw/%.o,$(FW_COV_SRC))
FW_OBJS		+= $(FW_COV_OBJ)
$(FW_COV_OBJ): FW_DEFS := -DLC_COV_DIR='"$(abspath $(OUT))/cov"'
endif

$(eval $(call c_objects,$(OUT)/fw,$(FW_DIR),LIBC_CC,FW_CFLAGS,CC))
$(FW_OBJS): $(LIBC_DEPS)

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

# Shell: lc_defs_flags := the -DLC_REQ_<name> of category $(1).
lc_defs_shell = lc_defs_flags=""; for n in $(LC_DEFS_$(1)); do \
		if [ "$(PROBE)" != 1 ] || [ "$$(cat $(OUT)/probe/$(1)/$$n.status 2>/dev/null)" = ok ]; \
		then lc_defs_flags="$$lc_defs_flags -DLC_REQ_$$n"; fi; done

# A defs unit waits for the probes of its symbols.
$(foreach c,$(CATEGORIES),$(eval $(OUT)/bin/$(c)/defs: \
	$(if $(filter 1,$(PROBE)),$(addprefix $(OUT)/probe/$(c)/,$(addsuffix .status,$(LC_DEFS_$(c)))))))

$(OUT)/bin/%: $(SUITES_DIR)/%.c $(FW_HDRS) $(SUITE_HDRS) $(FW_OBJS) $(LIBC_DEPS) $(SPEC_MK)
	@mkdir -p $(@D)
	$(AT)$(if $(filter %/defs,$*),$(call lc_defs_shell,$(firstword $(subst /, ,$*)));) \
	if $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(LAX_CFLAGS) \
		$(if $(filter $*,$(LC_EXT_UNITS)),$(LC_EXT_CFLAGS)) \
		$(if $(filter %/defs,$*),$$lc_defs_flags) \
		$(TEST_CFLAGS) $(COV_CFLAGS) \
		-DLC_CATEGORY='"$(firstword $(subst /, ,$*))"' \
		-DLC_FUNCTION='"$(lastword $(subst /, ,$*))"' \
		$< $(FW_OBJS) $(LIBC_LDFLAGS) $(COV_LDFLAGS) $(LIBC_LIBS) -o $@ 2> $@.err; \
	then rm -f $@.err; $(call lc_tick,$*,  TEST     $*); \
	else rm -f $@; $(call lc_clear); echo "  N/A      $*  (see $@.err)"; \
		$(call lc_tick,$*); fi

# ----- Probes: is every required interface declared and defined? -----
# undeclared: the probe does not compile (header misses it);
# unlinked:   it compiles but does not link: undefined symbol (library
#             misses it);
# broken:     it compiles but does not link for another reason (compiler
#             crash in LTO, archive built for coverage, ...): see the .err;
# ok:         both. Macros and types are only compiled.

$(OUT)/probe-src.stamp: $(SPEC_MK)
	@rm -rf $(OUT)/probe-src
	@$(RUNNER) $(LCTARGET) $(LCPATHS) --spec-probes $(OUT)/probe-src 2>/dev/null
	@touch $@

$(OUT)/probe/%.status: $(OUT)/probe-src.stamp $(LIBC_DEPS)
	@mkdir -p $(@D)
	$(AT)base=$(@:.status=); \
	if ! $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(LAX_CFLAGS) $(PROBE_CFLAGS) \
		-c $(OUT)/probe-src/$*.c -o $$base.o 2> $$base.err; then \
		echo undeclared > $@; \
	elif [ -n "$(filter $*,$(LC_PROBES_LINK))" ] && ! $(LIBC_CC) $$base.o \
		$(LIBC_LDFLAGS) $(LIBC_LIBS) -o $$base 2>> $$base.err; then \
		if grep -Eq 'undefined (reference to|symbol)' $$base.err; \
		then echo unlinked; else echo broken; fi > $@; \
	else echo ok > $@; fi; $(call lc_tick,probe $*)

# ----- Hidden probes (strict mode): must the header hide it? -----
# Built in the target's context only (no LAX_CFLAGS, no extension macro):
# it compiles exactly when the header makes the name visible anyway.
# leaked: visible although the target forbids it; hidden: as required.

$(OUT)/probe/%.hidden: $(OUT)/probe-src.stamp $(LIBC_DEPS)
	@mkdir -p $(@D)
	$(AT)base=$(@:.hidden=).hid; \
	if $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(PROBE_CFLAGS) \
		-c $(OUT)/probe-src/$*.hidden.c -o $$base.o 2> $$base.err; then \
		echo leaked > $@; \
	else echo hidden > $@; fi; rm -f $$base.o; $(call lc_tick,hidden $*)

# ----- Reject probes: translation units the implementation must refuse -----
# e.g. K.3.1.1p4, __STDC_WANT_LIB_EXT1__ changed between two inclusions:
# "a diagnostic as if a preprocessor error directive were used".
# rejected: refused, as required; accepted: compiled, diagnostic missing.

$(OUT)/probe/%.reject: $(OUT)/probe-src.stamp $(LIBC_DEPS)
	@mkdir -p $(@D)
	$(AT)base=$(@:.reject=).rej; \
	if $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(LAX_CFLAGS) $(PROBE_CFLAGS) \
		-c $(OUT)/probe-src/$*.reject.c -o $$base.o 2> $$base.err; then \
		echo accepted > $@; \
	else echo rejected > $@; fi; rm -f $$base.o; $(call lc_tick,reject $*)
