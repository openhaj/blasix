# Contributing to blåsix

What a libc must provide and how it must behave is written down in
`spec/` (which interfaces exist, since which edition, in which option
group) and in `src/suites/` (how they behave, each test citing the clause
it checks).

## Layout

```tree
spec/              interfaces per header, per edition and option group
src/suites/        the tests: <header>/<group>/<function>.c (functions),
                   <header>/defs.c (its types and constants),
                   <header>/<header>_cases.h (cases of the category),
                   bits/<origin>/<symbol>.h (cases of every type and
                   constant: size_t, NULL, locale_t, PRId8, ...)
src/framework/     test side: lc.c (checks, protocol, main),
                   sys.c (raw syscalls), cov.c (coverage, COV=1 only)
src/runner/        lcrun (host libc): spec.c (editions, database, targets),
                   tools.c (make fragment, probes, spec-list, spec-check),
                   main.c (selection, discovery), exec.c (process pool),
                   report.c (report, TSV, JSON, compare)
include/framework/ lc.h (test API), sys.h
include/runner/    lcrun.h
mk/                build modules: config.mk (layout), sources.mk and
                   suites.mk (every compiled file, listed: no wildcard),
                   build.mk (compile helpers), runner.mk, target.mk,
                   tests.mk, run.mk, tooling.mk
mk/libc/           one profile per libc: host.mk, hajlibc.mk
templates/         test.c (a unit), defs.c (a header's definitions),
                   spec.spec (a spec file, with its guide); make new uses them
scripts/           new-test.sh, cov-report.sh and gcov2lcov.awk (coverage),
                   sync-tree.sh (probe sources), header*.sh
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
  target is not run;
- **what the headers must hide** (strict mode, the default): see
  [Visibility: strict and lax](#visibility-strict-and-lax).

### Visibility: strict and lax

The standards also say what a header must **not** declare:

- ISO C (4p6, 7.1.3): a strictly conforming program may use every name
  that is not reserved, so a header may only add names reserved for it,
  the "future library directions" (C99 7.26, C11 / C17 7.31, C23 7.33:
  `str[a-z]*` in `<string.h>`, `is[a-z]*` in `<ctype.h>`, ...) and names
  with a leading underscore;
- POSIX (XSH 2.2.1): "Additional symbols not required or explicitly
  permitted by POSIX.1 to be in that header shall not be made visible,
  except when enabled by another feature test macro"; XSH 2.2.2 lists
  what each header may add (prefixes, suffixes: `_t` in any header);
- Annex K (K.3.1.1): with `__STDC_WANT_LIB_EXT1__` defined as 0 none of
  its identifiers is declared (reserved or not), with 1 they all are,
  undefined is implementation-defined (not checked).

For each target, every interface of `spec/` is then (`make spec-list
V=1`, column notes):

| Visibility | When | Checked by |
| --- | --- | --- |
| required | the target requires it | the declaration probe (`ok` / `undeclared` / `unlinked`) |
| may be visible | not required, name reserved for the header (`reserved` lines, leading `_`) | nothing |
| must be hidden | not required, not reserved; Annex K with the want macro 0 | the hidden probe: `leak` if visible |
| header not in target | no interface of the header is required (`<strings.h>` on an ISO C target) | nothing |

A removed interface follows the same rule: `bcmp` (removed in
POSIX.1-2008, no reserved prefix in `<strings.h>`) must be gone from
2008 on, while `isascii` (removed in 2024) may stay: `is[a-z]*` is
reserved for `<ctype.h>`.

Hidden probes (`probe-src/<cat>/<name>.hidden.c`) include the header
with the target's macros only (and `#define __STDC_WANT_LIB_EXT1__ 0`
for Annex K); one that compiles is a **leak**: a `leak` column and a
`leaked` list in the conformance table, `N leaked` on the last line,
and a failing run.

**Lax mode** (`LAX=1`, build directory `+lax`): visibility is not
checked, and the units and declaration probes are also compiled with
the profile's `LIBC_LAX_CFLAGS` (host: `-D_DEFAULT_SOURCE`). A function
the libc only shows as an extension (glibc: `memmem`, `strlcpy`, `ffsl`
under POSIX.1-2024) is then tested anyway. Strict mode is the
conformance result; lax mode answers "does it work, once visible?".

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
interface and per ladder. **Start from `templates/spec.spec`** (`make
new` copies it for a new category): its `#:` lines are the guide (what
to read in `data/`, what to write, in which order), removed in the copy;
its `#` lines are the skeleton every spec follows: the summary per
edition, the host deviations, the `header` line, the `reserved` lines
with their clauses, then the interfaces grouped as the header's page
groups them. A spec file holds data and its citations, not the format:
the format is described here.

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

