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
// Convert float value between IEEE and X1 formats.
//
Word ieee_to_x1(double d);
double x1_to_ieee(Word word);

//
// Print X1 word.
//
void x1_print_word_octal(std::ostream &out, Word value);
void x1_print_word_bytes(std::ostream &out, Word value);

//
// Print X1 instruction.
//
void x1_print_instruction_octal(std::ostream &out, unsigned cmd);
void x1_print_instruction_mnemonics(std::ostream &out, unsigned cmd);

//
// Convert numbers to strings.
//
std::string to_octal(unsigned val);

//
// Bits of memory word, from right to left, starting from 1.
//
#define ONEBIT(n)      (1U << (n))                   // one bit set, from 0 to 31
#define BITS(n)        ((uint32_t)~0ULL >> (32 - n)) // bitmask of bits from 0 to n
#define ADDR(x)        ((x) & BITS(15))              // address of word

#endif // X1_ARCH_H
