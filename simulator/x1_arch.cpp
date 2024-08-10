#include "x1_arch.h"
#include "opc.h"

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
        throw std::runtime_error("Overflow in real arithmetic");
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
long double x1_to_ieee(Real input)
{
    const bool negate_flag = input >> 53 & 1;
    if (negate_flag) {
        input ^= BITS(54);
    }

    // Rearrange so that sign bit becomes a sign of int64.
    // So mantissa equals real mantissa multiplied by 2**63.
    //
    const auto mantissa = (long double) (int64_t) (
        (input >> 27 << (64 - 27)) |
        (((input >> 12) & BITS(14)) << (37 - 14))
    );
    const int exponent = input & BITS(12);

    // Add exponent with offset.
    // Compensate for 63 bits in mantissa.
    auto result = ldexpl(mantissa, exponent - 04000 - 63);
    if (negate_flag) {
        result = -result;
    }
    return result;
}

//
// Convert integer value between native and X1 formats.
//
Word integer_to_x1(int input)
{
    const bool negate_flag = input < 0;
    if (negate_flag) {
        input = -input;
    }

    if (input > (int)BITS(26)) {
        throw std::runtime_error("Overflow in integer arithmetic");
    }

    Word result = input;
    if (negate_flag) {
        result ^= BITS(27);
    }
    return result;
}

int x1_to_integer(Word input)
{
    if (input & ONEBIT(26)) {
        // Negative.
        return - (input ^ BITS(27));
    } else {
        // Positive.
        return input & BITS(27);
    }
}

