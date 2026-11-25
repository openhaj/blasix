#!/bin/sh
# sync-tree.sh NEW DST
#
# Make directory DST hold exactly the files of NEW, then remove NEW
# (make: the probe sources, mk/tests.mk). A file whose content did not
# change keeps its date, so make rebuilds only what depends on a file
# that really changed; a file that is no longer in NEW is removed.
#
# Each file is replaced atomically (copy, then rename): several syncs may
# run at once on the same DST, while files of it are being compiled.
set -eu
new=$1
dst=$2

mkdir -p "$dst"
(cd "$new" && find . -type f) | while IFS= read -r f; do
	if ! cmp -s "$new/$f" "$dst/$f"; then
		mkdir -p "$(dirname "$dst/$f")"
		cp -f "$new/$f" "$dst/$f.tmp.$$"
		mv -f "$dst/$f.tmp.$$" "$dst/$f"
	fi
done
(cd "$dst" && find . -type f ! -name '*.tmp.*') | while IFS= read -r f; do
	[ -f "$new/$f" ] || rm -f "$dst/$f"
done
rm -rf "$new"
