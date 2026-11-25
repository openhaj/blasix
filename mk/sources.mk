# sources.mk - source lists, written out explicitly (no wildcard).
# The test units are in suites.mk.

# ----- lcrun (host libc) -----

RUNNER_SRCS		:= \
	cli.c \
	main.c \
	util.c \
	report/compare.c \
	report/conformance.c \
	report/console.c \
	report/export.c \
	run/discover.c \
	run/cache.c \
	run/exec.c \
	run/virtual.c \
	spec/database.c \
	spec/require.c \
	spec/standards.c \
	spec/visibility.c \
	tools/check.c \
	tools/coverage.c \
	tools/list.c \
	tools/make.c \
	tools/matrix.c \
	tools/probes.c \

RUNNER_HDRS		:= \
	cli.h \
	lcrun.h \
	report.h \
	run.h \
	spec.h \
	tools.h \
	util.h \

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

# <category>/<group>/<unit>: the source paths, as lcrun prints them
# (LC_UNITS in spec.mk) and as bin/ is laid out.
SUITE_UNITS		:= $(foreach c,$(CATEGORIES),$(addprefix $(c)/,$($(c)_UNITS)))
SUITES			:= $(addprefix $(SUITES_DIR)/,$(addsuffix .c,$(SUITE_UNITS)))
SUITE_HDRS		:= $(foreach c,$(CATEGORIES),$(addprefix $(SUITES_DIR)/$(c)/,$($(c)_HDRS))) \
	$(addprefix $(SUITES_DIR)/bits/,$(BITS_HDRS))
SPECS			:= $(addprefix $(SPEC_DIR)/,$(addsuffix .spec,$(CATEGORIES)))

# Every C file of the project, for the tooling (tidy, format, headers).
ALL_C_FILES		= $(RUNNER_SRCS) $(FW_SRCS) $(FW_COV_SRC) $(SUITES) $(SUITE_HDRS) \
	$(RUNNER_HDRS) $(FW_HDRS)
