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
TEST_CFLAGS	:= -O1 -g -fno-builtin $(WARNINGS) -I$(INC_DIR)
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

$(OUT)/bin/%: $(SUITES_DIR)/%.c $(FW_HDRS) $(FW_OBJS) $(LIBC_DEPS) $(SPEC_MK)
	@mkdir -p $(@D)
	$(AT)if $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) \
		$(if $(filter $*,$(LC_EXT_UNITS)),$(LC_EXT_CFLAGS)) \
		$(TEST_CFLAGS) $(COV_CFLAGS) \
		-DLC_CATEGORY='"$(word 1,$(subst /, ,$*))"' \
		-DLC_FUNCTION='"$(word 2,$(subst /, ,$*))"' \
		$< $(FW_OBJS) $(LIBC_LDFLAGS) $(COV_LDFLAGS) $(LIBC_LIBS) -o $@ 2> $@.err; \
	then rm -f $@.err; echo "  TEST     $*"; \
	else rm -f $@; echo "  N/A      $*  (see $@.err)"; fi

# ----- Probes: is every required interface declared and defined? -----
# undeclared: the probe does not compile (header misses it);
# unlinked:   it compiles but does not link (library misses it);
# ok:         both. Macros and types are only compiled.

$(OUT)/probe-src.stamp: $(SPEC_MK)
	@rm -rf $(OUT)/probe-src
	@$(RUNNER) $(LCTARGET) $(LCPATHS) --spec-probes $(OUT)/probe-src 2>/dev/null
	@touch $@

$(OUT)/probe/%.status: $(OUT)/probe-src.stamp $(LIBC_DEPS)
	@mkdir -p $(@D)
	$(AT)base=$(@:.status=); \
	if ! $(LIBC_CC) $(LIBC_BASE) $(LC_STD_CFLAGS) $(PROBE_CFLAGS) \
		-c $(OUT)/probe-src/$*.c -o $$base.o 2> $$base.err; then \
		echo undeclared > $@; \
	elif [ -n "$(filter $*,$(LC_PROBES_LINK))" ] && ! $(LIBC_CC) $$base.o \
		$(LIBC_LDFLAGS) $(LIBC_LIBS) -o $$base 2>> $$base.err; then \
		echo unlinked > $@; \
	else echo ok > $@; fi
