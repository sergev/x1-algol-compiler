#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <string>
#include <vector>

const int d26 = 1 << 26;
const int d5 = 1 << 5;
const int d10 = 1 << 10;
const int d15 = 1 << 15;
const int d12 = 1 << 12;

typedef void Void;
int bitcount, bitstock;

bool word_store = false;
std::vector<int> store;

int odd_parity(int a) {
    int i;
    for (i = 0; i < 27; ++i)
        a = (a & 1) ^ (a >> 1);
    return !a;
}

Void bit_string_maker(int w)
{
    int head = 0;
    int tail, i, FORLIM;
    tail   = w & (d10 - 1);
    FORLIM = bitcount;
    /*shift (head,tail) bitcount places to the left:*/
    for (i = 1; i <= FORLIM; i++) {
        head = head * 2 + tail / d26;
        tail = (tail & (d26 - 1)) * 2;
    } /*shift*/
    bitstock += tail;
    bitcount += w / d10;
    if (bitcount <= 27)
        return;
    bitcount -= 27;
    if (word_store) {
        store.push_back(bitstock);
    } else {
        store.push_back((bitstock>>0) & 127);
        store.push_back((bitstock>>7) & 127);
        store.push_back((bitstock>>14) & 127);
        store.push_back((bitstock>>21) << 1 | odd_parity(bitstock));
    }
    bitstock    = head;
} /*bit_string_maker*/

Void address_coder(int a)
{
    /*LS*/
    int w;

    w = a & (d5 - 1);
    if (w == 1)
        w = 2048; /*2*1024 +  0*/
    else {
        if (w == 2)
            w = 3074; /*3*1024 +  2*/
        else {
            if (w == 3)
                w = 3075; /*3*1024 +  3*/
            else
                w += 6176; /*6*1024 + 32*/
        }
    }
    bit_string_maker(w);
    w = (a / d5) & (d5 - 1);
    if (w == 0)
        w = 2048; /*2*1024 +  0*/
    else {
        if (w == 1)
            w = 4100; /*4*1024 +  4*/
        else {
            if (w == 2)
                w = 4101; /*4*1024 +  5*/
            else {
                if (w == 4)
                    w = 4102; /*4*1024 +  6*/
                else {
                    if (w == 5)
                        w = 4103; /*4*1024 +  7*/
                    else
                        w += 6176; /*6*1024 + 32*/
                }
            }
        }
    }
    bit_string_maker(w);
    w = (a / d10) & (d5 - 1);
    if (w == 0)
        w = 1024; /*1*1024 + 0*/
    else
        w += 6176; /*6*1024 + 32*/
    bit_string_maker(w);
} /*address_coder*/

