# aero123_trifan
Class project repository for group work in CISS 465, Summer 2019.

## Narrative Background

Aero123 (fake company) intends to produce a flight control package for the XTI scaled drone of its TriFan 
600 aircraft.  The goal is to have simple pilot support, allowing the scaled drone to operate as an 
unmanned vehicle that can be controlled with high level flight objectives translated into specific 
tactical flight control operations by the software package.  Because the TriFan 600 drone does not have 
any commercial application and cannot carry human passengers, the objective is to demonstrate the flight 
characteristics and capabilities of the TriFan 600 in a safe and professional manner.

## Project Structure

Each subdirectory has a specific purpose.

- **bin/** executables should end up here.  The build process should put the final executable here.

- **build/** this is for transient build objects like object files.  `make clean` should clear everything in this directory.

- **doc/** Any documentation that doesn't really belong in this readme can live in this directory.

- **include/** Any header files for inclusion need to be stored here.

- **src/** all the source files for the application should live here.

- **test/** any test files for the application source files can live in this directory.

## Project Building

This project strives to use no external libraries, and
has no special hardware drivers at the moment.  That means
you shoud not need any specificl compiler flags to build it.
Making a binary should be as easy as `make build`, which will
use `clang` to build the project.  For refence, this
has been tested with the following clang version:

```
Apple LLVM version 10.0.1 (clang-1001.0.46.4)
Target: x86_64-apple-darwin18.5.0
Thread model: posix
```

but any modern c++ compiler should do fine.

If you want to remove any existing build artifacts you can use:

`make clean`
