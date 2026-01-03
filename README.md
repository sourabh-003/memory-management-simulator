# Memory Management Simulator

This project implements a user-space memory management simulator that models
dynamic memory allocation in operating systems.

## Implemented Features
- Contiguous physical memory simulation
- First Fit, Best Fit, and Worst Fit allocation strategies
- Memory allocation and deallocation with block splitting
- Coalescing of adjacent free blocks
- Command-line interface for interaction
- Fragmentation and utilization statistics

## Build
Windows (MinGW):
mingw32-make
Run:
./memsim


## Notes
- Internal fragmentation is zero due to exact block splitting
- Buddy allocation, cache simulation, and virtual memory were considered optional
  and are not implemented

## Author
Sourabh Hukkeri