Void fill_result_list(int opc, int w)
{
    /*ZF*/
    char j;

    if (opc < 8) {
        address_coder(w);
        w = w / d15 * d15 + opc;
        if (w == 21495808L) /*  2S   0 A  */
            w = 3076;       /*3*1024 +   4*/
        else if (w == 71827459L) /*  2B   3 A  */
            w = 3077;       /*3*1024 +   5*/
        else if (w == 88080386L) /*  2T 2X0    */
            w = 4108;       /*4*1024 +  12*/
        else if (w == 71827456L) /*  2B   0 A  */
            w = 4109;       /*4*1024 +  13*/
        else if (w == 4718592L) /*  2A   0 A  */
            w = 7280;       /*7*1024 + 112*/
        else if (w == 71303170L) /*  2B 2X0    */
            w = 7281;       /*7*1024 + 113*/
        else if (w == 88604673L) /*  2T   1 A  */
            w = 7282;       /*7*1024 + 114*/
        else if (w == 0)    /*  0A 0X0    */
            w = 7283;       /*7*1024 + 115*/
        else if (w == 524291L)  /*  0A   3 A  */
            w = 7284;       /*7*1024 + 116*/
        else if (w == 88178690L) /*N 2T 2X0    */
            w = 7285;       /*7*1024 + 117*/
        else if (w == 71827457L) /*  2B   1 A  */
            w = 7286;       /*7*1024 + 118*/
        else if (w == 1048577L) /*  0A 1X0 B  */
            w = 7287;       /*7*1024 + 119*/
        else if (w == 20971522L) /*  2S 2X0    */
            w = 7288;       /*7*1024 + 120*/
        else if (w == 4784128L) /*Y 2A   0 A  */
            w = 7289;       /*7*1024 + 121*/
        else if (w == 8388608L) /*  4A 0X0    */
            w = 7290;       /*7*1024 + 122*/
        else if (w == 4390912L) /*Y 2A 0X0   P*/
            w = 7291;       /*7*1024 + 123*/
        else if (w == 13172736L) /*Y 6A   0 A */
            w = 7292;       /*7*1024 + 124*/
        else if (w == 1572865L) /*  0A 1X0 C  */
            w = 7293;       /*7*1024 + 125*/
        else if (w == 524288L)  /*  0A     0 A */
            w = 7294;       /*7*1024 + 126*/
        else  {             /*7*1024 + 127*/
            address_coder(w / d15 + opc * d12);
            w = 7295;
        }
    } /*opc < 8*/
    else if (opc <= 61) {
        j = opc;
        switch (j) {
        case 8:
            w = 10624; /*10*1024+384*/
            break;

        case 9:
            w = 6160; /* 6*1024+ 16*/
            break;

        case 10:
            w = 10625; /*10*1024+385*/
            break;

        case 11:
            w = 10626; /*10*1024+386*/
            break;

        case 12:
            w = 10627; /*10*1024+387*/
            break;

        case 13:
            w = 7208; /* 7*1024+ 40*/
            break;

        case 14:
            w = 6161; /* 6*1024+ 17*/
            break;

        case 15:
            w = 10628; /*10*1024+388*/
            break;

        case 16:
            w = 5124; /* 5*1024+  4*/
            break;

        case 17:
            w = 7209; /* 7*1024+ 41*/
            break;

        case 18:
            w = 6162; /* 6*1024+ 18*/
            break;

        case 19:
            w = 7210; /* 7*1024+ 42*/
            break;

        case 20:
            w = 7211; /* 7*1024+ 43*/
            break;

        case 21:
            w = 10629; /*10*1024+389*/
            break;

        case 22:
            w = 10630; /*10*1024+390*/
            break;

        case 23:
            w = 10631; /*10*1024+391*/
            break;

        case 24:
            w = 10632; /*10*1024+392*/
            break;

        case 25:
            w = 10633; /*10*1024+393*/
            break;

        case 26:
            w = 10634; /*10*1024+394*/
            break;

        case 27:
            w = 10635; /*10*1024+395*/
            break;

        case 28:
            w = 10636; /*10*1024+396*/
            break;

        case 29:
            w = 10637; /*10*1024+397*/
            break;

        case 30:
            w = 6163; /* 6*1024+ 19*/
            break;

        case 31:
            w = 7212; /* 7*1024+ 44*/
            break;

        case 32:
            w = 10638; /*10*1024+398*/
            break;

        case 33:
            w = 4096; /* 4*1024+  0*/
            break;

        case 34:
            w = 4097; /* 4*1024+  1*/
            break;

        case 35:
            w = 7213; /* 7*1024+ 45*/
            break;

        case 36:
            w = 10639; /*10*1024+399*/
            break;

        case 37:
            w = 10640; /*10*1024+400*/
            break;

        case 38:
            w = 10641; /*10*1024+401*/
            break;

        case 39:
            w = 7214; /* 7*1024+ 46*/
            break;

        case 40:
            w = 10642; /*10*1024+402*/
            break;

        case 41:
            w = 10643; /*10*1024+403*/
            break;

        case 42:
            w = 10644; /*10*1024+404*/
            break;

        case 43:
            w = 10645; /*10*1024+405*/
            break;

        case 44:
            w = 10646; /*10*1024+406*/
            break;

        case 45:
            w = 10647; /*10*1024+407*/
            break;

        case 46:
            w = 10648; /*10*1024+408*/
            break;

        case 47:
            w = 10649; /*10*1024+409*/
            break;

        case 48:
            w = 10650; /*10*1024+410*/
            break;

        case 49:
            w = 10651; /*10*1024+411*/
            break;

        case 50:
            w = 10652; /*10*1024+412*/
            break;

        case 51:
            w = 10653; /*10*1024+413*/
            break;

        case 52:
            w = 10654; /*10*1024+414*/
            break;

        case 53:
            w = 10655; /*10*1024+415*/
            break;

        case 54:
            w = 10656; /*10*1024+416*/
            break;

        case 55:
            w = 10657; /*10*1024+417*/
            break;

        case 56:
            w = 5125; /* 5*1024+  5*/
            break;

        case 57:
            w = 10658; /*10*1024+418*/
            break;

        case 58:
            w = 5126; /* 5*1024+  6*/
            break;

        case 59:
            w = 10659; /*10*1024+419*/
            break;

        case 60:
            w = 10660; /*10*1024+420*/
            break;

        case 61:
            w = 7215; /* 7*1024+ 47*/
            break;
        }
    } /*opc <= 61*/
    else if (opc == 85) /*ST*/
        w = 5127;       /* 5*1024 +   7*/
    else
        w = opc + 10599; /*10*1024 + 359*/
    bit_string_maker(w);
} /*fill_result_list*/

