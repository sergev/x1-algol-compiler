#ifndef X1_ARCH_H
#define X1_ARCH_H

#include <cstdint>
#include <ostream>
#include <vector>

//
// Memory has 32768 words.
//
static const unsigned MEMORY_NWORDS = 32 * 1024;

//
// 27-bit memory word in lower bits of unsigned value.
//
using Word = uint32_t;

//
// Real values are represented by two 27-bit words.
// We combine them into a single 54-bit value.
//
using Real = uint64_t;

//
// Array of words.
//
using Words = std::vector<Word>;

//
// Convert assembly source code into binary word.
//
Word x1_asm(const char *source);

//
// Get instruction mnemonics by opcode.
//
const char *x1_opname(unsigned opcode);

//
// Get instruction opcode by mnemonics (UTF-8).
//
bool x1_opcode(const char *opname, unsigned &opcode);

//
// Create real value from two words.
//
Real x1_words_to_real(Word hi, Word lo);

//
// Convert real value between IEEE and X1 formats.
//
Real ieee_to_x1(long double d);
long double x1_to_ieee(Real word);

//
// Print X1 word.
//
void x1_print_word_octal(std::ostream &out, Word value);
void x1_print_word_bytes(std::ostream &out, Word value);

//
// Print X1 instruction with mnemonics.
//
void x1_print_instruction(std::ostream &out, unsigned cmd);

//
// Convert numbers to strings.
//
std::string to_octal(unsigned val);

//
// Bits of memory word, from right to left, starting from 1.
//
#define ONEBIT(n) (1ULL << (n))       // one bit set, from 0 to 31
#define BITS(n)   (~0ULL >> (64 - n)) // bitmask of bits from 0 to n
#define ADDR(x)   ((x) & BITS(15))    // address of word

//
// Negate a number.
//
Word x1_negate_int(Word value);
Real x1_negate_real(Real value);

//
// Standard function: absolute value.
//
Word x1_abs_int(Word value);
Real x1_abs_real(Real value);

//
// Print a number.
//
void x1_print_integer(std::ostream &out, Word value);
void x1_print_real(std::ostream &out, Real value);

#endif // X1_ARCH_H
