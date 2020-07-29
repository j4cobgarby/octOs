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
| 0 | eax | |
| 4 | ebx | |
| 8 | ecx | |
| 12 | edx | |
| 16 | cs | |
| 18 | ss | |
| 20 | ds | |
| 22 | ss | |
| 24 | eip | |
| 28 | esp | |
| 32 | ebp | |
| 36 | state | 0 = blocked, 1 = runnable, 2 = running |
| 37 | pid | ID used to refer to the process |
| 39 | parent pid | Parent process |
| 41 | priority | Represents importance of process, in scheduling |
