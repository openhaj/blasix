# <fcntl.h> - file control options.
#
# ISO C: none (not an ISO C header).
# POSIX: XBD <fcntl.h>; XSH creat(), fcntl(), open(), posix_fadvise(),
# posix_fallocate().
#   2001  F_DUPFD ... F_SETOWN, FD_CLOEXEC, F_RDLCK, F_UNLCK, F_WRLCK;
#         O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC, O_APPEND, O_NONBLOCK, O_SYNC,
#         O_DSYNC and O_RSYNC (SIO), O_ACCMODE, O_RDONLY, O_RDWR, O_WRONLY;
#         XSI: SEEK_* (as in <unistd.h>) and the file mode bits (as in
#         <sys/stat.h>); ADV: POSIX_FADV_*, posix_fadvise,
#         posix_fallocate; mode_t, off_t, pid_t; creat, fcntl, open
#   2008  + F_DUPFD_CLOEXEC, O_CLOEXEC, O_DIRECTORY, O_NOFOLLOW,
#         O_TTY_INIT, O_EXEC, O_SEARCH, AT_FDCWD, AT_EACCESS,
#         AT_SYMLINK_NOFOLLOW, AT_SYMLINK_FOLLOW, AT_REMOVEDIR, openat;
#         SEEK_* (as in <stdio.h>) and the mode bits move to the base
#   2017  same as 2008
#   2024  + F_DUPFD_CLOFORK, FD_CLOFORK, O_CLOFORK, F_GETOWN_EX,
#         F_SETOWN_EX, F_OWNER_PID, F_OWNER_PGRP, F_OFD_GETLK,
#         F_OFD_SETLK, F_OFD_SETLKW (open file description locks)
#
# Not listed: the structures flock (2001 on) and f_owner_ex (2024): a
# spec name is one identifier; the fcntl() unit uses struct flock.
# "The symbolic constants for file modes ... as described in <sys/stat.h>"
# are taken as the file mode bits S_IRWXU ... S_ISVTX (not the S_IF* file
# types).
#
# Host deviations: none known yet.

header fcntl.h

# Reserved: XSH 2.2.2 (l_, F_, O_ for <fcntl.h>; S_ in 2001 to 2017, and
# <fcntl.h> may make visible all of <sys/stat.h> and <unistd.h> in every
# edition; posix_, POSIX_ for any header).
reserved POSIX.1-2001  *_t l_* F_* O_* S_* posix_* POSIX_*

# ----- Types (as described in <sys/types.h>) -----
# name                      since          tags
mode_t                      POSIX.1-2001   type same=sys/types.h
off_t                       POSIX.1-2001   type same=sys/types.h
pid_t                       POSIX.1-2001   type same=sys/types.h

# ----- fcntl() commands (unique, usable in #if) -----
F_DUPFD                     POSIX.1-2001   macro
F_GETFD                     POSIX.1-2001   macro
F_SETFD                     POSIX.1-2001   macro
F_GETFL                     POSIX.1-2001   macro
F_SETFL                     POSIX.1-2001   macro
F_GETLK                     POSIX.1-2001   macro
F_SETLK                     POSIX.1-2001   macro
F_SETLKW                    POSIX.1-2001   macro
F_GETOWN                    POSIX.1-2001   macro
F_SETOWN                    POSIX.1-2001   macro
F_DUPFD_CLOEXEC             POSIX.1-2008   macro
F_DUPFD_CLOFORK             POSIX.1-2024   macro
F_GETOWN_EX                 POSIX.1-2024   macro
F_SETOWN_EX                 POSIX.1-2024   macro
F_OFD_GETLK                 POSIX.1-2024   macro
F_OFD_SETLK                 POSIX.1-2024   macro
F_OFD_SETLKW                POSIX.1-2024   macro

# ----- Descriptor flags (bitwise-distinct), lock types, owner types -----
FD_CLOEXEC                  POSIX.1-2001   macro
FD_CLOFORK                  POSIX.1-2024   macro
F_RDLCK                     POSIX.1-2001   macro
F_UNLCK                     POSIX.1-2001   macro
F_WRLCK                     POSIX.1-2001   macro
F_OWNER_PID                 POSIX.1-2024   macro
F_OWNER_PGRP                POSIX.1-2024   macro

# ----- l_whence: as in <stdio.h> (XSI and "as in <unistd.h>" in 2001) -----
SEEK_SET                    POSIX.1-2001   macro XSI base=POSIX.1-2008 same=stdio.h
SEEK_CUR                    POSIX.1-2001   macro XSI base=POSIX.1-2008 same=stdio.h
SEEK_END                    POSIX.1-2001   macro XSI base=POSIX.1-2008 same=stdio.h

# ----- open() flags -----
# creation flags (bitwise-distinct; O_TTY_INIT may be 0)
O_CREAT                     POSIX.1-2001   macro
O_EXCL                      POSIX.1-2001   macro
O_NOCTTY                    POSIX.1-2001   macro
O_TRUNC                     POSIX.1-2001   macro
O_CLOEXEC                   POSIX.1-2008   macro
O_DIRECTORY                 POSIX.1-2008   macro
O_NOFOLLOW                  POSIX.1-2008   macro
O_TTY_INIT                  POSIX.1-2008   macro
O_CLOFORK                   POSIX.1-2024   macro
# file status flags
O_APPEND                    POSIX.1-2001   macro
O_NONBLOCK                  POSIX.1-2001   macro
O_SYNC                      POSIX.1-2001   macro
O_DSYNC                     POSIX.1-2001   macro SIO
O_RSYNC                     POSIX.1-2001   macro SIO
# access modes and their mask (unique; O_EXEC and O_SEARCH may be equal)
O_ACCMODE                   POSIX.1-2001   macro
O_RDONLY                    POSIX.1-2001   macro
O_RDWR                      POSIX.1-2001   macro
O_WRONLY                    POSIX.1-2001   macro
O_EXEC                      POSIX.1-2008   macro
O_SEARCH                    POSIX.1-2008   macro

# ----- File mode bits, as described in <sys/stat.h> (XSI in 2001) -----
S_IRWXU                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IRUSR                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IWUSR                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IXUSR                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IRWXG                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IRGRP                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IWGRP                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IXGRP                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IRWXO                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IROTH                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IWOTH                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_IXOTH                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_ISUID                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_ISGID                     POSIX.1-2001   macro XSI base=POSIX.1-2008 same=sys/stat.h
S_ISVTX                     POSIX.1-2001   macro XSI same=sys/stat.h

# ----- *at() functions (2008) -----
AT_FDCWD                    POSIX.1-2008   macro
AT_EACCESS                  POSIX.1-2008   macro
AT_SYMLINK_NOFOLLOW         POSIX.1-2008   macro
AT_SYMLINK_FOLLOW           POSIX.1-2008   macro
AT_REMOVEDIR                POSIX.1-2008   macro

# ----- Advisory Information -----
POSIX_FADV_DONTNEED         POSIX.1-2001   macro ADV
POSIX_FADV_NOREUSE          POSIX.1-2001   macro ADV
POSIX_FADV_NORMAL           POSIX.1-2001   macro ADV
POSIX_FADV_RANDOM           POSIX.1-2001   macro ADV
POSIX_FADV_SEQUENTIAL       POSIX.1-2001   macro ADV
POSIX_FADV_WILLNEED         POSIX.1-2001   macro ADV
posix_fadvise               POSIX.1-2001   ADV
posix_fallocate             POSIX.1-2001   ADV

# ----- Functions -----
creat                       POSIX.1-2001
fcntl                       POSIX.1-2001
open                        POSIX.1-2001
openat                      POSIX.1-2008
