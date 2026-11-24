#: templates/spec.spec - template of spec/<category>.spec (one per header).
#:
#: `make new CAT=<cat> GROUP=<g> FUNC=<f> SINCE=<v>` copies this file into
#: spec/<cat>.spec when it does not exist yet, without the "#:" lines (this
#: guide) and with @HEADER@ replaced. Fill in the "#" skeleton, then add
#: one line per interface. Only write what the texts of data/ say; an
#: uncertain point stays a "# TODO" until checked. Everything here is data
#: for lcrun: the format itself is documented in CONTRIBUTING.md, not in
#: each spec file.
#:
#: ----- 1. Summary: what each edition says of the header -----
#:
#: The clauses read (ISO C: chapter of the header in each edition; POSIX:
#: XBD <header.h> and the XSH pages), then one line per edition: what it
#: adds, moves, makes obsolescent or removes. Name the clause when it is
#: not obvious. Write "same as <edition>" rather than nothing.
#:
#: ----- 2. header -----
#:
#: The header the probes and units include.
#:
#: ----- 3. reserved -----
#:
#: What the header may declare without the target requiring it (strict
#: mode: any other name the target does not require must be hidden):
#:   - ISO C future library directions of this header: C99 7.26.x, C11 /
#:     C17 7.31.x, C23 7.33.x (and those of a header it includes, e.g.
#:     <inttypes.h> includes <stdint.h>);
#:   - POSIX: the XSH 2.2.2 table row of the header, and "*_t" (any header);
#:   - leading underscores are built in: do not list them.
#: One line per edition from which the patterns hold (fnmatch() syntax):
#:   reserved C99           str[a-z]* mem[a-z]*
#:   reserved C23           PRIB* SCNB*
#:   reserved POSIX.1-2001  *_t
#: Never widen a pattern to silence a leak of a libc: only the standard
#: decides. A header that is not ISO C (strings.h) has no C99 line.
#:
#: ----- 4. Interfaces: one line per interface and per ladder -----
#:
#:   <name>   <since>   [kind] [OPTION] [base=V] [ob=V,..] [removed=V,..]
#:                      [ext=NAME] [same=H,..]
#:
#:   since    C99 C11 C17 C23 POSIX.1-2001 POSIX.1-2008 POSIX.1-2017
#:            POSIX.1-2024, or EXT (with ext=GNU / BSD); older ISO C
#:            interfaces are C99 (no older text in data/)
#:   kind     macro, type, either (macro or function), var; default
#:            function
#:   OPTION   one option group (make options): XSI, TSF, ..., LIB_EXT1,
#:            THREADS, ...
#:   base=V   the option group's interface moves to the base in V
#:   ob= / removed=   obsolescent from / removed in (comma lists)
#:   same=H   other headers that define it "as described in" (the probe
#:            checks they agree); a symbol several headers define also
#:            gets its shared cases in src/suites/bits/<name>.h
#:
#: Two lines for one name when ISO C and POSIX tell different stories
#: (memccpy: C23, and POSIX.1-2001 XSI). Group the lines as the header's
#: page does (copying, search, ...) with a "#" comment above each group,
#: and give the clause of anything that is not plain (an option, a move
#: to the base, a removal). Annex K: the LIB_EXT1 lines go last.
#:
#: ----- 5. Host deviations -----
#:
#: What the reference libc gets wrong, found while testing (an interface
#: the strict headers hide, ...): one line each, with the clause, so that
#: a failing probe is not taken for a bad spec.
#:
# <@HEADER@> - <one-line description>.
#
# ISO C: <clause per edition>.
#   C99   <what C99 defines>
#   C11   <same as C99 | what changes>
#   C17   <...>
#   C23   <...>
# POSIX: XBD <@HEADER@>; XSH <pages>.
#   2001  <...>
#   2008  <...>
#   2017  <...>
#   2024  <...>
#
# Host deviations: <none known | interface: what, clause>.

header @HEADER@

# Reserved: ISO C <7.26.x / 7.31.x / 7.33.x>, XSH 2.2.2.
reserved C99           <patterns>
reserved POSIX.1-2001  *_t

# name                      since          tags
