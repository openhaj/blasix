#!/bin/sh
# progress.sh DONE TOTAL WHAT [LINE]
#
# One step of the build progress bar (make: lc_tick in mk/build.mk).
# On a terminal: redraw "  building [#####.....]  DONE/TOTAL  WHAT" on the
# same line. Elsewhere (a log, CI): print LINE when given, else nothing,
# so that logs keep one line per unit and no escape codes.
set -eu
done=${1:-0}
total=${2:-0}
what=${3:-}
line=${4:-}
if [ ! -t 1 ]; then
	[ -n "$line" ] && printf '%s\n' "$line"
	exit 0
fi
[ "$total" -gt 0 ] || total=1
[ "$done" -le "$total" ] || done=$total
width=30
fill=$((done * width / total))
bar=$(printf '%*s' "$fill" '' | tr ' ' '#')$(printf '%*s' $((width - fill)) '' | tr ' ' '.')
printf '\r\033[2K  building [%s] %*d/%d  %s' "$bar" ${#total} "$done" "$total" "$what"