//
// Print CPU instruction with mnemonics.
//
void x1_print_instruction(std::ostream &out, unsigned cmd)
{
    unsigned opcode = (cmd >> 15) & BITS(12);
    unsigned addr   = cmd & BITS(15);

    switch (opcode) {
    case 002'20:
        out << "A := " << addr;
        break;
    case 012'20:
        out << "S := " << addr;
        break;
    case 042'20:
        out << "B := " << addr;
        break;
    case 052'20:
        out << "T := " << addr;
        break;
    case 052'23:
        out << "if !C then T := " << addr;
        break;
    //TODO: process other instructions
    case 0:
        switch (addr) {
        // clang-format off
        case OPC_ETMR: out << "ETMR extransmark result"; break;
        case OPC_ETMP: out << "ETMP extransmark procedure"; break;
        case OPC_FTMR: out << "FTMR formtransmark result"; break;
        case OPC_FTMP: out << "FTMP formtransmark procedure"; break;
        case OPC_RET:  out << "RET return"; break;
        case OPC_EIS:  out << "EIS end of implicit subroutine"; break;
        case OPC_TRAD: out << "TRAD take real address dynamic"; break;
        case OPC_TRAS: out << "TRAS take real address static"; break;
        case OPC_TIAD: out << "TIAD take integer address dynamic"; break;
        case OPC_TIAS: out << "TIAS take integer address static"; break;
        case OPC_TFA:  out << "TFA take formal address"; break;
        case OPC_FOR0: out << "FOR0"; break;
        case OPC_FOR1: out << "FOR1"; break;
        case OPC_FOR2: out << "FOR2"; break;
        case OPC_FOR3: out << "FOR3"; break;
        case OPC_FOR4: out << "FOR4"; break;
        case OPC_FOR5: out << "FOR5"; break;
        case OPC_FOR6: out << "FOR6"; break;
        case OPC_FOR7: out << "FOR7"; break;
        case OPC_FOR8: out << "FOR8"; break;
        case OPC_GTA:  out << "GTA goto adjustment"; break;
        case OPC_SSI:  out << "SSI store switch index"; break;
        case OPC_CAC:  out << "CAC copy boolean acc. into condition"; break;
        case OPC_TRRD: out << "TRRD take real result dynamic"; break;
        case OPC_TRRS: out << "TRRS take real result static"; break;
        case OPC_TIRD: out << "TIRD take integer result dynamic"; break;
        case OPC_TIRS: out << "TIRS take integer result static"; break;
        case OPC_TFR:  out << "TFR take formal result"; break;
        case OPC_ADRD: out << "ADRD add real dynamic"; break;
        case OPC_ADRS: out << "ADRS add real static"; break;
        case OPC_ADID: out << "ADID add integer dynamic"; break;
        case OPC_ADIS: out << "ADIS add integer static"; break;
        case OPC_ADF:  out << "ADF add formal"; break;
        case OPC_SURD: out << "SURD subtract real dynamic"; break;
        case OPC_SURS: out << "SURS subtract real static"; break;
        case OPC_SUID: out << "SUID subtract integer dynamic"; break;
        case OPC_SUIS: out << "SUIS subtract integer static"; break;
        case OPC_SUF:  out << "SUF subtract formal"; break;
        case OPC_MURD: out << "MURD multiply real dynamic"; break;
        case OPC_MURS: out << "MURS multiply real static"; break;
        case OPC_MUID: out << "MUID multiply integer dynamic"; break;
        case OPC_MUIS: out << "MUIS multiply integer"; break;
        case OPC_MUF:  out << "MUF static multiply formal"; break;
        case OPC_DIRD: out << "DIRD divide real dynamic"; break;
        case OPC_DIRS: out << "DIRS divide real static"; break;
        case OPC_DIID: out << "DIID divide integer dynamic"; break;
        case OPC_DIIS: out << "DIIS divide integer static"; break;
        case OPC_DIF:  out << "DIF divide formal"; break;
        case OPC_IND:  out << "IND indexer"; break;
        case OPC_NEG:  out << "NEG Invert sign accumulator"; break;
        case OPC_TAR:  out << "TAR take result"; break;
        case OPC_ADD:  out << "ADD add"; break;
        case OPC_SUB:  out << "SUB subtract"; break;
        case OPC_MUL:  out << "MUL multiply"; break;
        case OPC_DIV:  out << "DIV divide"; break;
        case OPC_IDI:  out << "IDI integer division"; break;
        case OPC_TTP:  out << "TTP to the power"; break;
        case OPC_MOR:  out << "MOR more >"; break;
        case OPC_LST:  out << "LST at least ≥"; break;
        case OPC_EQU:  out << "EQU equal ="; break;
        case OPC_MST:  out << "MST at most ≤"; break;
        case OPC_LES:  out << "LES less <"; break;
        case OPC_UQU:  out << "UQU unequal ≠"; break;
        case OPC_NON:  out << "NON non ¬"; break;
        case OPC_AND:  out << "AND and ∧"; break;
        case OPC_OR:   out << "OR or ∨"; break;
        case OPC_IMP:  out << "IMP implies ⊃"; break;
        case OPC_QVL:  out << "QVL equivalent ≡"; break;
        case OPC_abs:  out << "abs"; break;
        case OPC_sign: out << "sign"; break;
        case OPC_sqrt: out << "sqrt"; break;
        case OPC_sin:  out << "sin"; break;
        case OPC_cos:  out << "cos"; break;
        case OPC_ln:   out << "ln"; break;
        case OPC_exp:  out << "exp"; break;
        case OPC_entier: out << "entier"; break;
        case OPC_ST:   out << "ST  store"; break;
        case OPC_STA:  out << "STA store also"; break;
        case OPC_STP:  out << "STP store procedure value"; break;
        case OPC_STAP: out << "STAP store also procedure value"; break;
        case OPC_SCC:  out << "SCC short circuit"; break;
        case OPC_RSF:  out << "RSF real arrays storage function frame"; break;
        case OPC_ISF:  out << "ISF integer arrays storage function frame"; break;
        case OPC_RVA:  out << "RVA real value array storage function frame"; break;
        case OPC_IVA:  out << "IVA integer value array storage function frame"; break;
        case OPC_LAP:  out << "LAP local array positioning"; break;
        case OPC_VAP:  out << "VAP value array positioning"; break;
        case OPC_START: out << "START"; break;
        case OPC_STOP: out << "STOP"; break;
        case OPC_TFP:  out << "TFP take formal parameter"; break;
        case OPC_TAS:  out << "TAS type algol symbol"; break;
        case OPC_OBC6: out << "OBC6 output buffer class 6"; break;
        case OPC_FLOATER: out << "FLOATER"; break;
        case OPC_read: out << "read"; break;
        case OPC_print: out << "print"; break;
        case OPC_TAB:  out << "TAB"; break;
        case OPC_NLCR: out << "NLCR"; break;
        case OPC_XEEN: out << "XEEN"; break;
        case OPC_SPACE: out << "SPACE"; break;
        case OPC_stop: out << "stop"; break;
        case OPC_P21:  out << "P21"; break;
        default: out << "OPC #" << addr; break;
        // clang-format on
        }
        break;
    default:
        out << "???";
        break;
    }
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
    auto native = x1_to_ieee(value);
    auto fmt = "%.13Lg";
    int nbytes = 1 + std::snprintf(nullptr, 0, fmt, native);
    std::vector<char> buf(nbytes);
    std::snprintf(buf.data(), nbytes, fmt, native);
    out << buf.data();
}

//
// Absolute value of integer number.
//
Word x1_abs_int(Word value)
{
    if (value & ONEBIT(26)) {
        value ^= BITS(27);
    }
    return value & BITS(27);
}

//
// Absolute value of real number.
//
Real x1_abs_real(Real value)
{
    if (value & ONEBIT(53)) {
        value ^= BITS(54);
    }
    return value & BITS(54);
}
