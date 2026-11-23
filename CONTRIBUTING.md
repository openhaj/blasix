# Contributing to blåsix

What a libc must provide and how it must behave is written down in
`spec/` (which interfaces exist, since which edition, in which option
group) and in `src/suites/` (how they behave, each test citing the clause
it checks).

## Layout

```tree
spec/              interfaces per header, per edition and option group
src/suites/        the tests: src/suites/<header>/<function>.c
src/framework/     test side: lc.c (checks, protocol, main),
                   sys.c (raw syscalls), cov.c (coverage, COV=1 only)
src/runner/        lcrun (host libc): spec.c (editions, database, targets),
                   tools.c (make fragment, probes, spec-list, spec-check),
                   main.c (selection, discovery), exec.c (process pool),
                   report.c (report, TSV, JSON, compare)
include/framework/ lc.h (test API), sys.h
include/runner/    lcrun.h
mk/                build modules: config.mk (layout), sources.mk,
                   build.mk (compile helpers), runner.mk, target.mk,
                   tests.mk, run.mk, tooling.mk
mk/libc/           one profile per libc: host.mk, hajlibc.mk, musl.mk
templates/         test.c (make new)
scripts/           new-test.sh, cov-report.sh, header*.sh
```

## The target in detail

The target decides everything:

- **the feature-test macros**: every test and probe is compiled with
  the `-std=` and `_POSIX_C_SOURCE` / `_XOPEN_SOURCE` of the target (for
  POSIX.1-2024 + XSI: `-std=c17 -D_POSIX_C_SOURCE=202405L
  -D_XOPEN_SOURCE=800`) and nothing else. A header that hides a required
  interface under these macros is non-conforming, and it shows;
- **which interfaces are required** (from `spec/`);
- **which tests run**: units whose function the target does not require
  are not built; a test whose `.since` / `.until` / `.opt` excludes the
  target is not run.

### The 8 standards

| ISO C | POSIX.1 (C base) |
| --- | --- |
| `c99` C99 | `posix-2001` POSIX.1-2001 (C99) |
| `c11` C11 | `posix-2008` POSIX.1-2008 (C99) |
| `c17` C17 | `posix-2017` POSIX.1-2017 (C99) |
| `c23` C23 | `posix-2024` POSIX.1-2024 (C17) |

The texts are in `data/` (not versioned, see `data/README.md`); every
line of `spec/` and every `.ref` of a test comes from them.

### Option groups

`make options` lists them. Two families:

- **POSIX options**, the margin codes of XBD 1.7.1 (`XSI`, `TSF`, `ADV`,
  `THR`, ...): only on a POSIX target; an interface or a behaviour of
  an option is required only when the option is selected (`OPTS=`,
  default `XSI` on a POSIX target). `CX` (extension to ISO C) and `OB`
  (obsolescent) are not options: `CX` is base POSIX, `OB` is `ob=`.
- **ISO C conditional features** (`COMPLEX`, `THREADS`, `ATOMICS`,
  `LIB_EXT1`, `IEC_60559`, `DFP`, `IEC_60559_TYPES`): each has a `from`
  edition. Before it the feature is required (complex arithmetic is
  mandatory in C99), from it on only when selected; selecting
  `LIB_EXT1` also defines `__STDC_WANT_LIB_EXT1__`.

A POSIX edition includes an ISO C edition: POSIX.1-2001, 2008 and 2017
are built on C99, POSIX.1-2024 on C17. So `aligned_alloc` (C11) is
required by POSIX.1-2024 but not by POSIX.1-2008, and `gets` (removed in
C11) is required, obsolescent, by POSIX.1-2008 but gone from POSIX.1-2024.

## The spec database: `spec/<header>.spec`

One file per header (`sys/stat.h` -> `spec/sys_stat.spec`), one line per
interface and per ladder:

```C
header string.h

# name        since          tags
memcpy        C99
NULL          C99            macro
size_t        C99            type
memccpy       C23
memccpy       POSIX.1-2001   XSI
strdup        C23
strdup        POSIX.1-2001   XSI base=POSIX.1-2008
strtok_r      POSIX.1-2001   TSF base=POSIX.1-2008
strlcpy       POSIX.1-2024
strcpy_s      C11            LIB_EXT1
memrchr       EXT            ext=GNU
```

- `since`: the edition that introduced it (`C99` ... `C23`,
  `POSIX.1-2001` ... `POSIX.1-2024`, `make standards`), or `EXT` for an
  extension. ISO C interfaces older than C99 are `C99` (no older text in
  `data/`).
- **Two ladders**: when ISO C and POSIX do not tell the same story
  (`memccpy`: POSIX XSI since 2001, ISO C since C23), the interface gets
  one ISO C line and one POSIX line; it is required when either line
  requires it. One line is enough when POSIX simply includes the ISO C
  interface (`memcpy`).
