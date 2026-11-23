# sources.mk - source lists, discovered from the layout in config.mk.

RUNNER_SRCS		:= $(sort $(wildcard $(RUNNER_DIR)/*.c))
RUNNER_HDRS		:= $(wildcard $(INC_DIR)/runner/*.h)

# cov.c is only linked with COV=1 (see tests.mk).
FW_SRCS			:= $(FW_DIR)/lc.c $(FW_DIR)/sys.c
FW_COV_SRC		:= $(FW_DIR)/cov.c
FW_HDRS			:= $(wildcard $(INC_DIR)/framework/*.h)

SUITES			:= $(sort $(wildcard $(SUITES_DIR)/*/*.c))
SPECS			:= $(sort $(wildcard $(SPEC_DIR)/*.spec))

# Every C file of the project, for the tooling (tidy, format, headers).
ALL_C_FILES		= $(RUNNER_SRCS) $(FW_SRCS) $(FW_COV_SRC) $(SUITES) \
	$(RUNNER_HDRS) $(FW_HDRS)
