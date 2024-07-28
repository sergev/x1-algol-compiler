(pages [134](x1-binary-instructions.png)-[135](x1-binary-p-orders.png) from Dijkstra's PhD thesis)

# Appendix 2. Binary representation of orders

The digits of the order word are numbered

              d₂₆ d₂₅ . . . d₁ d₀

The name of the order (function letter(s) and function number) determines the contents of the six most significant digits d₂₆…d₂₁ of the order word. The number formed by these six binary digits can run from 0 to 63; its value is found by adding a number of times eight to the function number. The multiple of eight is determined by the function letter(s) and is given in the table below.

        A:  0            LS:  3½
        S:  1            B :  4
        X:  2            T :  5
       LA:  2½           Y :  6
        D:  3            Z :  7

The value of the next six bits is determined by the variants:
```
Address modification Condition-setting Condition reaction
          d₂₀ d₁₉              d₁₈ d₁₇            d₁₆ d₁₅
normally   0  0      normally   0  0   normally    0  0
   A       0  1         P       0  1      U        0  1
   B       1  0         Z       1  0      Y        1  0
   C       1  1         E       1  1      N        1  1
```
The bits d₁₄…d₀ give the binary representation of the address.

According to the above rules d₂₁ would be = 0 in counting and subroutine Jumps (4T and 6T); however, it is used for the index m, more precisely, if the binary digits of the index m are b₂ b₁ b₀ (for 4T) or b₃ b₂ b₁ b₀ (for 6T), then the following holds.
```
                  d₂₁ = b₂
                  d₂₀ = b₁
                  d₁₉ = b₀
and for 6T:       d₁₈ = b₃
```
(In the subroutine jump, the binary digit b₃ is therefore placed at the least significant side.)

The binary representation of the P-orders is given in the table below (the special paragraph letter C causes an increase of 2¹⁴ in the address: 0 C0 = 16384 X0).
```
Circuit    0P n     1P n    2P n    3P n    4P      5P      6P      7P

   AA     6Y nX0   7Y nX0  6Y nX1  7Y nX1  6Y 0X8  7Y 0X8  6Y 0X5   -
   AS     6Y nX2   7Y nX2  6Y nX3  7Y nX3  6Y 1X8  7Y 1X8  6Y 0X7   -
   AB        -        -       -       -    6Y 2X8  7Y 2X8     -     -
   SA     6Z nX2   7Z nX2  6Z nX3  7Z nX3  6Y 0X9  7Y 0X9  6Y 0X5   -
   SS     6Z nX0   7Z nX0  6Z nX1  7Z nX1  6Y 1X9  7Y 1X9  6Z 0X5   -
   SB        -        -       -       -    6Y 2X9  7Y 2X9     -     -
   BA        -        -       -       -    6Y 0X10 7Y 0X10    -     -
   BS        -        -       -       -    6Y 1X10 7Y 1X10    -     -
   BB        -        -       -       -    6Y 2X10 7Y 2X10    -     -

    -        -        -       -       -       -       -       -  7Y 0C0
```
