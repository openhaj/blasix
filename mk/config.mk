# config.mk - project configuration.
#
# This is the ONLY file you should need to edit when starting a new
# project from this template. The shell scripts in scripts/ read
# PROJECT_NAME and LICENSE_SPDX from here too.

# Human-readable project name (used in license headers).
PROJECT_NAME	:= blahaj-template

# Project version (semver).
VERSION		:= 0.1.0

# What to build:
#   exec   : an executable named $(PROJECT_NAME)
#   static : a static library named lib$(PROJECT_NAME).a
#   shared : a shared library named lib$(PROJECT_NAME).so
TYPE		:= exec

# SPDX identifier written in license headers.
LICENSE_SPDX	:= GPL-3.0-or-later

# C standard (keep in sync with .clangd).
STD		:= c11

# Source / header directories, and build output root
# (objects go in $(BUILD_DIR)/<mode>/).
SRC_DIR		:= src
INC_DIR		:= include
BUILD_DIR	:= objs
