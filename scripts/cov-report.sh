#!/bin/sh
# cov-report.sh ROOT OBJ_DIR DUMP_DIR OUT_DIR [REAL_ROOT]
#
# Line, function and branch coverage of a libc built with coverage
# counters, for EVERY file compiled into it, run or not.
#
#   ROOT       the tree the libc was compiled in (relative source paths of
#              the .gcno are relative to it);
#   OBJ_DIR    where its objects and .gcno are;
#   DUMP_DIR   one tree per test process (DUMP_DIR/<pid>/<abs path>.gcda,
#              written by src/framework/cov.c);
#   OUT_DIR    the report;
#   REAL_ROOT  the path the report names the sources by (the coverage
#              build is a copy of the sources: the originals); default ROOT.
#
# The profiles are merged with gcov-tool and copied next to the .gcno; then
# every object that has a .gcno goes through gcov: one that no test linked
# or ran has no .gcda and counts as never executed (0%), so the total is
# over the whole library, not only over what the tests happened to call.
#
# OUT_DIR gets:
#   lcov.info   lcov tracefile (lines, functions, branches; SF under
#               REAL_ROOT), for genhtml, editors, CI services, ...;
#   lines.tsv   <path relative to ROOT> <percent> <lines>, one per file;
#   html/       genhtml's report, when genhtml (lcov) is installed.
set -eu
root=$(cd "$1" && pwd)
obj=$(cd "$2" && pwd)
dump=$3
out=$4
real=${5:-$root}
here=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$out"

# GCOV / GCOV_TOOL: the versions matching the compiler (default gcov, gcov-tool).
gcov=${GCOV:-gcov}
gcov_tool=${GCOV_TOOL:-gcov-tool}
command -v "$gcov" >/dev/null || { echo "coverage: $gcov not found" >&2; exit 1; }
command -v "$gcov_tool" >/dev/null || { echo "coverage: $gcov_tool not found" >&2; exit 1; }

find "$obj" -name '*.gcno' | sort > "$out/gcno.list"
if [ ! -s "$out/gcno.list" ]; then
	echo "coverage: no .gcno under $obj (was the libc built with coverage?)" >&2
	exit 1
fi

# ----- merge the profiles of every test process -----
find "$obj" -name '*.gcda' -delete
acc="$out/merged"
rm -rf "$acc" "$acc.tmp"
set -- "$dump"/*/
if [ -d "$1" ]; then
	echo "  COV      merging $# profiles"
	cp -r "$1" "$acc"
	shift
	for d in "$@"; do
		"$gcov_tool" merge -o "$acc.tmp" "$acc" "$d" >/dev/null 2>&1 \
			&& { rm -rf "$acc"; mv "$acc.tmp" "$acc"; } \
			|| rm -rf "$acc.tmp"
	done
	(cd "$acc" && find . -name '*.gcda') | while read -r f; do
		cp "$acc/$f" "/${f#./}"
	done
else
	echo "coverage: no profile under $dump (did the tests run?): everything at 0%" >&2
fi

# ----- every object, with or without a .gcda -----
echo "  COV      $(wc -l < "$out/gcno.list") objects, $(find "$obj" -name '*.gcda' | wc -l) run"
(cd "$root" && xargs "$gcov" -t -b -c < "$out/gcno.list" 2>/dev/null) \
	| awk -v root="$root" -v real="$real" -v tsv="$out/lines.tsv" \
		-f "$here/gcov2lcov.awk" > "$out/lcov.info"

# ----- summary per directory (src/<dir>, include/<dir>) -----
awk -F'\t' '
	{ n = split($1, p, "/"); d = (n > 2) ? p[1] "/" p[2] : p[1];
	  hit = $2 * $3 / 100; H[d] += hit; N[d] += $3; TH += hit; TN += $3 }
	END {
		printf "\n  %-28s %8s %8s %8s\n", "directory", "lines", "hit", "cover"
		for (d in N) printf "  %-28s %8d %8d %7.1f%%\n", d, N[d], H[d], N[d] ? 100 * H[d] / N[d] : 0 | "sort"
		close("sort")
		printf "  %-28s %8d %8d %7.1f%%\n", "TOTAL", TN, TH, TN ? 100 * TH / TN : 0
	}' "$out/lines.tsv"
echo "  lcov tracefile: $out/lcov.info"
echo "  per-file data:  $out/lines.tsv"

if command -v genhtml >/dev/null 2>&1; then
	rm -rf "$out/html"
	genhtml -q --branch-coverage --prefix "$real" -o "$out/html" "$out/lcov.info" >/dev/null 2>&1 \
		&& echo "  html report:    $out/html/index.html"
fi
