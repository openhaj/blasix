# <malloc.h> - allocator extensions (in no standard).
#
# ISO C: none. POSIX: none (<malloc.h> is no POSIX header).
# Every interface is an extension (EXT=1); the references are the manual
# pages of the system that defines it:
#   GNU    glibc <malloc.h>: malloc_usable_size(3); memalign, valloc,
#          pvalloc in posix_memalign(3) (obsolete; valloc is also in the
#          <stdlib.h> of the BSDs); cfree(3), a synonym of free(), removed
#          from the glibc headers in 2.26
#   APPLE  malloc_size(3): malloc_size, malloc_good_size (macOS declares
#          them in <malloc/malloc.h>; the libcs that copy the API, hajlibc
#          among them, in <malloc.h>)
#   42     show_alloc_mem, subject "malloc" of the 42 school: the zones
#          and the blocks, by increasing address, then the total
#
# No reserved line: no standard reserves names for <malloc.h>, and no
# target requires the header (header not in target without EXT=1).
#
# Host deviations (glibc 2.44): cfree (removed in 2.26), malloc_size,
# malloc_good_size and show_alloc_mem are not declared.

header malloc.h

# name                      since          tags
malloc_usable_size          EXT            ext=GNU
memalign                    EXT            ext=GNU
valloc                      EXT            ext=GNU
pvalloc                     EXT            ext=GNU
cfree                       EXT            ext=GNU
malloc_size                 EXT            ext=APPLE
malloc_good_size            EXT            ext=APPLE