# what the header may declare beyond the target (ISO C 7.31.13, XSH 2.2.2)
reserved C99           str[a-z]* mem[a-z]* wcs[a-z]*
reserved POSIX.1-2001  *_t
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
  (extension family), `same=H[,H2]` (other headers that declare it too).
- **Reserved names**: `reserved V pattern...` (fnmatch patterns), from
  edition V on: copy them from the future library directions of the
  header (C99 7.26, C11 / C17 7.31, C23 7.33; a header that includes
  another one, like `<inttypes.h>` and `<stdint.h>`, gets both) and from
  the XSH 2.2.2 table (POSIX prefixes and suffixes; `*_t` for every
  header), with their edition (`reserved C23 PRIB* SCNB*`). Leading
  underscores are built in. Without the right lines, interfaces of other
  editions are wrongly reported as leaks (or not checked).
- **Shared definitions**: when the standard says a header defines
  something "as described in" another header (`locale_t` in `<ctype.h>`,
  as in `<locale.h>`), tag it `same=locale.h`: its probe includes both
  headers and fails if they disagree (a type: the two types must be
  compatible; a macro: the same integer value; a function or variable:
  no conflicting redeclaration). Each header still lists it in its own
  spec file, so that each header is checked to provide it.

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

## What is rebuilt

`make run` rebuilds only what changed since the last build of the same
libc and target (`build/<libc>/<target>/`):

- **a unit** when its source, a header it includes (`<bin>.d`, from
  `-MMD`), the framework or the library changes, or its command line
  (`<bin>.cmd`: compiler, flags, libraries; rewritten only when it
  differs). Adding a unit or editing a spec does not rebuild the others;
- **a unit that did not build** leaves an empty, non-executable file
  next to its `.err`: it is not tried again until one of those inputs
  changes, and is still reported unavailable;
- **`spec.mk`** is regenerated when lcrun, a spec or a unit changes, but
  rewritten only if its content differs;
