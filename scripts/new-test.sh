#!/bin/sh
# new-test.sh CATEGORY FUNCTION [GROUP] [SINCE] [HEADER]
#
# Create src/suites/CATEGORY/GROUP/FUNCTION.c from templates/test.c. GROUP
# is a folder of the category (string: copy, search, ...; `make new`
# lists the existing ones); it is only a way to sort the files, the unit
# stays CATEGORY/FUNCTION. The function must be in spec/CATEGORY.spec; if
# it is not, SINCE (C99 ... C23, POSIX.1-2001 ... POSIX.1-2024, EXT; make
# standards) is required and the entry is appended (add its tags after:
# option, base=, ob=, removed=, same=). A new spec file is made from
# templates/spec.spec (its "#:" instruction lines removed); HEADER is
# only needed when the category does not spell the header (default:
# sys_stat -> sys/stat.h).
#
# A type, or a macro without GROUP (a constant), gets no unit: its cases
# go in src/suites/bits/, run by src/suites/CATEGORY/defs.c (created from
# templates/defs.c and listed if needed); the script says what to add.
# GROUP is required for a function or a function-like macro.
#
# The unit is also added to mk/suites.mk (CATEGORY_UNITS, GROUP/FUNCTION,
# kept sorted; a new category gets its block and its CATEGORIES entry).
#
# The "Spec:" line of the new file points to the pages of data/ that
# describe the function, when they exist.
set -eu
. "$(dirname "$0")/project.sh"
cat=${1:-}
func=${2:-}
group=${3:-}
since=${4:-}
header=${5:-}
if [ -z "$cat" ] || [ -z "$func" ]; then
	echo "usage: make new CAT=<cat> FUNC=<function> GROUP=<folder> [SINCE=<version>] [HEADER=<x/y.h>]" >&2
	if [ -n "$cat" ] && [ -d "src/suites/$cat" ]; then
		echo "groups of $cat: $(cd "src/suites/$cat" && find . -mindepth 1 -type d | sed 's|^\./||' | sort | tr '\n' ' ')" >&2
	fi
	exit 2
fi
spec="spec/$cat.spec"
other=$(find "src/suites/$cat" -name "$func.c" 2>/dev/null | head -1)
if [ -n "$other" ]; then
	echo "$func already has a unit: $other" >&2
	exit 1
fi
if ! [ -e "$spec" ] || ! awk -v f="$func" '$1 == f { found = 1 } END { exit !found }' "$spec"; then
	if [ -z "$since" ]; then
		echo "$func is not in $spec: pass SINCE=<version> (make standards)" >&2
		exit 1
	fi
	if [ ! -e "$spec" ]; then
		[ -n "$header" ] || header=$(echo "$cat" | sed 's/^\(sys\|netinet\|arpa\|net\)_/\1\//').h
		grep -v '^#:' templates/spec.spec | sed -e "s|@HEADER@|$header|g" > "$spec"
		echo "created $spec from templates/spec.spec (header $header): fill it in"
	fi
	printf '%-27s %s\n' "$func" "$since" >> "$spec"
	echo "added $func ($since) to $spec"
fi
header=$(awk '$1 == "header" { print $2; exit }' "$spec")
kind=$(awk -v f="$func" '$1 == f { for (i = 3; i <= NF; i++) if ($i == "type" || $i == "macro") { print $i; exit } }' "$spec")
mkfile=mk/suites.mk

