This directory contains a few examples of programs written in Algol 60,
collected from various sources.

  * [beer.a60](beer.a60)
  * [bulls_and_cows.a60](bulls_and_cows.a60)
  * [disarium.a60](disarium.a60)
  * [hanoi.a60](hanoi.a60)
  * [hello.a60](hello.a60)
  * [lisp.a60](lisp.a60)
  * [magic_square.a60](magic_square.a60)
  * [man_or_boy.a60](man_or_boy.a60)
  * [mersenne.a60](mersenne.a60)
  * [outer_planets.a60](outer_planets.a60)
  * [palindromic_primes.a60](palindromic_primes.a60)
  * [pentomino.a60](pentomino.a60)
  * [perfect_numbers.a60](perfect_numbers.a60)
  * [quinio.a60](quinio.a60)
  * [sieve.a60](sieve.a60)
  * [sqrt_test.a60](sqrt_test.a60)
  * [steady_squares.a60](steady_squares.a60)
  * [sum.a60](sum.a60)
  * [time.a60](time.a60)
  * [whetstone.a60](whetstone.a60)

# beer.a60

"99 Bottles of Beer on the Wall" written in Algol60, for the a60 interpreter by Erik Schoenfelder.

    $ x1sim beer.a60
    99 bottles of beer on the wall, 99 bottles of beer
    take one down and pass it around, 98 bottles of beer on the wall.
    98 bottles of beer on the wall, 98 bottles of beer
    take one down and pass it around, 97 bottles of beer on the wall.
    ...
    2 bottles of beer on the wall, 2 bottles of beer
    take one down and pass it around, 1 bottle of beer on the wall.
    1 bottle of beer on the wall, 1 bottle of beer
    take one down and pass it around, no more bottles of beer on the wall.

# bulls_and_cows.a60

