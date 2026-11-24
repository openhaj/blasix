/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) @YEAR@ @AUTHOR@ <@EMAIL@>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file @FUNC@.c
 * @brief Tests for @FUNC@() (<@CAT@.h>).
 * @Created: @DATE@ by @AUTHOR@
 * @Updated: @DATE@ by @AUTHOR@
 *
 * Spec: @REFS@
 *
 * Checklist (delete what does not apply):
 *   - nominal values
 *   - limits: empty, 0, 1, max, min, NULL where the API allows it
 *   - every size / alignment for memory and string functions
 *   - errno and return value on each documented error
 *   - what must NOT change (bytes past the end, unchanged arguments)
 */

#include "framework/lc.h"
#include <@CAT@.h>

TEST_OPT(nominal, .ref = "@SPEC@")
{
	SKIP("not written yet");
}
