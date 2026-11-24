# sources.mk - source lists, written out explicitly (no wildcard).
# The test units are in suites.mk.

# ----- lcrun (host libc) -----

RUNNER_SRCS		:= \
	exec.c \
	main.c \
	report.c \
	spec.c \
	tools.c \

RUNNER_HDRS		:= \
	lcrun.h \

# ----- Framework (libc under test) -----

# cov.c is only linked with COV=1 (see tests.mk).
FW_SRCS			:= \
	lc.c \
	sys.c \

FW_COV_SRC		:= \
	cov.c \

FW_HDRS			:= \
	lc.h \
	sys.h \

# Prefix each list with its directory.
RUNNER_SRCS		:= $(addprefix $(RUNNER_DIR)/,$(RUNNER_SRCS))
RUNNER_HDRS		:= $(addprefix $(INC_DIR)/runner/,$(RUNNER_HDRS))
FW_SRCS			:= $(addprefix $(FW_DIR)/,$(FW_SRCS))
FW_COV_SRC		:= $(addprefix $(FW_DIR)/,$(FW_COV_SRC))
FW_HDRS			:= $(addprefix $(INC_DIR)/framework/,$(FW_HDRS))

# ----- Test units and specs (from suites.mk) -----

include mk/suites.mk

# <category>/<unit>, as lcrun names them (LC_UNITS in spec.mk).
SUITE_UNITS		:= $(foreach c,$(CATEGORIES),$(addprefix $(c)/,$($(c)_UNITS)))
SUITES			:= $(addprefix $(SUITES_DIR)/,$(addsuffix .c,$(SUITE_UNITS)))
SUITE_HDRS		:= $(foreach c,$(CATEGORIES),$(addprefix $(SUITES_DIR)/$(c)/,$($(c)_HDRS)))
SPECS			:= $(addprefix $(SPEC_DIR)/,$(addsuffix .spec,$(CATEGORIES)))

# Every C file of the project, for the tooling (tidy, format, headers).
ALL_C_FILES		= $(RUNNER_SRCS) $(FW_SRCS) $(FW_COV_SRC) $(SUITES) $(SUITE_HDRS) \
	$(RUNNER_HDRS) $(FW_HDRS)