- kind: `macro`, `type`, `either` (a macro or a function, unspecified:
  `setjmp`, `errno`, the generic functions of `<stdatomic.h>`), `var`
  (an object: `optarg`, `environ`); default function.
- tags: one option group (`make options`), `base=V` (moved from its
  option group to the base in V), `ob=V[,W]` (obsolescent from V, or W:
  `asctime` is `ob=POSIX.1-2008,C23`), `removed=V[,W]`, `ext=NAME`
  (extension family).

For a target, each interface is **required** (base), **required by an
option** (its group is in `OPTS` and it has not moved to the base yet),
an **extension** (`EXT=1`), or excluded: not yet, removed, option off.
Probes: a function, `either` or `var` must be declared and link; a
macro must be `#define`d; a type must be usable.
`make spec-list` prints the decision for every interface (`V=1`: the
excluded ones too); `make spec-check` lints the database and checks
that every test file has an entry.

Only add data you can cite. Uncertain cases stay as `# TODO` comments
until checked against the text of the standard.

## What a run measures

### Probes

For every required interface, a tiny program is generated
(`build/<libc>/<target>/probe-src/`) and built against the libc:

| Probe | Meaning |
| --- | --- |
| `ok` | the header declares it and the library defines it |
| `undeclared` | the header does not declare it under the target's macros (a macro: not `#define`d; a type: unknown) |
| `unlinked` | declared, but the library does not define it |

Functions are probed as `(name)`, which also catches a function-like
macro without the real function behind it. Interfaces that ISO C defines
as macros, such as `stdin`, must be macros.

### Tests

Each test runs in its own process. Status: `pass`, `fail` (a check
failed), `skip`, `crash` (signal), `timeout`, `error` (the process exited
before the end of the test), `unavailable` (the unit did not build
against this libc).

### The report

1. failures, with the spec reference of the test, the failed checks
   and the test's captured stdout / stderr;
2. units that did not build, with the compiler message;
3. results per header (per function with `V=1`): tests, pass, fail,
   crash, timeout, error, skip, checks, CPU time;
4. **conformance per header** for the target: `req` (required; `opt`
   by an option group, `ob` obsolescent), `ok` / `undec` / `unlnk`
   (probes), `tested`, `pass`, `fail`, `present` (ok / req) and
   `verified` (tested with every test passing / req), with the lists of
   undeclared, unlinked, failing and untested interfaces (`V=1`: in
   full);
5. slowest tests, peak RSS, flaky tests (`REPEAT=n`);
6. totals.

Every run writes `results.tsv` (tests, plus one `@interface` row per
required interface; used by `make compare`) and `results.json` (every
field, the target, every interface with its requirement and probe).

## Writing tests

### One file per function

```sh
src/suites/<header>/<function>.c        src/suites/string/strlen.c
```

The directory is the spec file name (`string` for `spec/string.spec`,
`<string.h>`); the function must have an entry there. Create a file
with:

```sh
make new CAT=string FUNC=strchr                 # entry already in spec/
make new CAT=string FUNC=strchrnul SINCE=EXT    # also adds the entry
```

### The file

The category and the function come from the path, so a file only holds
tests. Each test checks one property and says where the spec states it:

```c
#include "framework/lc.h"
#include <string.h>

TEST(empty)
{
    EXPECT_UEQ(strlen(""), 0);
}

TEST_OPT(compares_as_unsigned_char, .ref = "C11 7.24.4p1")
{
    EXPECT_EQ(LC_SIGN(strcmp("\x80", "\x7f")), 1);
}
```

Rules:

- **Test the spec, not an implementation.** Only what the standard
  guarantees (`strcmp` returns a sign, not `-1`; `errno` only where the
  standard sets it).
- **Depend only on the function under test.** Prepare data and clean up
  with the framework helpers (`lc_fill`, `lc_unlink`, `lc_tmpPath`,
  `lc_exitNow`), not with other libc functions: a missing `remove()`
  must not make the `fopen` tests unavailable.
- **Mind the edition.** A behaviour that only exists from some edition
  on gets `.since` (the `x` mode of `fopen`: `.since = "C11"`); a POSIX
  requirement on an ISO C function gets `.since = "POSIX.1-2001"` (the
  `errno` values of `fopen`), so it is not run against a pure ISO C
  target.

### Options

```c
TEST_OPT(name,
    .ref     = "POSIX fopen(): ERRORS [EINVAL]",  /* shown on failure */
    .since   = "POSIX.1-2008",                    /* required from */
    .until   = "POSIX.1-2024",                    /* removed in */
    .opt     = "XSI",                             /* only with this option */
    .expect  = LC_SIGNAL(SIGABRT),                /* or LC_EXIT(3) */
    .timeout = 30)
```

