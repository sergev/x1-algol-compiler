# The PORD's and the PARD's

The TRANSMARK logic translates PORD's into PARD's.
The PORD's are in the object program (in descending order,
starting at the return address = 3, see pg 10) and each cover 1 word,
the PARDs derived from this are placed in the stack in ascending order
and each cover two words (see pg 7).

The structure of a PORD is as follows:

|26 25 24 23 22|21|20 19|18|17 16|15|14 - 0|
| ------------ |--| --- |--| --- |--| ---- |
|      bn      |0 |  Q  |0 |  t  |0 | addr |

If t=0, then the 15 bits of addr are a physical address
(quantity from block 0, start address of procedure or implicit subroutine);
the five bits of bn are then irrelevant and are =0.

The value t=2 does not occur.

If t=1 or 3, then the bits of addr are a position with respect to a PP,
which is indicated by the bits of bn and in the first instance
the physical address PP[bn] + addr, which
indicates a quantity in the stack, is built up.
(This is therefore the normal "dynamic address" with the difference that
the bits for position and block number are in a different position)

If t=1, then the address thus formed is continued
as in the case t=0 with the address given directly in addr.

If t=3, - i.e. the current parameter is at the place where
it is given a formal, the so-called, "passing on of a formal
parameter - then the address thus derived guaranteedly points to one PARD,
and this PARD is passed on without further ado
(so (PP[bn]+addr) and (PP[bn]+addr+1) are together the resulting PARD).

If t=3, then the bits of Q are irrelevant, and are =0.

How this physical address should be interpreted is indicated in the bits of Q,

 * Q=0 address of real number
 * Q=1 address of integer (or boolean)
 * Q=2 start address of procedure or implicit subroutine without or with numerical result
 * Q=3 start address of implicit subroutine with an address as result,
