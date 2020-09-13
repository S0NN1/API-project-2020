# Algorithms and Principles of Computer Science Project

Final project for the Algorithm and Principles of Computer Science Course.

## Requirements

Implementation of an editor, including basic commands:
* ```addr1,addr2c``` replaces or adds strings based on specified addresses, new strings can only be added if addr1 is the first available address;
* ```addr1,addr2d``` deletes strings based on specified addresses, if string doesn't exist then command has no effect, but it's still counted towards undo / redo commands;
* ```addr1,addr2p``` prints strings from addr1 to addr2, prints "." if string is empty / invalid;
* ```nu```  undo _n_ commands;
* ```nr``` redo _n_ commands;
* ```q```  kills program.

## Tools used

* Valgrind;
* Callgrind;
* Massif-Visualizer;
* Address-Sanitizer;
* GDB;
* GCC.

## Software used

* Clion;
* Visual Studio Code.