- **the probes**: see [Probes](#probes).

`make clean` (this libc) or `make fclean` (everything) rebuilds from
scratch.

### The result cache

lcrun keeps the last result of every test in
`build/<libc>/<target>/cache.tsv` (`-C`, `src/runner/run/cache.c`) and
does not run a test again while these did not change:

- **the content of its binary** (a hash): the test, the framework and,
  for a static libc, every libc object the unit links. A relink that
  gives the same binary keeps the results;
- **its timeout** (`.timeout`, `TIMEOUT=`);
- **the run-time libc**, for a shared one: the files of `LIBC_RUNTIME`
  (path, size, date, inode). A change drops the whole cache;
- **lcrun itself** (it decides the statuses): a new lcrun drops it too.

Only reproducible results are kept: `pass`, `fail`, `skip`,
`unimplemented`, `crash`. A `timeout` or an `error` runs again. Running a
subset (`CAT=`, `FUNC=`, `TEST=`) keeps the entries of the other units.
The report says how many results come from the cache, `-v` marks them,
and `results.json` has `"cached"` per test.

A test that depends on something outside its binary (a file in `/tmp`,
an installed locale, the time) is reused as long as its binary does not
change: `CACHE=0` runs everything. The cache is not used with
`REPEAT=n` (flakiness), nor for `make coverage` (the tests must run to
write their counters).

## What a run measures

### Probes

For every required interface, a tiny program is generated
(`build/<libc>/<target>/probe-src/`) and built against the libc:

| Probe | Meaning |
| --- | --- |
| `ok` | the header declares it and the library defines it |
| `undeclared` | the header does not declare it under the target's macros (a macro: not `#define`d; a type: unknown) |
| `unlinked` | declared, but the library does not define it (undefined symbol) |
| `broken` | declared, but the link failed for another reason (compiler crash, library built for coverage, ...): see `probe/<cat>/<name>.err` |
| `leaked` / `hidden` | hidden probe (strict mode): an interface the target forbids is visible / is not |
| `accepted` / `rejected` | reject probe: a translation unit the standard requires the implementation to refuse compiled / was refused |

A probe runs again only when its source, a header it includes or its
flags change; a declaration probe, which is also linked, also when the
library changes. The sources are regenerated in a new directory and
copied over only where they differ (`scripts/sync-tree.sh`), so editing
one spec file reruns the probes of that header only.

**Reject probes** check a "shall issue a diagnostic" of the standard: a
generated translation unit must fail to compile. They are generated by
lcrun from the spec, wherever they apply, in strict and lax mode:

- K.3.1.1p4: `__STDC_WANT_LIB_EXT1__` must be defined identically for
  all inclusions of Annex K headers, "as if a preprocessor error
  directive were used" otherwise. For every header with interfaces of
  an option that has a want macro (`LIB_EXT1`), when that option is
  selected, two probes (`__want_LIB_EXT1_1_0`, `__want_LIB_EXT1_0_1`)
  include the header twice with the macro changed in between. A new
  Annex K header (`<stdio.h>`, `<stdlib.h>`, ...) gets them as soon as
  its `LIB_EXT1` lines are in its spec.

A new kind of required diagnostic goes in `specRejects()` (spec.c: the
list) and `writeReject()` (tools.c: the source); the make rule, the
report (`nodiag` column, `accepted` list) and the TSV / JSON rows
(`@reject:<id>`) are generic.

Functions are probed as `(name)`, which also catches a function-like
macro without the real function behind it. Interfaces that ISO C defines
as macros, such as `stdin`, must be macros.

### Tests

Each test runs in its own process. Status: `pass`, `fail` (a check
failed), `crash` (signal), `timeout`, `error` (the process exited
before the end of the test), `skip` (`SKIP`: the environment does not
allow it), `unimplemented` (`NOT_IMPLEMENTED`: the libc lacks what the test
needs), `unavailable` (the unit did not build against this libc). The
first four are failures; `skip` and `unimplemented` did not run and count
neither as passed nor as failed.

Results that come from the spec and the probes, not from a unit, are
added once the tests ran (`src/runner/run/virtual.c`), so that the
tables, the totals and the exit status show them like any test:

| Result | Status |
| --- | --- |
| `<cat>/<name>/@not_in_target` | `skip`: the target does not require it, with why (removed in POSIX.1-2008, not yet, option off, extension) and whether it must be hidden |
| `<cat>/<name>/@hidden` | strict mode, an interface that must be hidden: `pass` when the header hides it, `fail` when it is visible anyway (a leak, with the rule) |
| `<cat>/<want>/@reject_<a>_<b>` | a reject probe: `pass` when refused, `fail` when it compiled |

Only for the headers of the target; `-c`, `-f`, `-t` filter them too. The
summary line says how many interfaces the target leaves out and how many
of the failures are leaks or missing diagnostics.

Each required interface then gets one verdict, the first that applies:

| Verdict | When |
| --- | --- |
| `missing` | not implemented: its probe says `undeclared` or `unlinked` (without probes: its unit did not build because of it) |
| `broken` | its probe says `broken` |
| `failing` | a test failed |
| `verified` | tests ran and every one passed |
| `unbuilt` | its unit did not build, for another reason (a build error) |
| `untested` | no test ran: none written yet, or all skipped / not implemented |

The console table and `results.json` (`interfaces[].verdict`,
`conformance`) come from the same computation (`interfaceResult()`,
`confHeader()` in `report/conformance.c`).

### The report

1. failures, with the spec reference of the test, the failed checks
   and the test's captured stdout / stderr;
2. units that did not build, why (`not implemented` or `build error`) and
   the error lines of the build log;
3. results per header (per function with `V=1`): tests, pass, fail,
   crash, timeout, error, skip, checks, CPU time;
4. **conformance per header** for the target: `req` (required; `opt`
   by an option group, `ob` obsolescent), `ok` / `undec` / `unlnk`
   (probes), `leak` (strict mode: forbidden and visible), `nodiag`
   (reject probes that compiled), `tested`,
   `pass`, `fail`, `present` (ok / req) and
   `verified` (tests ran and every one passed / req), with the lists of
   undeclared, unlinked, link error, failing, not built and untested
   interfaces (`V=1`: in full);
5. slowest tests, peak RSS, flaky tests (`REPEAT=n`);
6. totals.

Every run writes `results.tsv` (tests, plus one `@interface` row per
required interface, one `@hidden` row per hidden probe and one
`@reject:<id>` row per reject probe; used by
`make compare`) and `results.json` (every field, the target, the mode,
every interface with its requirement and probe, every hidden probe).

## Writing tests

### One file per function

```sh
src/suites/<header>/<group>/<function>.c   src/suites/string/misc/strlen.c
```

The directory is the spec file name (`string` for `spec/string.spec`,
`<string.h>`); the function must have an entry there. The **group** is a
folder that sorts the units of a big header (`string`: `copy`, `concat`,
`compare`, `search`, `misc`, `annex_k`; `inttypes`: `func`), at any
depth: it is only a folder, the unit is still `<header>/<function>` for
lcrun, the filters and the report. Use the groups of the header's page
(its subclauses). One file per interface whatever the group (`make
spec-check` reports a second one). Units are for **functions** (and
function-like macros: `_tolower`); the types and constants of the header
are in its `defs.c` (see below).
The category's shared header stays at its root
(`src/suites/string/string_cases.h`); units include it as
`"string/string_cases.h"` (`-I src/suites`). Create a file with:

```sh
make new CAT=string GROUP=search FUNC=strchr               # entry in spec/
make new CAT=string GROUP=search FUNC=strchrnul SINCE=EXT  # adds the entry
make new CAT=string FUNC=x                                 # lists the groups
make new CAT=string FUNC=size_t                            # a type: says what
                                                           # to add to defs.c
```

The makefiles use no wildcard: every compiled file is listed. `make new`
adds the unit (`<group>/<function>`) to `<category>_UNITS` in
`mk/suites.mk` (a new category
also gets its entry in `CATEGORIES`); a helper shared by the units goes
in `<category>_HDRS` by hand, a runner or framework file in
`mk/sources.mk`. A unit that is not listed is not built: `make
sources-check` (also run by `make spec-check`) reports any file of
`src/`, `include/` or `spec/` missing from the lists.

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
  `lc_exitNow`, `lc_guarded`: bytes followed by an inaccessible page,
  for "never examines more than n bytes"), not with other libc functions: a missing `remove()`
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

