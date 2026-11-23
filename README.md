# Blahaj-C-Template

Template for C projects.

## Getting started

1. Edit `mk/config.mk`: `PROJECT_NAME`, `TYPE` (`exec` / `static` / `shared`),
   `VERSION`, `LICENSE_SPDX`, `STD`.
2. Put sources in `src/`, public headers in `include/`. Sources are
   discovered automatically, or list them in `mk/sources.mk`.
3. `make init` to enable the git hooks (header check on commit).
4. `make help` for all targets.

## Common targets

| Target | Description |
| --- | --- |
| `make` / `make debug` / `make asan` | Build (release / `-O0 -g3` / ASan+UBSan) |
| `make run ARGS="..."` | Build and run (exec only) |
| `make clean` / `fclean` / `re` | Clean objects / everything / rebuild |
| `make install PREFIX=... DESTDIR=...` | Install |
| `make compdb` | Generate `compile_commands.json` (needs `bear`) |
| `make tidy` / `make format` | clang-tidy / clang-format |
| `make headers-add` / `headers-check` | Add / check license headers |

Options: `WERROR=1`, `SHOW_FLAGS=1`, `UNWIND=1`.
