# runner.mk - lcrun, built with the host libc.

RUNNER		:= $(BUILD_DIR)/lcrun
RUNNER_OBJS	:= $(patsubst $(RUNNER_DIR)/%.c,$(BUILD_DIR)/runner/%.o,$(RUNNER_SRCS))
RUN_CFLAGS	:= -std=$(STD_HOST) -O2 -g $(WARNINGS)

$(eval $(call c_objects,$(BUILD_DIR)/runner,$(RUNNER_DIR),HOST_CC,RUN_CFLAGS,HOSTCC))

$(RUNNER): $(RUNNER_OBJS)
	$(call log,HOSTLD,$@)
	$(AT)$(HOST_CC) $^ -o $@

DEPS		+= $(RUNNER_OBJS:.o=.d)
