![build status](https://github.com/sergev/x1-algol-compiler/actions/workflows/c-cpp.yml/badge.svg)

The goal of this project is to revive the historic Algol compiler written by Edsger Dijkstra in 1960 for the Electrologica X1 computer.

Sources downloaded from: https://gtoal.com/languages/algol60/x1algol/

Directories:

 * compiler - sources of Algol-60 compiler in Pascal and C
 * doc - documentation about X1 compiler, Electrologica X1 machine and Algol language
 * examples - sample code to compile and run
 * misc - helper stuff in progress
 * scripts - disassemblers, decoders and other useful scripts
 * simulator - implementation of a virtual X1-like machine
 * tests - unit tests for compiler and simulator

# Build

Compile the X1 Algol simulator from sources and install into /usr/local:

```
make
make install
```

Run tests:
```
make test
```
Expected output:
```
ctest --test-dir build/tests
...
      Start  1: arith.x1_to_ieee
 1/80 Test  #1: arith.x1_to_ieee ......................   Passed    0.07 sec
      Start  2: arith.ieee_to_x1
 2/80 Test  #2: arith.ieee_to_x1 ......................   Passed    0.00 sec
      Start  3: x1_machine.arith_add
...
80/80 Test #80: cli.print123_x1 .......................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 75

Total Test time (real) =   0.73 sec
```

# Examples

A few demos are available in the `examples` directory:

```
$ cd examples

$ x1sim hello.a60
Hello `Algol'

$ x1sim man_or_boy.a60
-67
```
