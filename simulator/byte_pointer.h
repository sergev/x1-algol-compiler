//
// Byte pointer.
//
#include "machine.h"

class BytePointer {
private:
    std::array<Word, MEMORY_NWORDS> &memory;

public:
    unsigned word_addr;
    unsigned byte_index;

    BytePointer(std::array<Word, MEMORY_NWORDS> &m, unsigned wa, unsigned bi = 0)
        : memory(m), word_addr(wa), byte_index(bi)
    {
    }

    // Fetch byte at the pointer. No increment.
    uint8_t peek_byte()
    {
        const Word &w = memory[word_addr];
        return w >> (byte_index * 8);
    }

    // Get byte at the pointer, and increment.
    uint8_t get_byte()
    {
        auto ch = peek_byte();
        increment();
        return ch;
    }

    // Store byte at the pointer, and increment.
    void put_byte(uint8_t ch)
    {
        Word &w              = memory[word_addr];
        const unsigned shift = byte_index * 8;
        w                    = (w & ~(0xff << shift)) | (ch << shift);
        increment();
    }

    // Store byte at the pointer, and increment.
    void increment()
    {
        byte_index++;
        if (byte_index == 3) {
            byte_index = 0;
            word_addr++;
        }
    }
};
