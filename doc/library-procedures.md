## 7. Library procedures

The provisional library of the ALGOL system for the X8 contains
the following groups of procedures, which do not need to be
declared in the program:

### 7.1. Elementary functions

### 7.2. Input procedures

As input device a tape reader is available, with a speed of
1000 punches/second for 5-, 7-, or 8-hole punched tape. There
are three input procedures for the tape reader:

**7.2.3.** `real procedure READ;`

The function designator READ has meaning only if the input tape
is punched according to the punching conventions for number
tapes, shown in 8. The value of READ is the value of the next
number on the tape, with a relative precision of approximately
12 decimal places, in absolute value between the limits 10^-616
and 10^628 or exactly 0. Integers on the tape, in absolute
value smaller than 1 099 511 627 776, are entered exactly.

The relation <number> = READ is true if the decimal
representation of <number> in the ALGOL text and of the number
found by READ on the number tape are the same.

**7.2.4.** `real procedure read; read:= READ;`

`read` is just another name for READ.

### 7.3. Output procedures

For the time being, the following output devices are available:

  * a line printer, with a speed of 7 to 20 lines/second. Per page, 60 lines with a width of 144 positions are available,
  * a tape punch for 7-hole tape with a speed of 150 heptades/second.

The use of the tape punch as an output device is only recommended for outputting data that

  * must be read in again later,
  * must be published in a nicer typography than that of the line printer,
  * must be mechanically multiplied.

If you only have a few final answers, but also want to output
many intermediate results in order to be able to check them in
case of doubt, you can:

  * output the intermediate results on the line printer,
  * punch the final answers.

**7.3.1.2.** `procedure SPACE (n); value n; integer n;`

SPACE basically increases the position on the line by n. If the
number of symbols on the line threatens to exceed 144, the
system inserts as many line advances (each time decreasing the
position on the line by 144) as necessary to guarantee the
correct line width. SPACE (n) is equivalent to, but faster
than:

    begin integer i; for i := 1 step 1 until n do PRSYM (93) end;

**7.3.1.3.** `procedure TAB;`

TAB in principle increases the position on the line by at least
2 and at most 9, so that an 8-fold is achieved (the positions
on the line are numbered from 0 to 143. The "tab stops" are
therefore located at the positions numbered 8, 16, 24, ... 136).
If the line width is exceeded, a line advance is inserted
and the position on the line is reduced by 144.

**7.3.1.4.** `procedure NLCR;`

NLCR provides a line feed and resets the position on the line
to 0. If executing NLCR would cause the number of lines on the
page to exceed 60, a transition to a new page is accomplished
instead of a line feed.

**7.3.1.8.** `procedure ABSFIXT (n,m,x); value n,m,x; integer n,m; real x;`

The absolute value of x is generally printed by the procedure
ABSFIXT in fixed-point representation with n digits before the
decimal point, m digits after it, the whole preceded and
followed by a space. If m = 0, printing of the decimal point is
suppressed. In the integer part of x, non-significant zeros are
replaced by spaces, except for the zero at the units position
when m = 0 (i.e., when the fractional part is missing).

The number to be printed is rounded exactly to the last decimal
place to be printed. If after this its absolute value is >
10^n, or if the relations n >= 0, m >= 0, n + m <= 21 are not
satisfied, the system replaces ABSFIXT (n,m,x) by FLOT (13,3,x).

A call to ABSFIXT(n,m,x) essentially increases the position on
the line by if m = 0 then n + 2 else n + m + 3. If this would
cause the number of symbols on the line to exceed 144, the
system will insert an NLCR before printing starts.

**7.3.1.9.** `procedure FIXT (n,m,x); value n,m,x; integer n,m; real x;`

The FIXT procedure differs from the ABSFIXT procedure only in
that instead of the space immediately preceding the first digit
or the decimal point, the sign of x (+ or -) is printed.

**7.3.1.10.** `procedure FLOT (n,m,x); value n,m,x; integer n,m; real x;`

The FLOT procedure prints the value of x in floating-point
representation. The sign of x and the decimal point are
followed by an n-digit mantissa, the symbol "10", the sign of
the decimal exponent, the absolute value of that exponent in m
digits (with insignificant zeros, except in the ones position,
replaced by spaces), and finally a space.

