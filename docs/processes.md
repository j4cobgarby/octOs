# Processes in oct

## Overview

Processes in oct can be in three states: running, runnable, and blocked.

 - A running process is a process which is currently being executed by the processor.
 - A runnable process isn't currently being executed, but is ready to be.
 - A blocked process isn't able to run currently, even if the processor wasn't doing anything else.

## Process table

The process table is an area of memory which stores a process table entry structure for every current process.
The process table is pointed to by the memory variable `ptable_base`, and the amount of processes is in
`ptable_processes`. A process table entry looks like this:

| Offset (Bytes) | Value | Notes |
| --- | --- | --- |
| 0 | eax (4 bytes) | |
| 4 | ebx (4 bytes) | |
| 8 | ecx (4 bytes)| |
| 12 | edx (4 bytes) | |
| 16 | cs (2 bytes) | |
| 18 | ss (2 bytes) | |
| 20 | ds (2 bytes) | |
| 22 | ss (2 bytes) | |
| 24 | eip (4 bytes) | |
| 28 | esp (4 bytes) | |
| 32 | ebp (4 bytes) | |
| 36 | state (1 byte) | 0 = blocked, 1 = runnable, 2 = running |
| 37 | pid (2 bytes) | ID used to refer to the process |
| 39 | parent pid (2 bytes) | Parent process |
| 41 | priority (1 byte) | Represents importance of process, in scheduling |

This table is used to store information about all processes, including the value of all registers in that process,
and information to do with the scheduling algorithm.