### Cases shared by a category

When every function of a header is tested the same way, write the
cases once in a header of the category, `src/suites/<cat>/<name>.h`, and
instantiate them from each unit (a change to it rebuilds the units
that include it).
`src/suites/ctype/ctype_cases.h` is the example: the oracle of the
classes (from XBD 7.3.1.1, built from character literals), the ISO C
relations between classes, and generators (`CT_CLASSIFIER_TESTS`,
`CT_CASEMAP_TESTS`, `CT_LOCALE_TESTS`) that each unit calls with its
function and class. Functions of other headers needed by a case
(`setlocale`, `newlocale`) are referenced weakly there, so a libc without
them still builds the units and those cases skip.

### Types and constants: `bits/` and `<header>/defs.c`

Every **type** and every **constant** (object-like macro) of a header is
tested by one unit per header, `src/suites/<header>/defs.c`, from cases
written once in `src/suites/bits/`, sorted by origin:

```tree
bits/stddef/    size_t.h NULL.h wchar_t.h       ("as described in <stddef.h>")
bits/stdint/    intmax_t.h uintmax_t.h
bits/locale/    locale_t.h, locale_t_same.h (includes <locale.h>)
bits/annex_k/   errno_t.h rsize_t.h, lib_ext1.h (generator of the *_s units)
bits/version/   __STDC_VERSION_STRING_H__.h, __STDC_VERSION_INTTYPES_H__.h
bits/inttypes/  imaxdiv_t.h, convert.h, wide.h, format.h (generators),
                pri.h scn.h (one block per PRI / SCN macro)
```

A `bits/` file is not a generator: including it instantiates its cases,
each block guarded by the symbol's requirement and reported under the
symbol (`TEST_SYM`):

```c
# if defined(LC_REQ_size_t) || defined(LC_DEFS_ALL)
TEST_SYM(size_t__unsigned, "size_t", is_unsigned, .ref = "C11 7.19p2")
{
	EXPECT_TRUE((size_t)-1 > 0);
}
# endif
```

`defs.c` includes the header, then the `bits/` files of its symbols, then
what only that header says, then the `bits/` files that include other
headers (`templates/defs.c`):

```c
#include "framework/lc.h"
#include <string.h>                 /* the header under test, nothing else */

#include "bits/stddef/size_t.h"
#include "bits/stddef/NULL.h"
#include "bits/locale/locale_t.h"

# if defined(LC_REQ_size_t) || defined(LC_DEFS_ALL)
TEST_SYM(size_t__string_lengths, "size_t", is_the_type_of_string_lengths,
	.ref = "C11 7.24.6.3p1")
{
	EXPECT_TYPE(strlen(""), size_t);
}
# endif

#include "bits/locale/locale_t_same.h"  /* last: includes <locale.h> */
```

How it is built and checked:

- lcrun gives make `LC_DEFS_<cat>`, the types and macros the target
  requires; the defs unit gets `-DLC_REQ_<name>` for those whose probe
  says the header declares them (all with `PROBE=0`). A missing symbol is
  reported `undeclared` by its probe and leaves the others running;
  `LC_DEFS_ALL` (`.clangd`) compiles every block in the editor;
