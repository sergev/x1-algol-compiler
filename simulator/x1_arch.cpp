#include "x1_arch.h"

#include <bitset>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

//
// Create real value from two words.
//
Real x1_words_to_real(Word hi, Word lo)
{
    return ((Real)hi << 27) | lo;
}

//
// Convert long double value into X1 format.
// Always return normalized number (when non-zero).
//
// Representation of real numbers in Electrologica X1:
//  First word:
//              26   25————————————————0
//             sign  mantissa upper bits
//  Second word:
//              26   25——————12 11—————0
//             sign   mantissa  exponent
//         (ignored) lower bits  +04000
//
Real ieee_to_x1(long double input)
{
    const bool negate_flag = std::signbit(input);
    if (negate_flag) {
        input = -input;
    }

    // Split into mantissa and exponent.
    int exponent;
    long double mantissa = frexpl(input, &exponent);
    if (mantissa == 0.0) {
        // Either -0.0 or +0.0.
        return negate_flag ? BITS(54) : 0;
    }

    // Multiply mantissa by 2^40.
    mantissa = ldexpl(mantissa, 40);

    // Positive value in range [0.5, 1) * 2⁴⁰
    // Get 40 bits of mantissa.
    auto result = (Real)mantissa;
    if (mantissa - result >= 0.5) {
        // Rounding.
        result += 1;
        if (result == 1ull << 40) {
            // Normalize.
            result >>= 1;
            exponent += 1;
        }
    }
    if (exponent > 03777) {
        // Overflow: return maxreal.
        return negate_flag ? 0'40'00'00000'40'00'00000ull :
                             0'37'77'77777'37'77'77777ull;
    }
    if (exponent < -04000) {
        // Underflow: return -0.0 or +0.0.
        return negate_flag ? BITS(54) : 0;
    }

    // Put mantissa in place.
    result = (result >> 14 << 27) | ((result & BITS(14)) << 12);

    // Add exponent with offset.
    result |= exponent + 04000;
    if (negate_flag) {
        result ^= BITS(54);
    }
    return result;
}

//
// Convert real value from X1 format to long double.
//
long double x1_to_ieee(Real word)
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
