# gcov2lcov.awk - gcov's text output (gcov -t -b -c, every object in one
# stream) -> an lcov tracefile, plus a per-file summary.
#
#   gcov -t -b -c OBJ.gcno... | awk -v root=ROOT -v real=REAL -v tsv=FILE \
#       -f gcov2lcov.awk > lcov.info
#
#   root  directory the library was compiled in: relative "Source:" paths
#         are relative to it, and only files under it are kept (not the
#         compiler's own headers);
#   real  the path written in SF: lines instead of root (the coverage build
#         is a copy of the sources: point at the originals); default root;
#   tsv   also write "<path relative to root>\t<percent>\t<lines>" there,
#         one line per file.
#
# A header compiled into several objects (static inline functions) comes
# once per object: its counts are added up, as lcov does when merging.
# Files appear in the order they were first seen; lines in line order.
# POSIX awk only (no gawk extension).

function normalize(p,    n, i, k, parts, out) {
	if (substr(p, 1, 1) != "/")
		p = root "/" p
	n = split(p, parts, "/")
	k = 0
	for (i = 1; i <= n; i++) {
		if (parts[i] == "" || parts[i] == ".")
			continue
		if (parts[i] == "..") {
			if (k > 0)
				k--
			continue
		}
		out[++k] = parts[i]
	}
	p = ""
	for (i = 1; i <= k; i++)
		p = p "/" out[i]
	return p
}

BEGIN {
	if (real == "")
		real = root
	root = normalize(root)
	nfiles = 0
}

# "        -:    0:Source:src/string/mem/memchr.c"
/^ *-: *0:Source:/ {
	path = $0
	sub(/^ *-: *0:Source:/, "", path)
	path = normalize(path)
	if (index(path, root "/") != 1) {
		cur = ""                        # outside the tree: not ours
		next
	}
	cur = substr(path, length(root) + 2)
	if (!(cur in seen)) {
		seen[cur] = 1
		files[++nfiles] = cur
	}
	pendingFn = ""
	line = 0
	next
}

cur == "" { next }

# "function memchr called 12 returned 100% blocks executed 80%"
/^function / {
	pendingFn = $2
	fnCalls[cur SUBSEP $2] += $4
	next
}

# "branch  0 taken 3 (fallthrough)", "branch  1 never executed"
/^branch / {
	key = cur SUBSEP line SUBSEP $2
	if (!(key in brTaken)) {
		brTaken[key] = "-"
		brList[cur] = brList[cur] " " line ":" $2
	}
	if ($3 == "taken") {
		if (brTaken[key] == "-")
			brTaken[key] = 0
		brTaken[key] += $4
	}
	next
}

# "       12:   42:	code", "    #####:   43:	code", "        -:   44:	code"
/^ *[^ :]+: *[0-9]+:/ {
	split($0, f, ":")
	count = f[1]
	gsub(/ /, "", count)
	line = f[2] + 0
	if (line == 0)
		next
	if (line > maxLine[cur])
		maxLine[cur] = line
	if (pendingFn != "") {
		if (!((cur SUBSEP pendingFn) in fnLine)) {
			fnLine[cur SUBSEP pendingFn] = line
			fnList[cur] = fnList[cur] " " pendingFn
		}
		pendingFn = ""
	}
	if (count == "-")
		next
	if (count ~ /^[#=]+$/)
		count = 0
	sub(/\*$/, "", count)           # "3*": some blocks of the line not run
	hits[cur SUBSEP line] += count
	exec[cur SUBSEP line] = 1
}

END {
	for (i = 1; i <= nfiles; i++) {
		cur = files[i]
		print "TN:blasix"
		print "SF:" real "/" cur

		fnf = fnh = 0
		n = split(fnList[cur], fns, " ")
		for (k = 1; k <= n; k++)
			print "FN:" fnLine[cur SUBSEP fns[k]] "," fns[k]
		for (k = 1; k <= n; k++) {
			c = fnCalls[cur SUBSEP fns[k]]
			print "FNDA:" c "," fns[k]
			fnf++
			if (c > 0)
				fnh++
		}
		print "FNF:" fnf
		print "FNH:" fnh

		brf = brh = 0
		n = split(brList[cur], brs, " ")
		for (k = 1; k <= n; k++) {
			split(brs[k], lb, ":")
			t = brTaken[cur SUBSEP lb[1] SUBSEP lb[2]]
			print "BRDA:" lb[1] ",0," lb[2] "," t
			brf++
			if (t != "-" && t > 0)
				brh++
		}
		print "BRF:" brf
		print "BRH:" brh

		lf = lh = 0
		for (l = 1; l <= maxLine[cur]; l++) {
			if (!((cur SUBSEP l) in exec))
				continue
			c = hits[cur SUBSEP l]
			print "DA:" l "," c
			lf++
			if (c > 0)
				lh++
		}
		print "LF:" lf
		print "LH:" lh
		print "end_of_record"

		if (tsv != "")
			printf "%s\t%.2f\t%d\n", cur, lf ? 100 * lh / lf : 0, lf > tsv
	}
}