# List <cat>/<entry> in mk/suites.mk: the build only compiles the listed units.
list_unit() {
	if ! grep -q "^${cat}_UNITS :=" "$mkfile"; then
		# New category: entry in CATEGORIES, then an empty block at the end.
		LC_ALL=C awk -v c="$cat" '
			/^CATEGORIES :=/ { inb = 1; print; next }
			inb && !done && (/^$/ || $1 > c) { printf "\t%s \\\n", c; done = 1 }
			/^$/ { inb = 0 }
			{ print }' "$mkfile" > "$mkfile.tmp" && mv "$mkfile.tmp" "$mkfile"
		printf '\n# ----- %s -----\n\n%s_UNITS := \\\n\n%s_HDRS :=\n' \
			"$cat" "$cat" "$cat" >> "$mkfile"
	fi
	LC_ALL=C awk -v c="$cat" -v f="$1" '
		$0 == c "_UNITS := \\" { inb = 1; print; next }
		inb && !done && (/^$/ || $1 > f) { printf "\t%s \\\n", f; done = 1 }
		/^$/ { inb = 0 }
		{ print }' "$mkfile" > "$mkfile.tmp" && mv "$mkfile.tmp" "$mkfile"
	echo "listed $cat/$1 in $mkfile"
}

# A type, or a macro without GROUP (a constant): its cases go in bits/,
# run by <cat>/defs.c, not in a unit of its own.
if [ "$kind" = type ] || { [ "$kind" = macro ] && [ -z "$group" ]; }; then
	defs="src/suites/$cat/defs.c"
	if [ ! -e "$defs" ]; then
		mkdir -p "src/suites/$cat"
		sed -e "s|@HEADER@|$header|g" -e "s|@CAT@|$cat|g" \
			-e "s|@YEAR@|$(date '+%Y')|g" -e "s|@DATE@|$(date '+%Y/%m/%d %H:%M:%S')|g" \
			-e "s|@AUTHOR@|$AUTHOR_NAME|g" -e "s|@EMAIL@|$AUTHOR_EMAIL|g" \
			templates/defs.c > "$defs"
		echo "created $defs from templates/defs.c"
		list_unit defs
	fi
	shared=$(find src/suites/bits -name "$func.h" 2>/dev/null | head -1)
	if [ -n "$shared" ]; then
		echo "$func ($kind): add #include \"${shared#src/suites/}\" to $defs (shared cases)"
	else
		echo "$func ($kind): write its cases in src/suites/bits/<origin>/$func.h (guarded by"
		echo "  LC_REQ_$func, TEST_SYM), include it from $defs; cases only <$header> has go"
		echo "  in $defs itself (make spec-check: LC_REQ_$func must appear)"
	fi
	exit 0
fi
if [ -z "$group" ]; then
	echo "usage: make new CAT=$cat FUNC=$func GROUP=<folder> (a function, or a function-like macro)" >&2
	echo "groups of $cat: $(cd "src/suites/$cat" 2>/dev/null && find . -mindepth 1 -type d | sed 's|^\./||' | sort | tr '\n' ' ')" >&2
	exit 2
fi
file="src/suites/$cat/$group/$func.c"

# Where the standards describe it (newest POSIX page; ISO C: by clause).
refs=""
for d in POSIX2024/functions POSIX2017/functions POSIX2008/functions POSIX2001/susv3xsh; do
	if [ -f "data/POSIX/$d/$func.html" ]; then
		refs="data/POSIX/$d/$func.html"
		break
	fi
done
if awk -v f="$func" '$1 == f && $2 ~ /^C[0-9]/ { found = 1 } END { exit !found }' "$spec"; then
	refs="${refs:+$refs; }ISO C: data/C/C23.pdf, clause of $func()"
fi
refs=${refs:-"<section of the standard>"}

mkdir -p "src/suites/$cat/$group"
sed -e "s|@CAT@.h|$header|g" -e "s|@FUNC@|$func|g" -e "s|@SPEC@|$func()|g" \
	-e "s|@REFS@|$refs|g" \
	-e "s|@YEAR@|$(date '+%Y')|g" -e "s|@DATE@|$(date '+%Y/%m/%d %H:%M:%S')|g" \
	-e "s|@AUTHOR@|$AUTHOR_NAME|g" -e "s|@EMAIL@|$AUTHOR_EMAIL|g" \
	templates/test.c > "$file"
echo "created $file"

list_unit "$group/$func"
