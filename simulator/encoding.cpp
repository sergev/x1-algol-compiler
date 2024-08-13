#include "encoding.h"

//
// Internal encoding of symbols in X1 Algol compiler.
// Unused symbols are marked as ░.
// For details see: https://github.com/sergev/x1-algol-compiler/blob/main/doc/algol-encoding.md
//
static const char *algol_to_utf8[128] = {
    "0", "1", "2", "3", "4", "5", "6",  "7",  "8", "9",  "a", "b", "c", "d", "e", "f", // 0-15
    "g", "h", "i", "j", "k", "l", "m",  "n",  "o", "p",  "q", "r", "s", "t", "u", "v", // 16-31
    "w", "x", "y", "z", "░", "A", "B",  "C",  "D", "E",  "F", "G", "H", "I", "J", "K", // 32-47
    "L", "M", "N", "O", "P", "Q", "R",  "S",  "T", "U",  "V", "W", "X", "Y", "Z", "░", // 48-63
    "+", "-", "×", "/", "÷", "↑", ">",  "≥",  "=", "≤",  "<", "≠", "¬", "∧", "∨", "⊃", // 64-79
    "≡", "░", "░", "░", "░", "░", "░",  ",",  ".", "⏨",  ":", ";", "≔", " ", "░", "░", // 80-95
    "░", "░", "(", ")", "[", "]", "`",  "'",  "░", "░",  "░", "░", "░", "░", "░", "░", // 96-111
    "░", "░", "░", "░", "░", "░", "\t", "\n", "░", "\"", "?", "░", "░", "░", "░", "░", // 112-127
};

//
// Write character in Algol encoding to UTF-8 stream.
//
void algol_putc(int ch, std::ostream &out)
{
    out << algol_to_utf8[ch & 0177];
}
