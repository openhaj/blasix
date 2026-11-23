#!/bin/sh
# scripts/project.sh - shared project settings for the other scripts.
#
# Source it, do not execute it:
#   . "$(dirname "$0")/project.sh"
#
# Defines:
#   REPO_ROOT, PROJECT_NAME, LICENSE_SPDX, AUTHOR_NAME, AUTHOR_EMAIL
#
# PROJECT_NAME and LICENSE_SPDX are read from mk/config.mk, unless
# already set in the environment.
#
# Author info is read from (in order of priority):
#   1. Environment variables AUTHOR_NAME / AUTHOR_EMAIL
#   2. git config header.author.name / header.author.email
#   3. git config user.name / user.email

REPO_ROOT=$(git rev-parse --show-toplevel 2>/dev/null || pwd)
CONFIG_MK="$REPO_ROOT/mk/config.mk"

# Read `VAR := value` (or =, ?=) from mk/config.mk.
config_get() {
    [ -f "$CONFIG_MK" ] || return 0
    sed -n "s/^$1[[:space:]]*[:?]*=[[:space:]]*//p" "$CONFIG_MK" \
        | sed 's/[[:space:]]*#.*$//; s/[[:space:]]*$//' | head -1
}

: "${PROJECT_NAME:=$(config_get PROJECT_NAME)}"
: "${PROJECT_NAME:=$(basename "$REPO_ROOT")}"
: "${LICENSE_SPDX:=$(config_get LICENSE_SPDX)}"
: "${LICENSE_SPDX:=GPL-3.0-or-later}"

: "${AUTHOR_NAME:=$(git config header.author.name 2>/dev/null || git config user.name 2>/dev/null || echo 'Unknown')}"
: "${AUTHOR_EMAIL:=$(git config header.author.email 2>/dev/null || git config user.email 2>/dev/null || echo 'unknown@example.com')}"