For x = 0, a mantissa 0 and a decimal exponent 0, both with the
correct number of digits, are printed.

For x ≠ 10, the decimal exponent is determined such that the
mantissa in absolute value is >= .1 and < 1. If the decimal
exponent thus obtained cannot be printed in m digits, or
if 1 <= n <= 13, 1 <= m <= 3 does not apply, FLOT (n,m,x) is
replaced by FLOT (13,3,x).

**7.3.1.11.** `procedure PRINT (x); value x; real x;`

If the absolute value of x is an integer less than
1,099,511,627,776, x is printed according to FIXT (13,0,x),
followed by 6 extra spaces. Otherwise, it is printed according
to FLOT (13,3,x). In both cases, the position on the line is
increased by 21, but if necessary, a NLCR is inserted first by
the system.

**7.3.1.12.** `procedure print (x); PRINT (x);`

`print` is just another name for PRINT.

**7.3.1.13.** `procedure PRINTTEXT (s); strings;`

The actual parameter in a call to PRINTTEXT may only be: a
string, or a formal identifier (the case of a "passed" formal
parameter), provided that the latter ultimately corresponds to
a string. The symbols of the string, stripped of the outer
string quotes, are printed, symbol by symbol. Whenever the line
width of 144 positions is in danger of being exceeded, the
system inserts an NLCR.

### 7.4. Console-driven input procedures

Only with the permission of, and in consultation with, the
contact person at the "console" may the procedures HAND and
XEEN be used, which enable the operator to influence the course
of the calculation by manually entering numbers during the
execution of programs. Use of HAND and XEEN excludes the
programs from the two-minute service and will be extremely
expensive because of the time the operator needs for his
decisions and actions.

**7.4.2.** `integer procedure XEEN (n); value n; integer n;`

The value of the function designator XEEN is a function of two
parameters; the first is nothing but the argument of XEEN, n,
which in principle differs with every call to XEEN, while the
second parameter is a system variable, also an integer, which
in principle remains unchanged from call to call of XEEN.

The value of XEEN is formed from these two parameters by "bit
by bit collation". The binary representation of the result of
this operation contains a 1 if and only if the corresponding
position in both operands also contains a 1. (In the X8, the
binary representation of a negative integer is obtained from
that of its opposite number by replacing all zeros with ones
and vice versa. The most significant bit thereby functions as a
sign bit. There are in principle two representations of the
integer 0: a row of 27 zeros, or a row of 27 ones. If the
argument of XEEN is 0, however, the binary representation of
this is always a row of 27 ones.)

At the first call to XEEN that is executed in the program, the
word XEEN is typed out on a new line of the operator's
teleprinter. The operator is thus given the opportunity to
enter the above-mentioned system variable in octal, which will
also be collated with the argument of XEEN at each subsequent
call to XEEN. For this purpose, imagine the 27 bits of a word
split into 9 groups of 3 bits each; each group of 3 bits is
considered an octal figure (0 to 7). The operator must now
receive these 9 octals from the programmer, written as an octal
number; any zeros in non-significant positions may be omitted.

There are two ways in which a value once entered into the system can be changed:

  * by the program, by calling the STOP procedure,
  * by the operator, at any time, by pressing the X key on the control teleprinter.

In both cases the program will continue calculating normally.
However, at the next call to XEEN that is effected in the
program, the operator will again be given the opportunity to
give the system a value. The procedure is then exactly as
described above.

**7.4.3.** `procedure STOP;`

The procedure statement STOP has the sole effect that the next
time XEEN is called, the previously specified value of the
system parameter with which the argument of XEEN is collated is
no longer used, but the operator is asked for a new value.

### 7.5. Miscellaneous procedures

**7.5.1.** `real procedure SUM (i, a, b, x);`

    value b;
    integer i, a, b;
    real x;
    begin
        real s;
        s := 0;
        for i := a step 1 until b do
            s := s + x;
        SUM := s
    end;

The function designator SUM returns the sum of the values of
the current expression corresponding to x, evaluated for values
of i = a...b. For b < a, SUM returns the value 0.

**7.5.3.** `integer procedure EVEN (n); value n; integer n;`

    EVEN := if n ÷ 2 × 2 = n then 1 else -1;

The function designator EVEN is equivalent to what is often
written in mathematical notation as (-1)^n.
