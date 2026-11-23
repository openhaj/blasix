#!/bin/sh
# cov-report.sh SRC_DIR OBJ_DIR DUMP_DIR OUT_DIR
#
# DUMP_DIR holds one tree per test process (DUMP_DIR/<pid>/<abs path>.gcda,
# written by src/framework/cov.c). They are merged with gcov-tool, the result
# is copied next to the objects of the libc under test (where the .gcno
# are), then gcov gives the line coverage per directory and in total.
# With gcovr installed, OUT_DIR/index.html is written too.
set -eu
src=$1
obj=$2
dump=$3
out=$4
mkdir -p "$out"

command -v gcov-tool >/dev/null || { echo "coverage: gcov-tool not found" >&2; exit 1; }
set -- "$dump"/*/
if [ ! -d "$1" ]; then
	echo "coverage: nothing under $dump (did the tests run?)" >&2
	exit 1
fi
echo "  COV      merging $# profiles"
acc="$out/merged"
rm -rf "$acc"
cp -r "$1" "$acc"
shift
for d in "$@"; do
	gcov-tool merge -o "$acc.tmp" "$acc" "$d" >/dev/null 2>&1 \
		&& { rm -rf "$acc"; mv "$acc.tmp" "$acc"; } \
		|| rm -rf "$acc.tmp"
done
find "$obj" -name '*.gcda' -delete
(cd "$acc" && find . -name '*.gcda') | while read -r f; do
	cp "$acc/$f" "/${f#./}"
done

find "$obj" -name '*.gcda' > "$out/gcda.list" || true
if [ ! -s "$out/gcda.list" ]; then
	echo "coverage: no .gcda under $obj (did the tests run?)" >&2
	exit 1
fi

: > "$out/lines.tsv"
while read -r gcda; do
	o=${gcda%.gcda}.o
	rel=${o#"$obj"/}
	c="$src/${rel%.o}.c"
	[ -f "$c" ] || continue
	# "Lines executed:83.33% of 12" for the source file itself
	gcov -n -o "$(dirname "$o")" "$c" 2>/dev/null \
	| awk -v f="${rel%.o}.c" '
		/^File / { cur = $2; gsub(/\x27/, "", cur) }
		/^Lines executed:/ && index(cur, f) {
			split($2, a, ":"); sub(/%/, "", a[2]);
			printf "%s\t%s\t%s\n", f, a[2], $4; exit }' >> "$out/lines.tsv"
done < "$out/gcda.list"

awk -F'\t' '
	{ n = split($1, p, "/"); d = (n > 2) ? p[1] "/" p[2] : p[1];
	  hit = $2 * $3 / 100; H[d] += hit; N[d] += $3; TH += hit; TN += $3 }
	END {
		printf "\n  %-28s %8s %8s %8s\n", "directory", "lines", "hit", "cover"
		for (d in N) printf "  %-28s %8d %8d %7.1f%%\n", d, N[d], H[d], 100 * H[d] / N[d] | "sort"
		close("sort")
		printf "  %-28s %8d %8d %7.1f%%\n", "TOTAL", TN, TH, TN ? 100 * TH / TN : 0
	}' "$out/lines.tsv"
echo "  per-file data: $out/lines.tsv"

if command -v gcovr >/dev/null 2>&1; then
	gcovr -r "$src" --object-directory "$obj" --html-details -o "$out/index.html" >/dev/null 2>&1 \
		&& echo "  html report:   $out/index.html"
fi