Game [bulls and cows](https://en.wikipedia.org/wiki/Bulls_and_cows).

    $ x1sim bulls_and_cows.a60
    Welcome to bulls and cows

    I choose a number made of 4 digits (from 0 to 9) without repetitions.
    You enter a number of 4 digits, and I say you how many of them
    are in my secret number but in wrong position (cows),
    and how many are in the right position (bulls).

    Guess a number.
    Enter number: 3210
    You scored 0 bulls and 2 cows.
    Guess a number.
    Enter number: 5304
    You scored 0 bulls and 2 cows.
    Guess a number.
    Enter number: 4631
    You scored 0 bulls and 3 cows.
    Guess a number.
    Enter number: 1473
    You scored 1 bulls and 2 cows.
    Guess a number.
    Enter number: 8143
    You scored 3 bulls and 0 cows.
    Guess a number.
    Enter number: 9143
    You scored 4 bulls and 0 cows.
    Correct. That took you 6 guesses.

# disarium.a60

Compute disariums. A number is called a disarium if the sum of its digits raised to a power with their corresponding positions is equal to the number itself. For example, 135 = 1<sup>1</sup> + 3<sup>2</sup> + 5<sup>3</sup>.

    $ x1sim disarium.a60
     0 1 2 3 4 5 6 7 8 9 89 135 175 518 598 1306 1676 2427

# hanoi.a60

Solve the [Tower of Hanoi](https://en.wikipedia.org/wiki/Tower_of_Hanoi) puzzle.

    $ x1sim hanoi.a60
    move 1 --> 3
    move 1 --> 2
    move 3 --> 2
    move 1 --> 3
    move 2 --> 1
    move 2 --> 3
    move 1 --> 3

# hello.a60

Print simple text.

    $ x1sim hello.a60
    Hello `Algol'

# lisp.a60

Implementation of Lisp by F.E.J. Kruseman Aretz, March 28, l988.
The sources were published in article
["A comparison between the ALGOL 60 implementations on the Electrologica X1 and the Electrologica X8"](https://ir.cwi.nl/pub/13677).
A demo program written in Lisp solves the classic
[missionaries and cannibals problem](https://en.wikipedia.org/wiki/Missionaries_and_cannibals_problem).
Aretz has another article about it:
[On deriving a Lisp program from its specification](https://core.ac.uk/download/pdf/82584195.pdf).

See invocation and output on page: [lisp.md](lisp.md)

# magic_square.a60

Calculate the magic square 8x8.

    $ x1sim magic_square.a60
    magic square, n = 8

      1  2 62 61 60 59  7  8
      9 10 54 53 52 51 15 16
     48 47 19 20 21 22 42 41
     40 39 27 28 29 30 34 33
     32 31 35 36 37 38 26 25
     24 23 43 44 45 46 18 17
     49 50 14 13 12 11 55 56
     57 58  6  5  4  3 63 64

    magic constant = 260

# man_or_boy.a60

Run famous test [Man or Boy](https://en.wikipedia.org/wiki/Man_or_boy_test) by Donald Knuth.

    $ x1sim man_or_boy.a60
    -67

# mersenne.a60

Looking for Mersenne primes. This is when the number 2<sup>N</sup>-1 turns out to be prime.

    $ x1sim mersenne.a60
    Searching to M(19) for Mersenne primes
    M(3) : 7
    M(5) : 31
    M(7) : 127
    M(13) : 8191
    M(17) : 131071
    M(19) : 524287

# outer_planets.a60

Calculate the orbits of the outer planets of the solar system,
from the [PhD thesis](https://ir.cwi.nl/pub/27822/27822.pdf) of Zonneveld.
Algol-60 sources taken from article
[The Dijkstra–Zonneveld ALGOL 60 compiler for the Electrologica X1](https://gtoal.com/languages/algol60/x1algol/Electrologica_X1.pdf)
by F.E.J. Kruseman Aretz.

See invocation and output on page: [outer_planets.md](outer_planets.md)

# palindromic_primes.a60

Let's calculate prime palindromic numbers.
This is when a number is read the same from right to left and from left to right.
Note the two 'evil' palindromes, which have the "number of the beast" 666 inside.

    $ x1sim palindromic_primes.a60
    Computing primes up to 100000...
    Palindromic primes: 2 3 5 7 11 101 131 151 181 191 313 353 373 383 727 757
    787 797 919 929 10301 10501 10601 11311 11411 12421 12721 12821 13331 13831
    13931 14341 14741 15451 15551 16061 16361 16561 16661 17471 17971 18181
    18481 19391 19891 19991 30103 30203 30403 30703 30803 31013 31513 32323
    32423 33533 34543 34843 35053 35153 35353 35753 36263 36563 37273 37573
    38083 38183 38783 39293 70207 70507 70607 71317 71917 72227 72727 73037
    73237 73637 74047 74747 75557 76367 76667 77377 77477 77977 78487 78787
    78887 79397 79697 79997 90709 91019 93139 93239 93739 94049 94349 94649
    94849 94949 95959 96269 96469 96769 97379 97579 97879 98389 98689

# pentomino.a60

Solve the [pentomino puzzle](https://en.wikipedia.org/wiki/Pentomino#Games).
The sources were published in article
["A comparison between the ALGOL 60 implementations on the Electrologica X1 and the Electrologica X8"](https://ir.cwi.nl/pub/13677).

Solutions for 6x10, 5x12, 4x15 and 3x20 puzzles are shown on page: [pentomino.md](pentomino.md)

# perfect_numbers.a60

Let's find perfect numbers. Such a number is equal to the sum of its divisors.

    $ x1sim perfect_numbers.a60
    Searching up to 10000 for perfect numbers: 6 28 496 8128
    4 were found.

# quinio.a60

Quinio is a variation of the Japanese game [Gomoku](https://en.wikipedia.org/wiki/Gomoku).
It is an interactive game for human playing against the computer.
The game is about Noughts and Crosses (Dutch: butter, cheese and eggs) on a 13x13 field.
Developed by L. Meertens in September 1964 for the Electrologica X1.
Sources were obtained from [CWI Software Archive](https://github.com/cwi-software-archive/Algol60/tree/main/Quinio),
extracted from paper punched tape.

See invocation on page: [quinio.md](quinio.md)

# sieve.a60

Sieve of Eratosthenes: calculating prime numbers up to a million. The trick is that the original Electrologica X1 machine had only 32 kilowords of memory, and such a program would not fit. But on the simulator we made the stack 1 megaword, why not.

    $ cd x1-algol-compiler/examples
    $ x1sim sieve.a60
    Computing primes.............
    Number of primes below 1000 000: 78498

# sqrt_test.a60

Check the sqrt() elementary function: make sure it returns accurate values
for all integer arguments. From article
["Design and correctness proof of an emulation of the floating-point operations of the Electrologica X8. A case study"](https://pure.tue.nl/ws/portalfiles/portal/3023549/674735.pdf)
by F.E.J. Kruseman Aretz.

    $ x1sim sqrt_test.a60
      100000 passed
      200000 passed
      300000 passed
      400000 passed
      500000 passed
      600000 passed
      700000 passed
      800000 passed
      900000 passed
     1000000 passed

# steady_squares.a60

Let's calculate steady squares. This is when the square of a number ends with the same digits.

    $ x1sim steady_squares.a60
    Searching up to 10000 for steady squares:
     1 5 6 25 76 376 625

# sum.a60

Check the SUM() library function. It implements the famous
[Jensen's device](https://en.wikipedia.org/wiki/Jensen%27s_device).

    $ x1sim sum.a60
    1100

# time.a60

Check the TIMEOFDAY() library function. It returns obtains the current
local time from the host computer, with millisecond precision.

    $ x1sim time.a60
     19 : 38 : 39.562

And a little bit later:

    $ x1sim time.a60
     19 : 39 :  4.231

# whetstone.a60

Calculate one iteration of famous [Whetstone Benchmark](https://en.wikipedia.org/wiki/Whetstone_(benchmark)).
Just to make sure X1 arithmetics works as expected.

    $ x1sim whetstone.a60
    0 0 0 1 -1 -1 -1
    120 140 120 -0.0683421986223 -0.462637656321 -0.7297183878991 -1.123979070167
    140 120 120 -0.05533645260607 -0.447436563164 -0.7109733896477 -1.103098057683
    3450 1 1 1 -1 -1 -1
    2100 1 2 6 6 -0.7109733896477 -1.103098057683
    320 1 2 0.490407316182 0.490407316182 0.490392497979 0.490392497979
    8990 1 2 1 1 0.9999375006255 0.9999375006255
    6160 1 2 3 2 3 -1.103098057683
    0 2 3 1 -1 -1 -1
    930 2 3 0.8346655195446 0.8346655195446 0.8346655195446 0.8346655195446