An unknown version or option in `.since` / `.until` / `.opt` is an
error: lcrun reports it and the run fails (a typo must not silently drop
the test).

`TEST_FN(other_function, name)` attaches a test to another function than
the file's (e.g. `getc_unlocked` cases in `getc.c`).

### Checks

| Macro | Checks | Message on failure |
| --- | --- | --- |
| `EXPECT_TRUE(c)` / `EXPECT_FALSE(c)` | condition | `EXPECT_TRUE(x > 0): failed` |
| `EXPECT_EQ NE LT LE GT GE (a, b)` | signed integers | `EXPECT_EQ(f(), 3): 2 == 3 is false` |
| `EXPECT_UEQ UNE ULT UGT (a, b)` | unsigned integers (`size_t`) | same, unsigned |
| `EXPECT_PTREQ PTRNE (a, b)`, `EXPECT_NULL(p)`, `EXPECT_NOTNULL(p)` | pointers | both addresses |
| `EXPECT_STREQ STRNE (a, b)` | strings, NULL-safe | both quoted + first difference |
| `EXPECT_MEMEQ(a, b, n)` | `n` bytes | first differing byte |

Each `EXPECT_*` has an `ASSERT_*` twin that also leaves the test body
(only directly in a `TEST` body). `LC_SIGN(x)`, `SKIP("reason")` and
`LOG("message")` complete the set.

### What to cover, function by function

1. every statement of the function's DESCRIPTION, one test each;
2. RETURN VALUE: success and each failure;
3. ERRORS: each `errno` the standard lists, with the `.since` of the
   POSIX edition when the function comes from ISO C;
4. limits: empty input, 0, 1, maximum, minimum;
5. memory and string functions: every size and every misalignment of
   every pointer argument, inputs that end right before a page boundary;
6. what must not change (bytes past the end, arguments);
7. when the behaviour is a pure function of a small input, every input
   against a reference written from the spec (`ctype/*.c`, `all_values`).

### License headers

Every `.c` / `.h` file starts with a license block and a Doxygen block
(`@file`, `@brief`, `@Created`, `@Updated`). `make new` fills them in;
`make headers-add` adds them to a file that has none, `make
headers-check` checks them, and `make init` enables the pre-commit hook
that runs the check.

### Debugging a test

A test binary also runs alone, in-process and without isolation (for a
debugger): `build/host/posix-2024+XSI/bin/string/strlen [--list | --run
strlen empty]`.

## Coverage

`make coverage LIBC=hajlibc` rebuilds hajlibc with coverage counters,
runs the tests, merges the counters of every test process and prints the
line coverage per source directory (`coverage/lines.tsv` per file;
`index.html` with `gcovr`). hajlibc is rebuilt without instrumentation
afterwards.

The system libgcov runtime is built against glibc and cannot link with
another libc, so it is not used (`src/framework/cov.c`). Needs GCC ≥ 12 and
`gcov-tool`. Tests that end in `exit` or a signal are not counted.

## Design

- **The framework does not depend on the libc under test**: raw
  `write(2)`, tests registered in the `lc_tests` ELF section (no
  constructor), its own formatting. `nm -u
  build/<libc>/<target>/fw/lc.o` only lists the framework's own symbols.
- **lcrun is built with the host libc**: processes, timeouts, parsing
  and statistics live outside the libc under test.
- **One process per test, one binary per function**: crashes are
  isolated, a missing function is a result, not a build failure.

### Adding a libc

Write `mk/libc/<name>.mk` (contract in `host.mk`): `LIBC_CC`,
`LIBC_CPPFLAGS`, `LIBC_CFLAGS` (no `-std`, no feature macros: the target
sets them), `LIBC_LDFLAGS`, `LIBC_LIBS`, `LIBC_DEPS`, a `libc-prepare`
recipe, optionally the `LIBC_COV_*` hooks. Then `make run LIBC=<name>`.

### Adding an edition

Add it to the table at the top of `src/runner/spec.c` (C base, POSIX level,
`_POSIX_C_SOURCE`, `_XOPEN_SOURCE`, `-std=`), to `STANDARDS` in
`mk/config.mk`, then use its name in `spec/`. An option group goes in
`gOpts` in the same file.

### Following the progress

`plan.md` gives the order of the headers. While working on one:

```sh
make spec-list STD=c11 V=1        # what each target says of each interface
make todo CAT=string              # required by the target, no unit yet
make matrix                       # tested / required, header x 8 standards
make run-all                      # the 8 standards, one result line each
```

### Adding an architecture

Add its syscall numbers and the `lc_syscall3` / `lc_syscall6` asm in
`src/framework/sys.c` (x86_64 and aarch64 Linux are supported).
