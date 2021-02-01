# The filesystem

## Drives

Drives can be set up similar to drives on DOS/Windows, but instead of
referencing them with letters (like DOS's A:, B:, etc.) they are identified
by numbers. For example, a path might look like:

    1:/myfolder/subfolder/hello.txt

Which would access whatever filesystem is present on drive 1. Drive number 0 is
reserved. Devices which can be read/written (like a serial console, for
instance) will be accessed as a file, similar to UNIX, except that instead of
the devices being in the /dev directory like it is in UNIX, the path to these
devices will begin with the special character `%`, for example a serial console
might be referenced with a path like this: `%console`. There can also be
devices inside subdirectories, so something like this is allowed: `%vterms/0`,
which will refer to the 1st virtual terminal (similar to Linux's `/dev/tty0`).

Since a "drive" can be various different mediums, like an ATA hard disk, an
ATAPI drive, a floppy disk, etc., a drive is represented by a generic drive
struct. A drive has the following properties (`drive_t`):

 - Physical sector size
 - Logical sector size
 - Type of drive
 - Filesystem ID (More about this later)
 - Any attributes

An array of open drives is maintained in an array (`drivetable`), indexed on
the drive number.

The "type" of the drive will represent by what means the drive can be read/
written. The type number will refer to an entry in a table of drive types, and
each drive type will contain various functions for operating on that type of
drive.

The filesystem ID is described more below.

Attributes are described as bit flags in `attr`. All attributes are defined as
C #defines, beginning with `DRIVE_ATTR_`, in virtfs.h.

 - Bit 0 - Present: The drive is present in the drive table.
 - Bit 1 - Writable: The drive can be written to. (All drives are at least readable.)

## Filesystems

The filesystem table (`fstable`) is a table of each different type of
filesystem that the operating system recognises, for example FAT12, FAT16,
ext2, etc. As described earlier, this table is referenced by each drive in the
drive table. Each entry in the fs table has the following properties:

 - `char name[8]`: An 8-byte string holding a human-readable name for the fs.
 - `int (*open)(const char *fname, int flags)`: Opens a file in the
        fs. The drive which the filesystem is read off is taken from the
        filename, since the first character of the filename is the drive id.
        This function should return a file descriptor to the newly opened file.
        More on file descriptors later.
 - `int (*close)(int fd)`: Closes a file referred to by the file descriptor.
        Should return 1 on success, otherwise an error code.
 - `int (*read)(int fd, void *dest, uint32_t start, uint32_t n)`: Reads `n`
        blocks from the file at `fd`, and stores them at the memory starting at
        `dest`. The bytes from the file are read starting at position `start`.
        Should return 1 on success, otherwise an error code.
 - `int (*mkdir)(const char *dname, int flags)`: Creates a new directory in the
        filesystem at a given path. `flags` is used to modify the properties of
        the new directory. Should return 1 on success, otherwise an error code.
 - `int (*getpath)(int fd, char *path)`: Returns the absolute path to a file,
        beginning with the drive number.
 - `int (*rmdir)(const char *dname*)`: Deletes a directory. If it can't be
        deleted, return an error code, otherwise return 1.
 - `int (*rmfile)(int fd)`: Removes the file at the given file descriptor.
        Returns 1 on success, or an error code otherwise.