void usage(const char * prog_name) {
    printf("X1 object file generator\n");
    printf("Usage:\n");
    printf("    %s [options...] MCP# source\n", prog_name);
    printf("Input files:\n");
    printf("    source                  Source text (\"offset OPC [word]\" per line)\n");
    printf("Options:\n"
           "    -h                      Display available options\n"
           "    -w                      Output in word-based format\n");
}

int main(int argc, char*argv[]) {
    int i;
    int len = 0, num;
    char * line = nullptr;
    size_t lsize = 0;
    FILE * src;
    const char * prog_name = strrchr(argv[0], '/');
    if (prog_name == NULL) {
        prog_name = argv[0];
    } else {
        prog_name++;
    }
    for (;;) {
        switch (getopt(argc, argv, "hw")) {
        case EOF:
            break;
        case 'h':
        case '?':
            usage(prog_name);
            exit(EXIT_SUCCESS);
        case 'w':
            word_store = true;
            continue;
        }
        break;
    }

    if (argc != optind + 2) {
        usage(prog_name);
        exit(EXIT_FAILURE);
    }
    num = atoi(argv[optind]);
    src = fopen(argv[optind+1], "r");
    if (!src) {
        perror("Could not open source file");
        exit(EXIT_FAILURE);
    }
    if (word_store) {
        store.push_back(0770000000);
    } else {
        store.push_back(0);
        store.push_back(0);
        store.push_back(0);
        store.push_back(127);
    }
    int lnum = 0;
    while (0 < getline(&line, &lsize, src)) {
        ++lnum;
        char * end1;
        int offset = (int)strtol(line, &end1, 10);
        if (end1 == line) {
            // No number was read; treat as a comment line
            continue;
        }
        if (offset != len) {
            printf("Line %d: word offset mismatch, seen %d, expected %d\n",
                   lnum, offset, len);
            exit(EXIT_FAILURE);
        }
        char * end2;
        int opc = (int)strtol(end1, &end2, 10); // OPC is always decimal
        if (end1 == end2) {
            printf("Line %d: missing OPC\n", lnum);
            exit(EXIT_FAILURE);
        }
        int w = 0;
        if (opc < 8) {
            char * end3;
            w = (int)strtol(end2, &end3, 0);
            if (end2 == end3) {
                printf("Line %d: OPC %d requires a word\n", lnum, opc);
                exit(EXIT_FAILURE);
            }
        }
        fill_result_list(opc, w);
        ++len;
    }
    fprintf(stderr, "Making MCP %d, length %d\n", num, len);
    for (i = 0; i < 13; ++i)
        bit_string_maker (1<<10|(num>>i & 1));
    for (i = 0; i < 13; ++i)
        bit_string_maker (1<<10|(len>>i & 1));
    bit_string_maker(1|1<<10);
    while (bitstock) bit_string_maker(1 << 10);
    if (word_store) {
        for (size_t i = 0; i < store.size(); ++i) {
            printf("store[mcpb + %zu] = %010o;\n", i+1, store[i]);
        }
        exit(EXIT_SUCCESS);
    }
    std::string s{"30"};
    while (!store.empty()) {
        if (!s.empty()) s += ' ';
        s += std::to_string(store.back());
        store.pop_back();
        if (s.length() > 72) {
            puts(s.c_str());
            s.erase();
        }
    }
    if (!s.empty())
        puts(s.c_str());
}
