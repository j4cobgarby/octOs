# The filesystem

## Drives

Drives can be set up similar to drives on DOS/Windows, but instead of
referencing them with letters (like DOS's A:, B:, etc.) they are identified
by numbers. For example, a path might look like:

    1:/myfolder/subfolder/hello.txt

Which would access whatever filesystem is present on drive 1. Drive number 0 is
reserved.

Since a "drive" can be various different mediums, like an ATA hard disk, an
ATAPI drive, a floppy disk, etc., a drive is represented by a generic drive
struct. A drive has the following properties (`drive_t`):

    - Physical sector size
    - Logical sector size
    - Type of drive
    - Filesystem ID (More about this later)
    - Any attributes

An array of open drives is maintained in an array, indexed on the drive number.

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

Filesystem table, indexed on filesystem id