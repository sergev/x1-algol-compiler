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
        Start   1: arith.x1_to_ieee
  1/105 Test   #1: arith.x1_to_ieee ........................   Passed    0.01 sec
        Start   2: arith.ieee_to_x1
  2/105 Test   #2: arith.ieee_to_x1 ........................   Passed    0.01 sec
        Start   3: x1_machine.arith_add
...
105/105 Test #105: cli.lib_sum .............................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 105

Total Test time (real) =   1.39 sec
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

See full list on page [examples/README.md](examples/README.md).

# Known bugs

## Misplaced parentheses in the Pascal version of the compiler

The line `1415 arrc:= arrc - (arrd * store[tlsc-1]) mod d26;` as per the Kruseman Aretz paper, must be
`arrc:= (arrc - arrd * store[tlsc-1]) mod d26;` to ensure correct computation of *storage functions* of multi-dimensional arrays.

This is a Pascal transcription artifact.

## Mistakenly disabled generation of *passing of formals* descriptors

Formal parameters of procedures directly passed as actual parameters to other procedures produce thunks (*implicit subroutines*) unnecessarily.

The lines
```
1074     then if (store[tlsc-2] = rlsc)
1075         and (fflag = 0) and (jflag = 0) and (nflag = 1)
1076         then begin if nid > nlscop
1077             then begin if (pflag = 1) and (fflag = 0)
```
Have two checks for `fflag = 0` in nested `if` conditions. One of the checks is redundant.
Removing the first one, in line 1075, restores the ability to generate descriptors allowing a more efficient parameter passing.
This saves about 15% of stack depth for the "Man or boy" test.

It is unclear if this is a Pascal transcription artifact or a genuine performance regression in the original compiler.

## Passing predefined functions as procedure arguments sometimes results in bad code

A numerical actual parameter of a procedure call immediately following a name of a predefined function, e.g. *sin* or *cos*, results in garbage object code.
The check `nid > nlscop` in line 1076 *supra* fails because the variable `nid` still holds the ID of a predefined function when the next parameter
is not an expression and does not contain an identifier, resulting in processing the number as if it were a predefined function.

A fix is to add a flag which is set when such a name is encountered first (line 1104), and is used to trigger resetting `nid` to an arbitrary number greater than
the predefined function boundary (`nlscop`) at the end of `production_of_object_program`.
