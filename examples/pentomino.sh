#!/bin/sh
#
# Run pentomino solver.
#
x1sim pentomino.a60 << END_OF_INPUT
12
5
12
63
1   1   -9 1 2 11

2   4   1 2 10 12
        1 10 20 21
        2 10 11 12
        1 11 20 21

3   4   10 11 12 20
        -8 1 2 12
        1 2 11 21
        -19 -9 1 2

4   4   10 11 12 22
        1 11 21 22
        -8 1 2 10
        -19 -18 -9 1

5   4   1 2 10 20
        10 20 21 22
        -18 -8 1 2
        1 2 12 22

6   4   10 11 21 22
        -9 -8 1 10
        1 11 12 22
        -18 -9 -8 1

7   2   1 2 3 4
        10 20 30 40

8   8   -8 1 2 11
        -9 1 11 12
        -9 1 2 10
        1 11 12 21
        10 11 12 21
        -9 -8 1 11
        -9 1 2 12
        -19 -9 -8 1

9   8   10 11 21 31
        -8 -7 1 2
        10 20 21 31
        -9 -8 -7 1
        1 11 12 13
        -9 1 10 20
        1 2 12 13
        -19 -9 1 10

10  8   10 11 20 30
        1 2 3 12
        -19 -9 1 11
        -9 1 2 3
        1 2 3 11
        -9 1 11 21
        -8 1 2 3
        10 20 21 30

11  8   1 2 3 10
        10 20 30 31
        1 10 20 30
        1 2 3 13
        10 11 12 13
        -29 -19 -9 1
        -7 1 2 3
        1 11 21 31

12  8   1 10 11 20
        1 10 11 12
        1 2 10 11
        1 10 11 21
        10 11 20 21
        -9 1 10 11
        1 2 11 12
        -9 -8 1 2

END_OF_INPUT
