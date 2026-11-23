#!/bin/sh
# new-test.sh CATEGORY FUNCTION [SINCE] [HEADER]
#
# Create src/suites/CATEGORY/FUNCTION.c from templates/test.c. The function
# must be in spec/CATEGORY.spec; if it is not, SINCE (C99 ... C23,
# POSIX.1-2001 ... POSIX.1-2024, EXT; make standards) is required and the
# entry is appended (add its tags after: option, base=, ob=, removed=).
# HEADER is only needed to create a new spec file whose header the
# category does not spell (default: sys_stat -> sys/stat.h).
#
# The "Spec:" line of the new file points to the pages of data/ that
# describe the function, when they exist.
set -eu
. "$(dirname "$0")/project.sh"
cat=${1:-}
func=${2:-}
since=${3:-}
header=${4:-}
if [ -z "$cat" ] || [ -z "$func" ]; then
	echo "usage: make new CAT=<cat> FUNC=<function> [SINCE=<version>] [HEADER=<x/y.h>]" >&2
	exit 2
fi
file="src/suites/$cat/$func.c"
spec="spec/$cat.spec"
if [ -e "$file" ]; then
	echo "$file already exists" >&2
	exit 1
fi
if ! [ -e "$spec" ] || ! awk -v f="$func" '$1 == f { found = 1 } END { exit !found }' "$spec"; then
	if [ -z "$since" ]; then
		echo "$func is not in $spec: pass SINCE=<version> (make standards)" >&2
		exit 1
	fi
	if [ ! -e "$spec" ]; then
		[ -n "$header" ] || header=$(echo "$cat" | sed 's/^\(sys\|netinet\|arpa\|net\)_/\1\//').h
		printf '# <%s>\n\nheader %s\n\n# name                 since          tags\n' \
			"$header" "$header" > "$spec"
		echo "created $spec (header $header)"
	fi
	printf '%-22s %s\n' "$func" "$since" >> "$spec"
	echo "added $func ($since) to $spec"
fi
header=$(awk '$1 == "header" { print $2; exit }' "$spec")

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

mkdir -p "src/suites/$cat"
sed -e "s|@CAT@.h|$header|g" -e "s|@FUNC@|$func|g" -e "s|@SPEC@|$func()|g" \
	-e "s|@REFS@|$refs|g" \
	-e "s|@YEAR@|$(date '+%Y')|g" -e "s|@DATE@|$(date '+%Y/%m/%d %H:%M:%S')|g" \
	-e "s|@AUTHOR@|$AUTHOR_NAME|g" -e "s|@EMAIL@|$AUTHOR_EMAIL|g" \
	templates/test.c > "$file"
echo "created $file"
