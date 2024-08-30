#!/bin/sh
#
# Show SUM routine.
# This example shoult print 55*20 = 1100.
#
x1algc sum.a60 ../library/sum.lib > sum.x1
x1sim sum.x1
