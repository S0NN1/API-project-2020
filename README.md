# Algorithms and Principles of Computer Science Project

Final project for the Algorithm and Principles of Computer Science Course.

## Requirements

Implementation of an editor, including basic commands:

- **`addr1,addr2c`** replaces or adds strings based on specified addresses, new strings can only be added if addr1 is the first available address;

- **`addr1,addr2d`** deletes strings based on specified addresses, if string doesn't exist then command has no effect, but it's still counted towards undo / redo commands;

- **`addr1,addr2p`** prints strings from addr1 to addr2, prints "." if string is empty / invalid;

- **`nu`** undo _n_ commands (change / delete only);

- **`nr`** redo _n_ commands (change / delete only);

- **`q`** kills program.

## Test cases

| Task            | Commands   | Time Limit | Memory limit |
| --------------- | ---------- | ---------- | ------------ |
| WriteOnly       | c          | 7.4 s      | 3.00 GiB     |
| BulkReads       | c          | 1.250 s    | 240 MiB      |
| TimeForAChange  | c, d       | 2.100 s    | 540 MiB      |
| AlteringHistory | c, d, u    | 2.100 s    | 400 MiB      |
| RollingBack     | c, d, u, r | 2.400 s    | 550 MiB      |
| RollerCoaster   | c, d, u, r | 2.700 s    | 1.03 GiB     |
| Laude           | c, d, u, r | 2.000 s    | 340 MiB      |

## Execution example

![kek](github/Blank%20Diagram.svg)

## Tools used

- Valgrind;
- Callgrind;
- Massif-Visualizer;
- Address-Sanitizer;
- GDB;
- GCC.

## Software used

- Clion;
- Visual Studio Code.
