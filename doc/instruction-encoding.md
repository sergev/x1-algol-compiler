(page 134 from Dijkstra's PhD thesis)

#﻿ Appendix 2. Binary representation of orders

The digits of the order word are numbered

              d26 d25 . . . d1 d0

The name of the order (function letter(s) and function number) determines the contents of the six most significant digits d26…d21 of the order word. The number formed by these six binary digits can run from 0 to 63; its value is found by adding a number of times eight to the function number. The multiple of eight is determined by the function letter(s) and is given in the table below.

        A:  0            LS:  3½
        S:  1            B :  4
        X:  2            T :  5
       LA:  2½           Y :  6
        D:  3            Z :  7

The value of the next six bits is determined by the variants:
```
Address modification Condition-setting Condition reaction
          d20 d19              d18 d17            d16 d15
normally   0  0      normally   0  0   normally    0  0
   A       0  1         P       0  1      U        0  1
   B       1  0         Z       1  0      Y        1  0
   C       1  1         E       1  1      N        1  1
```
The bits d14…d0 give the binary representation of the address.

According to the above rules d21 would be = 0 in counting and subroutine Jumps (4T and 6T); however, it is used for the index m, more precisely, if the binary digits of the index m are b2 b1 b0 (for 4T) or b3 b2 b1 b0 (for 6T), then the following holds.
```
                  d21 = b2
                  d20 = b1
                  d19 = b0
and for 6T:       d18 = b3
```
(In the subroutine jump, the binary digit b3 is therefore placed at the least significant side.)

The binary representation of the P-orders is given in the table below (the special paragraph letter C causes an increase of 214 in the address: 0 C0 = 16384 X0).
