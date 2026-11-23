# build.mk - common build rules.
#
# This file knows how to compile .c and .S files into .o files, with
# a progress counter. It is included by the top-level Makefile after
# all configuration is done.
#
# Variables it uses (must be defined before including):
#   CC, CFLAGS, CPPFLAGS, OBJDIR, BUILD_DIR, SRCS
#
# Variables it defines:
#   TOTAL_OBJS, PROGRESS
#
# Options (on the command line):
#   SHOW_FLAGS=1 : print the full command line before executing it
#   UNWIND=1     : disable carriage-return line overwrite (non-TTY mode)

PROGRESS	:= $(BUILD_DIR)/.progress
TOTAL_OBJS	= $(words $(SRCS))

ifeq ($(MAKELEVEL),0)
$(shell rm -f $(PROGRESS) 2>/dev/null)
endif

define _progress
	@( \
		flock 9; \
		n=$$(($$(cat $(PROGRESS) 2>/dev/null || echo 0) + 1)); \
		echo $$n > $(PROGRESS); \
		total=$(TOTAL_OBJS); \
		msg="$(1)"; \
		if [ "$(UNWIND)" != "1" ] && [ -t 1 ]; then \
			if [ $$n -lt $$total ]; then \
				printf "\r[%0$${#total}d/%d] %s\033[K" $$n $$total "$$msg"; \
			else \
				printf "\r[%0$${#total}d/%d] %s\033[K\n" $$n $$total "$$msg"; \
			fi; \
		else \
			printf "[%0$${#total}d/%d] %s\n" $$n $$total "$$msg"; \
		fi; \
	) 9>>$(PROGRESS)
endef

# Terminate the progress line if the last compile did not (incremental
# build in TTY mode where fewer than TOTAL_OBJS files were rebuilt).
define _progress_end
	@n=$$(cat $(PROGRESS) 2>/dev/null || echo 0); \
	if [ "$(UNWIND)" != "1" ] && [ -t 1 ] && [ $$n -gt 0 ] && [ $$n -lt $(TOTAL_OBJS) ]; then \
		printf '\n'; \
	fi; \
	rm -f $(PROGRESS)
endef

define _compile
	$(call _progress,$(1))
	@$(if $(filter 1,$(SHOW_FLAGS)),echo '  $(2)' && )$(2)
endef

# Compile .c to .o
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(call _compile,CC $<,$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@)

# Compile .S (preprocessed assembly) to .o
$(OBJDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(call _compile,AS $<,$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@)

# Per-file overrides go below, e.g.:
# $(OBJDIR)/src/arch/foo.o: CFLAGS += -march=armv8-a+lse

# Auto-generated header dependencies (-MMD -MP).
-include $(DEPS)
