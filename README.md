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

### IF YOU'RE USING CYGWIN ON WINDOWS

If you'd like an editor amenable to this kind of process, consider installing
(vscode)[https://code.visualstudio.com/].

You'll need to install (cygwin)[https://cygwin.com/install.html], and while installing you need to
make sure you select the packages

`make`,`git`, `gcc-core` and  `gcc-g++` from cygwins package selections.

YOU MUST MAKE SURE gcc-core AND gcc-g++ ARE THE SAME VERSION!

1) Open cygwin, you should get a terminal.

2) Make sure you have the necessary tools:

`which make`

`which git`

`which gcc`

Each of those commands should give you a path showing you that 
an executable exists. If they don't, install cygwin again and make
sure to install the packages listed above.

3) In cygwin, make a new directory to hold your software projects in.
Mine is just called "Code", but whatever you want:

`mkdir Code`

4) switch into that directory, and use git to clone the project from
github:

`git clone https://github.com/evizitei/aero123_trifan.git`

5) change into the root of that directory and make sure you can build:

`cd aero123_trifan`

`make build`

Then you should be able to do the commands that follow below just like a mac or linux user.

#### End Windows-only Section

### Building and Running!

If you want to remove any existing build artifacts you can use:

`make clean`

To execute the flight controller, you can use

`make run`

which will re-build the binary and run it.
Four threads will be started: one to accept
command input, one to run the simulator
for feeding position changes into the GPS,
one for processing state changes and decisions in
the flight controller, and one to log the status of the
system to the flight log on disk.

The CLI will provide you with commands you can use to "control"
the flight.

To see the log after a flight, you can read through `simulator.log` and `flight.log`,
or alternatively tail the log while the flight is in progress
in another console:

`tail -f simulator.log`

The simulator log has both the status of each device, and also the
inferred values of things like heading and AOA from the simulator
status, which is useful for figuring out how control changes
translated into flight status changes.  The simulator.log has everything
in the flight.log.


## Running tests

To avoid external dependencies, tests are just executed
as assert statements factored into methods.

All tests that currently exist for the project can be run with
the make task:

`make test`
