#include "x1_arch.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

//
// Convert real value into X1 format.
//
Real ieee_to_x1(const double input)
{
    //TODO
    return 0;
}

double x1_to_ieee(Real word)
{
    //TODO
    return 0.0;
}

//
// Print CPU instruction with mnemonics.
//
void x1_print_instruction(std::ostream &out, unsigned cmd)
{
    //TODO
    x1_print_word_octal(out, cmd);
}

//
// Print word as octal.
//
void x1_print_word_octal(std::ostream &out, Word value)
{
    auto save_flags = out.flags();

    out << std::oct << std::setfill('0') << std::setw(2) << ((int)(value >> 21) & BITS(6)) << ' ';
    out << std::setfill('0') << std::setw(2) << ((int)(value >> 15) & BITS(6)) << ' ';
    out << std::setfill('0') << std::setw(5) << ((int)value & BITS(15));

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

//
// Negate integer value.
//
Word x1_negate_int(Word value)
{
    value ^= BITS(27);
    return value & BITS(27);
}

//
// Negate real value.
//
Real x1_negate_real(Real value)
{
    value ^= BITS(54);
    return value & BITS(54);
}

//
// Print integer value.
//
void x1_print_integer(std::ostream &out, Word value)
{
    if (value & ONEBIT(26)) {
        out << '-';
        value ^= BITS(27);
    }
    value &= BITS(26);
    out << value;
}

//
// Print real value.
//
void x1_print_real(std::ostream &out, Real value)
{
    //TODO
    if (value & ONEBIT(26)) {
        out << '-';
        value ^= BITS(54);
    }
    value &= BITS(54);
    out << value;
}
