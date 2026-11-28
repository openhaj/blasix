# <sys/stat.h> - data returned by the stat() function.
#
# ISO C: none (not an ISO C header).
# POSIX: XBD <sys/stat.h>; XSH chmod(), fchmod(), fstat(), lstat(),
# mkdir(), mkfifo(), mknod(), stat(), umask() and their *at() forms,
# futimens().
#   2001  struct stat; the types as in <sys/types.h> (blkcnt_t, blksize_t
#         in the base); XSI: S_IFMT and the S_IF* file types, S_ISVTX,
#         mknod; the mode bits S_IRWXU ... S_ISGID; S_ISBLK ... S_ISSOCK;
#         S_TYPEISMQ, S_TYPEISSEM, S_TYPEISSHM; TYM: S_TYPEISTMO; chmod,
#         fchmod, fstat, lstat, mkdir, mkfifo, stat, umask
#   2008  + st_atim, st_mtim, st_ctim (struct timespec) and the st_atime,
#         st_mtime, st_ctime macros; UTIME_NOW, UTIME_OMIT; the mode bits
#         get their numeric values (0400 ...) and are usable in #if;
#         fchmodat, fstatat, futimens, mkdirat, mkfifoat, utimensat;
#         XSI: mknodat
#   2017  blkcnt_t and blksize_t become XSI (with st_blksize, st_blocks);
#         may make visible all of <time.h>
#   2024  same as 2017
#
# Not listed: the structures stat and timespec (a spec name is one
# identifier); the fstat() unit uses struct stat.
#
# Host deviations: none known yet.

header sys/stat.h

# Reserved: XSH 2.2.2 (st_ and S_ for <sys/stat.h>; *_t, posix_, POSIX_
# for any header).
reserved POSIX.1-2001  *_t st_* S_* posix_* POSIX_*

# ----- Types (as described in <sys/types.h>) -----
# name                      since          tags
# blkcnt_t, blksize_t: base in 2001 and 2008, XSI from 2017. One POSIX
# line per name: XSI throughout, so without XSI 2001 and 2008 do not
# require them (too lax there; the other way would require them without
# XSI from 2017, which the text does not).
blkcnt_t                    POSIX.1-2001   type XSI same=sys/types.h
blksize_t                   POSIX.1-2001   type XSI same=sys/types.h
dev_t                       POSIX.1-2001   type same=sys/types.h
ino_t                       POSIX.1-2001   type same=sys/types.h
mode_t                      POSIX.1-2001   type same=sys/types.h
nlink_t                     POSIX.1-2001   type same=sys/types.h
uid_t                       POSIX.1-2001   type same=sys/types.h
gid_t                       POSIX.1-2001   type same=sys/types.h
off_t                       POSIX.1-2001   type same=sys/types.h
time_t                      POSIX.1-2001   type same=sys/types.h

# ----- Compatibility macros for the timestamps (2008) -----
st_atime                    POSIX.1-2008   macro
st_ctime                    POSIX.1-2008   macro
st_mtime                    POSIX.1-2008   macro

# ----- File types (XSI) -----
S_IFMT                      POSIX.1-2001   macro XSI
S_IFBLK                     POSIX.1-2001   macro XSI
S_IFCHR                     POSIX.1-2001   macro XSI
S_IFIFO                     POSIX.1-2001   macro XSI
S_IFREG                     POSIX.1-2001   macro XSI
S_IFDIR                     POSIX.1-2001   macro XSI
S_IFLNK                     POSIX.1-2001   macro XSI
S_IFSOCK                    POSIX.1-2001   macro XSI

# ----- File mode bits -----
S_IRWXU                     POSIX.1-2001   macro
S_IRUSR                     POSIX.1-2001   macro
S_IWUSR                     POSIX.1-2001   macro
S_IXUSR                     POSIX.1-2001   macro
S_IRWXG                     POSIX.1-2001   macro
S_IRGRP                     POSIX.1-2001   macro
S_IWGRP                     POSIX.1-2001   macro
S_IXGRP                     POSIX.1-2001   macro
S_IRWXO                     POSIX.1-2001   macro
S_IROTH                     POSIX.1-2001   macro
S_IWOTH                     POSIX.1-2001   macro
S_IXOTH                     POSIX.1-2001   macro
S_ISUID                     POSIX.1-2001   macro
S_ISGID                     POSIX.1-2001   macro
S_ISVTX                     POSIX.1-2001   macro XSI

# ----- File type test macros (function-like: one unit each) -----
S_ISBLK                     POSIX.1-2001   macro
S_ISCHR                     POSIX.1-2001   macro
S_ISDIR                     POSIX.1-2001   macro
S_ISFIFO                    POSIX.1-2001   macro
S_ISREG                     POSIX.1-2001   macro
S_ISLNK                     POSIX.1-2001   macro
S_ISSOCK                    POSIX.1-2001   macro
S_TYPEISMQ                  POSIX.1-2001   macro
S_TYPEISSEM                 POSIX.1-2001   macro
S_TYPEISSHM                 POSIX.1-2001   macro
S_TYPEISTMO                 POSIX.1-2001   macro TYM

# ----- Special tv_nsec values of futimens() and utimensat() (2008) -----
UTIME_NOW                   POSIX.1-2008   macro
UTIME_OMIT                  POSIX.1-2008   macro

# ----- Functions -----
chmod                       POSIX.1-2001
fchmod                      POSIX.1-2001
fstat                       POSIX.1-2001
lstat                       POSIX.1-2001
mkdir                       POSIX.1-2001
mkfifo                      POSIX.1-2001
stat                        POSIX.1-2001
umask                       POSIX.1-2001
fchmodat                    POSIX.1-2008
fstatat                     POSIX.1-2008
futimens                    POSIX.1-2008
mkdirat                     POSIX.1-2008
mkfifoat                    POSIX.1-2008
utimensat                   POSIX.1-2008
mknod                       POSIX.1-2001   XSI
mknodat                     POSIX.1-2008   XSI
