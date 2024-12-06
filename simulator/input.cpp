#include <iostream>
#include <istream>
#include <streambuf>
#include <limits>

#include "machine.h"

#ifdef _WIN32
#include <io.h>
bool Machine::is_interactive = _isatty(0);
#else
#include <unistd.h>
bool Machine::is_interactive = isatty(0);
#endif

class ReplaceTenWithEStreamBuf : public std::streambuf {
public:
    explicit ReplaceTenWithEStreamBuf(std::istream& inp) : in_stream(inp.rdbuf()) {}

protected:
    // Override underflow to replace '⏨' with 'E'
    int underflow() override {
        int ch = in_stream->sbumpc();  // Read the next character from the input stream

        if (ch == EOF) {  // Check for end-of-file
            return EOF;
        }

        const unsigned char U_23E8[] = "⏨";
        int buf_pos = 0;
        if (ch == U_23E8[0]) {
            char_buffer[buf_pos++] = ch;
            ch = in_stream->sbumpc();
            if (ch == EOF)
                return EOF;     // in the middle of a UTF-8 symbol
            if (ch == U_23E8[1]) {
                char_buffer[buf_pos++] = ch;
                ch = in_stream->sbumpc();
                if (ch == EOF)
                    return EOF;     // in the middle of a UTF-8 symbol
                if (ch == U_23E8[2]) {
                    ch = 'E';
                    buf_pos = 0;
                }
            }
        }

        // Put the character back into the stream
        this->setg(char_buffer, char_buffer, char_buffer + buf_pos + 1);
        char_buffer[buf_pos] = ch;
        return int(char_buffer[0]) & 0xFF;
}

private:
    std::streambuf* in_stream;  // The original stream buffer
    char char_buffer[3]{};        // Buffer to store up to 3 characters
};

class ReplaceTenWithEStream : public std::istream {
public:
    explicit ReplaceTenWithEStream(std::istream& inp) : std::istream(&buf), buf(inp) {}

private:
    ReplaceTenWithEStreamBuf buf;  // Custom stream buffer
};

//
// Read real or integer number from input stream.
// Return the number.
// On end of file, throw exception.
// When input is invalid:
//  - throw exception when non-interactive
//  - retry when in interactive mode
//
long_double Machine::input_real(std::istream &orig_stream)
{
    ReplaceTenWithEStream input_stream(orig_stream);
    // Loop until user enters a valid input
    for (;;) {
        if (is_interactive) {
            std::cout << "Enter number: " << std::flush;
        }
        long_double x{};
        input_stream >> x;

        if (input_stream) {
            // Number is valid.
            return x;
        }

        // Cannot parse the input.
        if (input_stream.eof()) {
            throw std::runtime_error("No input");
        }
        if (!is_interactive) {
            throw std::runtime_error("Bad input");
        }
        std::cout << "Bad input, try again.\n";
        input_stream.clear();

        // Remove bad input and try again.
        input_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

//
// Read console switches.
// Called by XEEN procedure.
//
unsigned Machine::read_console_switches(std::istream &input_stream, unsigned bitmask)
{
    if (!switches_are_valid) {
        // Loop until user enters a valid input
        for (;;) {
            if (is_interactive) {
                std::cout << "\n";
                std::cout << "Enter console switches (up to 27 binary digits)\n";
                std::cout << "> " << std::flush;
            }
            try {
                std::string line;
                if (std::getline(input_stream, line)) {
                    console_switches = std::stoul(line, nullptr, 2);
                    switches_are_valid = true;
                    break;
                }
            } catch (...) {
                // No valid number.
            }

            // Cannot parse the input.
            if (input_stream.eof()) {
                throw std::runtime_error("No input");
            }
            if (!is_interactive) {
                throw std::runtime_error("Bad input");
            }
            std::cout << "Bad input, try again.\n";
            input_stream.clear();
        }
    }
    return console_switches & bitmask;
}

//
// Read character from input stream.
// Return integer number in Algol encoding.
// On end of file, return 0.
//
int Machine::input_char(std::istream &stream)
{
    char ch;
again:
    if (!stream.get(ch)) {
        // End of file.
eof:    return 0377;
    }
    if (ch >= '0' && ch <= '9') {
        // Digits.
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'z') {
        // Letters lowercase.
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'Z') {
        // Letters uppercase.
        return ch - 'A' + 37;
    }
    switch (ch) {
    case '+': return 64;
    case '-': return 65;
    case '*': return 66; // ×
    case '/': return 67;
    case '>': return 70;
    case '=': return 72;
    case '<': return 74;
    case ',': return 87;
    case '.': return 88;
    case '@': return 89; // ⏨
    case ':': return 90;
    case ';': return 91;
    case ' ': return 93; // space
    case '(': return 98;
    case ')': return 99;
    case '[': return 100;
    case ']': return 101;
    case '`': return 102;
    case '\'': return 103;
    case '\t': return 118; // tab
    case '\n': return 119; // newline
    case '\r': goto again; // CR - ignore
    case '"': return 121;
    case '?': return 122;
    case '|': return 162;
    case '_': return 163;
    case '\302':
        if (!stream.get(ch)) {
            goto eof;
        }
        switch (ch) {
        case '\254': return 76; // ¬
        default: goto bad;
        }
    case '\303':
        if (!stream.get(ch)) {
            goto eof;
        }
        switch (ch) {
        case '\227': return 66; // ×
        case '\267': return 68; // ÷
        default: goto bad;
        }
    case '\342':
        if (!stream.get(ch)) {
            goto eof;
        }
        switch (ch) {
        case '\206':
            if (!stream.get(ch)) {
                goto eof;
            }
            switch (ch) {
            case '\221': return 69; // ↑
            default: goto bad;
            }
        case '\210':
            if (!stream.get(ch)) {
                goto eof;
            }
            switch (ch) {
            case '\247': return 77; // ∧
            case '\250': return 78; // ∨
            default: goto bad;
            }
        case '\211':
            if (!stream.get(ch)) {
                goto eof;
            }
            switch (ch) {
            case '\240': return 75; // ≠
            case '\241': return 80; // ≡
            case '\244': return 73; // ≤
            case '\245': return 71; // ≥
            default: goto bad;
            }
        case '\212':
            if (!stream.get(ch)) {
                goto eof;
            }
            switch (ch) {
            case '\203': return 79; // ⊃
            default: goto bad;
            }
        case '\217':
            if (!stream.get(ch)) {
                goto eof;
            }
            switch (ch) {
            case '\250': return 89; // ⏨
            default: goto bad;
            }
        default: goto bad;
        }
    default:
bad:    throw std::runtime_error("Bad input character 0" + to_octal(ch & 0377));
    }
}
