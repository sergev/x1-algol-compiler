#include <cstdio>
#include <cstdlib>

int read_byte() {
    int byte;
    if (0 >= scanf("%d", &byte) || byte < 0 || byte > 31) {
        printf("Short or bad tape\n");
        exit(EXIT_FAILURE);
    }
    return byte;
}

int parity(unsigned long a) {
    for (int i = 0; i < 35; ++i)
        a = (a & 1) ^ (a >> 1);
    return a;
}

bool read_header(int& mcp, int& len) {
    int byte;
    bool p;
    if (0 >= scanf("%d", &byte) || byte != 31) {
        printf("Header expected\n");
        exit(EXIT_FAILURE);
    }
    len = read_byte();
    len = (len << 5) | read_byte();
    len = (len << 5) | read_byte();
    p = parity(len);
    len &= 037777;
    mcp = read_byte();
    mcp = (mcp << 5) | read_byte();
    p |= parity(mcp);
    mcp &= 0777;
    return p;
}

int main() {
    unsigned word;
    int mcp, len;
    if (read_header(mcp, len)) {
        printf("Parity error in header\n");
        exit(EXIT_FAILURE);
    }
    printf("Converting MCP %d, length %d\n", mcp, len);
    for (int i = 0; i < len; ++i) {
        word = read_byte();
        if (word & 8) {
            word = (word << 5) | read_byte();
            printf("%d %d%s\n", i, word & 127, parity(word) ? " !" : "");
        } else if ((word & 0xC) == 4) {
            unsigned prefix = word;
            word = read_byte();
            word = (word << 5) | read_byte();
            word = (word << 5) | read_byte();
            word = (word << 5) | read_byte();
            word = (word << 5) | read_byte();
            word = (word << 5) | read_byte();
            printf("%d %d %010o%s\n", i, (word >> 27) & 3,
                   word & 0777777777, parity(word^prefix) ? " !" : "");
        } else {
            unsigned p = 0777777777;
            word = (word << 5) | read_byte();
            word = (word << 5) | read_byte();
            word = (word << 5) | read_byte();
            word = (word << 5) | read_byte();
            switch ((word >> 17) & 31) {
            case 1: p = 0000000000; break;
            case 2: p = 0022000000; break;
            case 3: p = 0120000000; break;
            case 4: p = 0122000000; break;
            case 5: p = 0420000000; break;
            case 6: p = 0422000000; break;
            case 7: p = 0520000000; break;
            case 8: p = 0522000000; break;
            case 9: p = 0520300000; break;
            case 10: p = 0040000000; break;
            default: printf("Unknown short code %d\n", (word >> 17) & 31);
                exit(EXIT_FAILURE);
            }
            printf("%d %d %010o%s\n", i, (word >> 15) & 3,
                   p | (word & 077777), parity(word) ? " !" : "" );
        }
    }
}
