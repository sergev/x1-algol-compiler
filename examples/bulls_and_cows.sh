#!/bin/sh
#
# Play bulls-and-cows game.
#
x1algc bulls_and_cows.a60 ../library/random.lib ../library/setrandom.lib > bulls_and_cows.x1
x1sim bulls_and_cows.x1