- `FUNC=size_t` runs the defs units with only the cases of `size_t`;
- `make spec-check` fails when a type or constant of the spec has no
  `LC_REQ_<name>` in its `defs.c` (directly or in a `bits/` file it
  includes), when a type has a unit of its own, or when a `bits/` file
  marked `BITS_INCLUDES_OTHER_HEADERS` comes before an unmarked one;
- a macro with a unit of its own (`ctype/xsi/_tolower.c`) is a
  function-like macro: tested like a function. `make new` without `GROUP`
  sends a type or a macro to `defs.c` (created from `templates/defs.c`)
  and says which `bits/` file to include or write.

Rules:

- `defs.c` includes **only** the header under test before the cases:
  another header could make the symbol visible and the case would pass
  for a header that lacks it. A check against another header (`same=` in
  the spec, "as described in") is a `bits/` file of its own, marked
  `BITS_INCLUDES_OTHER_HEADERS`, included last; the `same=` probe checks
  compatibility at compile time too;
- a `bits/` file uses only its symbol (`size_t`, not `SIZE_MAX`), which
  every header that defines it provides; what needs more of the header
  (`INTMAX_MAX` in `<inttypes.h>`) goes in that header's `defs.c`;
- a new symbol: add `bits/<origin>/<symbol>.h` (listed in `BITS_HDRS`,
  `mk/suites.mk`), and include it from the `defs.c` of **every** header
  that defines it, the ones already done included.

### Checks

| Macro | Checks | Message on failure |
| --- | --- | --- |
| `EXPECT_TRUE(c)` / `EXPECT_FALSE(c)` | condition | `EXPECT_TRUE(x > 0): failed` |
| `EXPECT_EQ NE LT LE GT GE (a, b)` | signed integers | `EXPECT_EQ(f(), 3): 2 == 3 is false` |
| `EXPECT_UEQ UNE ULT UGT (a, b)` | unsigned integers (`size_t`) | same, unsigned |
| `EXPECT_PTREQ PTRNE (a, b)`, `EXPECT_NULL(p)`, `EXPECT_NOTNULL(p)` | pointers | both addresses |
| `EXPECT_STREQ STRNE (a, b)` | strings, NULL-safe | both quoted + first difference |
| `EXPECT_MEMEQ(a, b, n)` | `n` bytes | first differing byte |
| `EXPECT_TYPE(expr, type)` | type of `expr` compatible with `type` (prototypes: `EXPECT_TYPE(&isalpha, int (*)(int))`) | `failed` |

Each `EXPECT_*` has an `ASSERT_*` twin that also leaves the test body
(only directly in a `TEST` body). `LC_SIGN(x)`, `SKIP("reason")`,
`NOT_IMPLEMENTED("what")` and `LOG("message")` complete the set.

`SKIP` and `NOT_IMPLEMENTED` both leave the test without a result, for
different reasons:

- `NOT_IMPLEMENTED("setlocale() (<locale.h>)")`: the libc lacks something
  the test needs (a function referenced weakly that is NULL, a header or a
  macro that is missing). It is a gap of the libc, reported as such;
- `SKIP("no other locale installed")`: the libc may be fine, the
  environment does not allow the test (no locale installed, no guarded
  mapping, the execution character set is not ASCII).

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

`make coverage LIBC=hajlibc` copies hajlibc's sources to
`build/hajlibc/<target>/libc-cov/` and builds them with coverage counters
there (the hajlibc tree itself is never instrumented), runs the tests,
merges the counters of every test process, then runs gcov on **every
object of the library**: a file no test linked or ran counts as never
executed, so the percentages are over the whole library. It writes, in
`build/hajlibc/<target>/coverage/`:

- `lcov.info`: an lcov tracefile (lines, functions, branches), the
  sources named by their path in the hajlibc tree; written by
  `scripts/gcov2lcov.awk` from gcov's output, lcov itself is not needed;
- `lines.tsv`: per file, path, percentage of lines, lines;
- `html/`: genhtml's report, when lcov is installed.

It prints the line coverage per directory.

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
recipe, optionally `LIBC_LAX_CFLAGS` (lax mode: the macros that make its
headers expose everything, e.g. `-D_DEFAULT_SOURCE`) and the `LIBC_COV_*`
hooks. Add `<name>` to `LIBC_PROFILES`
in `mk/config.mk`, then `make run LIBC=<name>`.

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
