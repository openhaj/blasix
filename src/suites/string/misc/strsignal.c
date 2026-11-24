/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strsignal.c
 * @brief Tests for strsignal() (<string.h>).
 * @Created: 2026/11/24 05:24:35 by Moutig
 * @Updated: 2026/11/24 06:13:21 by Moutig
 *
 * strsignal() (POSIX.1-2008) maps a signal number to an
 * implementation-defined string; the result for an invalid number is
 * unspecified (not tested).
 *
 * POSIX: XSH strsignal() (CX). The signal numbers come from <signal.h>.
 */
#include <signal.h>
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strsignal() SYNOPSIS")
{
	EXPECT_TYPE(&strsignal, char *(*)(int));
}

TEST_OPT(string_for_valid_signals, .ref = "POSIX strsignal() RETURN VALUE")
{
	static const int	sig[] = {SIGINT, SIGTERM, SIGSEGV, SIGKILL};

	for (int i = 0; i < 4; i++)
	{
		const char	*m = strsignal(sig[i]);

		ASSERT_NOTNULL(m);
		EXPECT_GT(strlen(m), 0);
	}
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strsignal() SYNOPSIS")
{
	EXPECT_NOTNULL((strsignal)(SIGINT));
}
