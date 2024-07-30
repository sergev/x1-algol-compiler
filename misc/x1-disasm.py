#!/usr/bin/env python3
#
# Disassemble X1 code.
#
import sys

if len(sys.argv) != 2:
    print("Usage: x1-disasm.py input.txt")
    sys.exit(1)

inputfile = sys.argv[1]

def print_instruction(addr, opcode):
    print(f"{addr:05o}: {opcode >> 21 :02o} {opcode >> 15 & 0o77:02o} {opcode & 0o77777 :05o}")

with open(inputfile, 'r') as f:
    for line in f.readlines():
        #print(line)
        word = line.split()
        if len(word) == 2:
            print_instruction(int(word[0]), int(word[1]))
