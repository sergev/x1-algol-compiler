#include "x1_arch.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

//
// Convert real value into X1 format.
//
Word ieee_to_x1(const double input)
{
    //TODO
    return 0;
}

double x1_to_ieee(Word word)
{
    //TODO
    return 0.0;
}

//
// Print CPU instruction with mnemonics.
//
void x1_print_instruction_mnemonics(std::ostream &out, unsigned cmd)
{
    //TODO
    x1_print_instruction_octal(out, cmd);
}

//
// Print CPU instruction as octal number.
//
void x1_print_instruction_octal(std::ostream &out, unsigned cmd)
{
    auto save_flags = out.flags();

    out << std::oct << std::setfill('0') << std::setw(2) << (cmd >> 21) << ' ';
    out << std::setfill('0') << std::setw(2) << ((cmd >> 15) & 077) << ' ';
    out << std::setfill('0') << std::setw(5) << (cmd & BITS(15));

    // Restore.
    out.flags(save_flags);
}

//
// Print word as octal.
//
void x1_print_word_octal(std::ostream &out, Word value)
{
    auto save_flags = out.flags();

    out << std::oct << ((int)(value >> 24) & BITS(3)) << ' ';
    out << std::setfill('0') << std::setw(4) << ((int)(value >> 12) & BITS(12)) << ' ';
    out << std::setfill('0') << std::setw(4) << ((int)value & BITS(12));

    // Restore.
    out.flags(save_flags);
}

//
// Print word as bytes.
//
void x1_print_word_bytes(std::ostream &out, Word value)
{
    auto save_flags = out.flags();

    out << std::oct << ((int)(value >> 24) & BITS(3)) << ' ';
    out << std::setfill('0') << std::setw(3) << ((int)(value >> 16) & BITS(8)) << ' ';
    out << std::setfill('0') << std::setw(3) << ((int)(value >> 8) & BITS(8)) << ' ';
    out << std::setfill('0') << std::setw(3) << ((int)value & BITS(8));

    // Restore.
    out.flags(save_flags);
}

//
// Convert number to string as octal.
//
std::string to_octal(unsigned val)
{
    std::ostringstream buf;
    buf << std::oct << val;
    return buf.str();
}
