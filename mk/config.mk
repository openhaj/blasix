# config.mk - project configuration and layout.
#
# The shell scripts in scripts/ read PROJECT_NAME and LICENSE_SPDX from
# here too (scripts/project.sh).

PROJECT_NAME	:= blasix
VERSION			:= 0.1.0
LICENSE_SPDX	:= GPL-3.0-or-later

# ----- Layout -----

SRC_DIR			:= src
INC_DIR			:= include
SPEC_DIR		:= spec
SUITES_DIR		:= $(SRC_DIR)/suites
FW_DIR			:= $(SRC_DIR)/framework
RUNNER_DIR		:= $(SRC_DIR)/runner
BUILD_DIR		:= build

# ----- Host toolchain (lcrun, tooling) -----

HOST_CC			?= cc
# C standard of lcrun and the framework (keep in sync with .clangd).
STD_HOST		:= c11
JOBS			?= $(shell nproc 2>/dev/null || echo 4)

# The libc profiles of mk/libc/ (make run LIBC=<name>).
LIBC_PROFILES	:= host hajlibc

# The 8 standards (make standards; make run-all runs every one).
STANDARDS		:= c99 c11 c17 c23 posix-2001 posix-2008 posix-2017 posix-2024

comma			:= ,
