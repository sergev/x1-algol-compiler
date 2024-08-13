#include "encoding.h"

//
// Write character in Algol encoding to UTF-8 stream.
//
// For details see: https://github.com/sergev/x1-algol-compiler/blob/main/doc/algol-encoding.md
//
void algol_putc(int ch, std::ostream &out)
{
    ch &= 0177;

    switch (ch) {
    case 0 ... 9:
        out << char(ch + '0');
        break;
    case 10 ... 35:
        out << char(ch - 10 + 'a');
        break;
    case 37 ... 62:
        out << char(ch - 37 + 'A');
        break;
    case 64:
    case 65:
    case 67:
    case 70:
    case 72:
    case 74:
        out << "+-!/!!>!=!<"[ch - 64];
        break;
    case 68:
    case 69:
    case 71:
    case 73:
    case 75 ... 80: {
        static const char *c[] = { "÷", "↑", "", "≥", "", "≤", "", "≠", "¬", "∧", "∨", "⊃", "≡" };
        out << c[ch - 68];
        break;
    }
    case 87:
    case 88:
    case 90:
    case 91:
    case 93:
        out << ",.!:;! "[ch - 87];
        break;
    case 89:
        out << "⏨";
        break;
    case 98 ... 101:
        out << "()[]"[ch - 98];
        break;
    case 102:
        out << "‘";
        break;
    case 103:
        out << "’";
        break;
    case 118:
        out << '\t';
        break;
    case 119:
        out << '\n';
        break;
    case 121:
        out << '"';
        break;
    case 122:
        out << '?';
        break;
    }
}
