# blåsix

Conformance tests for C libraries, driven by the specification.

There is no reference libc: what a libc must provide and how it must
behave comes from ISO C and POSIX.1. Any libc can be checked against any
edition: glibc, musl, hajlibc, ...

## Usage

```sh
make run                                     # host libc, POSIX.1-2024 + XSI
make run LIBC=hajlibc                        # hajlibc (HAJLIBC=path)
make run LIBC=musl                           # musl
make run LIBC=hajlibc STD=posix-2008 OPTS=   # another edition, no XSI
make run CAT=string FUNC=strlen,strcmp       # only some functions
make run-all                                 # the 8 standards, one line each
make compare A=host B=hajlibc                # same target, two libcs
make coverage LIBC=hajlibc                   # line coverage of the libc sources
make spec-list STD=c11                       # what C11 requires
make matrix                                  # tested / required, 8 standards
make help                                    # all targets and options
```

## The target

A run checks one libc against one target:

| Variable | Values | Default |
| --- | --- | --- |
| `STD` | `c99 c11 c17 c23 posix-2001 posix-2008 posix-2017 posix-2024` | `posix-2024` |
| `OPTS` | option groups, comma list (`make options`): POSIX ones (`XSI`, `TSF`, ...) and ISO C features (`THREADS`, `COMPLEX`, `LIB_EXT1`, ...) | `XSI` on POSIX target, none on ISO C |
| `EXT` | `1`: also the GNU / BSD extensions | `0` |
| `LAX` | `1`: lax mode, do not check what the headers must hide (and expose everything the libc has, `LIBC_LAX_CFLAGS`) | `0` (strict) |

The target sets the feature-test macros the tests are compiled with,
which interfaces are required, which tests run, and (strict mode) which
names the headers must not make visible.

Other options: `TEST='glob'`, `V=1` (verbose), `Q=1` (quiet),
`REPEAT=n` (flakiness), `TIMEOUT=s`, `JOBS=n` (parallel jobs, for the
build and the tests; default: the number of CPUs; `make -jN` sets the
build jobs too), `PROBE=0`. On a terminal, the build and the run show a
progress bar (units and probes built, tests done and failed so far).

## Results

A run reports, for the chosen libc and target:

- **probes**: whether each required interface is declared by the
  header (`undeclared`) and defined by the library (`unlinked`), and
  in strict mode whether a name the target forbids is visible anyway
  (`leak`);
- **tests**: each runs in its own process: `pass`, `fail`, `skip`,
  `crash`, `timeout`, `error`, or `unavailable` (did not build);
- **conformance per header**: how many required interfaces are present
  and verified, with the lists of missing, failing and untested ones.

Results go to `build/<libc>/<target>/` (`results.tsv`, `results.json`),
so several libcs and targets can live side by side.

## Testing your own libc

Write a profile `mk/libc/<name>.mk` that says how to build against it,
add `<name>` to `LIBC_PROFILES` in `mk/config.mk`, then run
`make run LIBC=<name>`. For a libc installed under
`/opt/mylibc`, built with `-nostdlib`:

```make
# mk/libc/mylibc.mk
MYLIBC          ?= /opt/mylibc

LIBC_CC         := cc
LIBC_CPPFLAGS   := -nostdinc -isystem $(MYLIBC)/include
LIBC_CFLAGS     :=
LIBC_LDFLAGS    := -nostdlib -static $(MYLIBC)/lib/crt1.o
LIBC_LIBS       := $(MYLIBC)/lib/libc.a -lgcc
LIBC_DEPS       := $(MYLIBC)/lib/libc.a

libc-prepare:
    @true
```

```sh
make run LIBC=mylibc                         # or MYLIBC=/elsewhere
make compare A=host B=mylibc                 # what differs from the host libc
```

| Variable | Meaning |
| --- | --- |
| `LIBC_CC` | compiler driver (`cc`, or a wrapper such as `musl-gcc`) |
| `LIBC_CPPFLAGS` | include paths of the libc |
| `LIBC_CFLAGS` | flags every file needs; **no `-std` or feature macros**, the target sets them |
| `LIBC_LDFLAGS` / `LIBC_LIBS` | link flags / libraries (after the objects) |
| `LIBC_DEPS` | files whose change rebuilds the tests (e.g. `libc.a`) |
| `libc-prepare` | recipe run before building (e.g. build the libc, or check it exists) |
| `LIBC_LAX_CFLAGS` | optional, `LAX=1` only: macros that expose everything (e.g. `-D_DEFAULT_SOURCE`) |

`mk/libc/host.mk` is a minimal example,
`mk/libc/hajlibc.mk` builds the libc itself and supports coverage
(`LIBC_COV_*`, see [CONTRIBUTING.md](CONTRIBUTING.md)).

## Contributing

To write tests, edit the spec database, or add a libc, an edition or an
architecture, see [CONTRIBUTING.md](CONTRIBUTING.md).

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE, see
[LICENSE](LICENSE).
