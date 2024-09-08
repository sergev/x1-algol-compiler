/* Output from p2c 2.00.Oct.15, the Pascal-to-C translator */
/* From input file "x1algol.pas" */

#include <getopt.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* Pertinent pieces from p2c/p2c.h and p2clib.c */
#define Local  static
#define Static static
typedef void Void;
typedef unsigned char Char;
typedef unsigned char boolean;

int P_argc;
char **P_argv;

void PASCAL_MAIN(int argc, char **argv)
{
    P_argc   = argc;
    P_argv   = argv;
}

int P_eof(FILE *f)
{
    int ch;

    if (feof(f))
        return 1;
    if (isatty(fileno(f)))
        return 0; /* not safe to look-ahead on the keyboard! */
    ch = getc(f);
    if (ch == EOF)
        return 1;
    ungetc(ch, f);
    return 0;
}

#define d2  4
#define d3  8
#define d4  16
#define d5  32
#define d6  64
#define d7  128
#define d8  256
#define d10 1024
#define d12 4096
#define d13 8192

#define d15 32768
#define d16 65536
#define d17 131072
#define d18 262144
#define d19 524288
#define d20 1048576
#define d21 2097152
#define d22 4194304
#define d23 8388608
#define d24 16777216
#define d25 33554432
#define d26 67108864
#define mz  134217727

// For convenience, both PRINTTEXT and EVEN are in store by default.

// MCP_xxx_LEN is the object bitstream length,
// not the final procedure length in  memory.
#define MCP_PRINTTEXT_LEN 22
#define MCP_EVEN_LEN 14
#define MCP_TIMEOFDAY_LEN 6

#define MCP_EVEN_BASE MCP_PRINTTEXT_LEN
#define MCP_TIMEOFDAY_BASE (MCP_EVEN_BASE+MCP_EVEN_LEN)

#define MCPLEN (MCP_PRINTTEXT_LEN+MCP_EVEN_LEN+MCP_TIMEOFDAY_LEN)

#define gvc0   138  /*0-04-10*/
#define tlib   800  /*0-25-00*/
#define plie   6783 /*6-19-31*/
#define bim    (mcpb+MCPLEN+2)  /*0-29-nn */
#define nlscop 43
#define nlsc0  62
#define mlib   800   /*0-25-00*/
#define klie   10165 /*9-29-21*/
#define crfb   623   /*0-19-15*/
#define mcpb   928   /*0-29-00*/

Static int tlsc, plib, flib, klib, nlib, rht, vht, qc, scan, rnsa, rnsb, rnsc, rnsd, dl, inw, fnw,
    dflag, bflag, oflag, nflag, kflag, iflag, mflag, vflag, aflag, sflag, eflag, jflag, pflag,
    fflag, bn, vlam, pnlv, gvc, lvc, oh, id, nid, ibd, inba, fora, forc, psta, pstb, spe, arra,
    arrb, arrc, arrd, ic, aic, rlaa, rlab, qa, qb, rlsc, flsc, klsc, nlsc, bitcount, bitstock;

/* Added to fix a bug in passing standard functions as parameters  */
Static int tfpflag;

Static int store[12288];
static inline int _Rstore(int idx, int line, const char * expr) {
    if (0 <= idx && idx < 12288)
        return store[idx];
    else {
        printf("Runaway read @%d, %s: %d\n", line, expr, idx);
        exit(1);
    }
    return -1;
}
#define Rstore(expr) _Rstore(expr, __LINE__, #expr)

static inline void _Wstore(int idx, int val, int line, const char * expr) {
    if (0 <= idx && idx < 12288)
        store[idx] = val;
    else {
        printf("Runaway write @%d, %s: %d\n", line, expr, idx);
        exit(1);
    }
}
#define Wstore(expr, val) _Wstore(expr, val, __LINE__, #expr)

Static enum { ps, ms, virginal } rns_state;

Static int nas_stock, pos_;
Static int word_del_table[29];
Static int ascii_table[128];
Static int opc_table[113];
Static int alnum_table[64];

Static int rlib, mcpe;

Static FILE *source_tape;
Static FILE *lib_tape;

Static int ii;

Static char *input_line;
size_t input_line_len;
Static int input_pos;
Static boolean input_eof_seen;
Static boolean binary_lib_tape, verbose_tape, pass_formals;

const char * mcp_names[128];

Static Void stop(int n, const char * txt)
{
    /*emulation of a machine instruction*/
    printf("\n*** stop %d-%2d *** (%s)\n",
           n / d5, n & (d5 - 1), txt ? txt : "yet unknown" );
    if (!P_eof(source_tape)) {
        printf("Line: %s\n", input_line);
        printf("      %*c\n", (int)input_pos, '^');
    }
    exit(EXIT_FAILURE);
} /*stop*/

Static int read_next_byte()
{
    int i;
    Char ch;

    if (input_pos >= strlen(input_line)) {
        printf("Bad input: %s\n", input_line);
        exit(EXIT_FAILURE);
    }
    input_pos++;
    ch = input_line[input_pos - 1];
    i  = ch;
    /*writeln(ch, ' ', i);*/
    /*for debug*/
    return i & 0xFF;
}

Static int peek_next_byte(int k)
{
    int i;
    Char ch;

    if (input_pos + k >= strlen(input_line)) {
        return -1;
    }
    ch = input_line[input_pos + k];
    i  = ch;
    return i & 0xFF;
}

Static int read_utf8_symbol()
{
    int i, a;

    if (input_eof_seen) {
        printf("End of input\n");
        exit(EXIT_FAILURE);
    }
_L1:
    if (input_pos >= strlen(input_line)) {
        if (P_eof(source_tape)) {
            /*writeln('End of input');*/
            /*for debug*/
            input_eof_seen = true;
            return 123; /*space*/
        }
        if (getline(&input_line, &input_line_len, source_tape))
            ;
        input_pos = 0;
        /*writeln('---');*/
        /*for debug*/
        return 119; /*newline*/
    }
    i = read_next_byte();
    if (i < 128) {
        a = ascii_table[i];
        if (a < 0) {
            /*writeln('--- Bad symbol!'); halt;*/
            /*for debug*/
            goto _L1;
        }
        // Check for Unicode underlining: U+0332 is 0xCC 0xB2
        if (peek_next_byte(0) == 0xCC && peek_next_byte(1) == 0xB2) {
            // Faking "_sym", leaving a non-combining representation in the string
            // for the sake of correct position of the pointer in error messages.
            //printf("Pos %d: underlined %c\n", input_pos, i);
            input_line[input_pos] = input_line[input_pos-1] = '_';
            input_line[++input_pos] = i;
            return ascii_table['_'];
        }
        return a;
    }
    /*decode utf-8*/
    if (i == 194) {
        i = read_next_byte();
        if (i == 172) {
            return 76; /* ¬ */
        }
    } else if (i == 195) {
        i = read_next_byte();
        if (i == 151) {
            return 66; /* × */
        }
        if (i == 183) {
            return 68; /* ÷ */
        }
    } else if (i == 226) {
        i = read_next_byte();
        if (i == 134) {
            i = read_next_byte();
            if (i == 145) {
                return 69; /* ↑ */
            }
        } else if (i == 136) {
            i = read_next_byte();
            if (i == 167) {
                return 77; /* ∧ */
            }
            if (i == 168) {
                return 78; /* ∨ */
            }
        } else if (i == 137) {
            i = read_next_byte();
            if (i == 160) {
                return 75; /* ≠ */
            }
            if (i == 161) {
                return 80; /* ≡ */
            }
            if (i == 164) {
                return 73; /* ≤ */
            }
            if (i == 165) {
                return 71; /* ≥ */
            }
        } else if (i == 138) {
            i = read_next_byte();
            if (i == 131) {
                return 79; /* ⊃ */
            }
        } else if (i == 143) {
            i = read_next_byte();
            if (i == 168) {
                return 89; /* ⏨ */
            }
        }
    }
    printf("Bad input: %s\n", input_line);
    exit(EXIT_FAILURE);
    return -1;
}

Static int next_ALGOL_symbol()
{
    /*HT*/
    int sym, wdt1, wdt2;

    sym = -nas_stock;
    if (sym >= 0)            /*symbol in stock*/
        nas_stock = sym + 1; /*stock empty now*/
    else
        sym = read_utf8_symbol();
_L1:
    if (sym <= 103) /*analysis required*/
        return sym;
    if (sym == 123) /*space symbol*/
        sym = 93;
    if (sym <= 119) { /*space symbol, tab, or nlcr*/
        if (qc == 0) {
            sym = read_utf8_symbol();
            goto _L1;
        }
        return sym;
    }
    if (sym == 121) { /*"*/
        return sym;
    }
    if (sym == 122) { /*?*/
        return sym;
    }
    if (sym == 124) { /*:*/
        sym = read_utf8_symbol();
        if (sym == 72)
            sym = 92; /*:=*/
        else {        /*:*/
            nas_stock = -sym;
            sym       = 90;
        }
        return sym;
    }
    if (sym == 162) { /*|*/
        do {
            sym = read_utf8_symbol();
        } while (sym == 162);
        if (sym == 77) { /*^*/
            sym = 69;    /*|^*/
            return sym;
        }
        if (sym == 72) { /*=*/
            sym = 75;    /*|=*/
            return sym;
        }
        if (sym == 74) { /*<*/
            sym = 102;   /*|<*/
            return sym;
        }
        if (sym == 70) /*>*/
            sym = 103; /*|>*/
        else
            stop(11, "Bad char after |");
        return sym;
    }
    if (sym != 163) { /*_*/
        stop(14, "Unpermitted char");
        /*? or "*/
        return sym;
    }
    do {
        sym = read_utf8_symbol();
    } while (sym == 163);
    if (sym > 9 && sym <= 38) /*a..B*/
    {                         /*word delimiter*/
        wdt1 = word_del_table[sym - 10] & 127;
        if (wdt1 >= 63)
            sym = wdt1;
        else if (wdt1 == 0)
            stop(13, "Unrecognized underlined word");
        else if (wdt1 == 1) { /*sym = c*/
            if (qc == 0)      /*outside string*/
            {                 /*skip comment*/
                do {
                    sym = read_utf8_symbol(); /*;*/
                } while (sym != 91);
                sym = read_utf8_symbol();
                goto _L1;
            }
            sym = 97; /*comment*/
        } else {      /*_*/
            sym = read_utf8_symbol();
            if (sym == 163) {
                do {
                    sym = read_utf8_symbol();
                } while (sym == 163);
                if (sym > 9 && sym <= 32) {
                    if (sym == 29) { /*t*/
                        sym = read_utf8_symbol();
                        if (sym == 163) { /*_*/
                            do {
                                sym = read_utf8_symbol();
                            } while (sym == 163);
                            if (sym == 14) /*e*/
                                sym = 94;  /*step*/
                            else
                                sym = 113; /*string*/
                        } else
                            stop(12, "Partially underlined word");
                    } else {
                        wdt2 = word_del_table[sym - 10] / 128;
                        if (wdt2 == 0)
                            sym = wdt1 + 64;
                        else
                            sym = wdt2;
                    }
                } else
                    stop(13, "Unrecognized underlined word");
            } else
                stop(12, "Partially underlined word");
        }
        do {
            nas_stock = -read_utf8_symbol();
            if (nas_stock == -163) { /*_*/
                do {
                    nas_stock = read_utf8_symbol();
                } while (nas_stock == 163);
            }
        } while (nas_stock > 0); /*word delimiter*/
        return sym;
    }
    if (sym == 70) { /*>*/
        sym = 71;    /*>=*/
        return sym;
    }
    if (sym == 72) { /*=*/
        sym = 80;    /*eqv*/
        return sym;
    }
    if (sym == 74) { /*<*/
        sym = 73;    /*<=*/
        return sym;
    }
    if (sym == 76) { /*~*/
        sym = 79;    /*imp*/
        return sym;
    }
    if (sym == 124) /*:*/
        sym = 68;   /*div*/
    else
        stop(13, "Unrecognized underlined char");
    return sym;
} /*next_ALGOL_symbol*/

Static Void read_next_symbol()
{
    /*ZY*/
_L1:
    switch (rns_state) {
    case ps:
        dl = next_ALGOL_symbol();
        /*store symbol in symbol store:*/
        if (rnsa > d7) {
            rnsa /= d7;
            store[rnsb] += dl * rnsa;
        } else {
            rnsa = d15;
            rnsb++;
            store[rnsb] = dl * rnsa;
            if (rnsb + 8 > plib)
                stop(25, "Exhausted source store");
        }
        break;

    case ms: /*take symbol from symbol store:*/
        dl = (Rstore(rnsd) / rnsc) & (d7 - 1);
        if (rnsc > d7)
            rnsc /= d7;
        else {
            rnsc = d15;
            rnsd++;
        }
        break;

    case virginal:
        qc        = 0;
        nas_stock = 1;
        if (scan > 0) { /*prescan*/
            rns_state = ps;
            /*initialize symbol store:*/
            rnsb        = bim + 8;
            rnsd        = bim + 8;
            rnsa        = d22;
            rnsc        = d15;
            store[rnsb] = 0;
        } else
            rns_state = ms;
        goto _L1;
        break;
    }
} /*read_next_symbol*/

typedef enum { retFALSE, retTRUE, retGOTO } retType;

Local retType test1()
{
    if (dl == 88) { /*.*/
        dflag = 1;
        read_next_symbol();
        return test1();
    }
    if (dl != 89) /*ten*/
        return (dl > 9) ? retTRUE : retFALSE;
    return retGOTO;
} /*test1*/

Local retType test2()
{
    if (dl == 89) /*ten*/
        inw = 1;
    return test1();
} /*test2*/

Local retType test3()
{
    read_next_symbol();
    return test1();
} /*test3*/

Static Void read_until_next_delimiter()
{ /*body of read_until_next_delimiter*/
    /*FT*/
    retType rt;
    int marker;
    int elsc = 0, bexp = 2100;

    read_next_symbol();
    nflag = 1;
    if (dl > 9 && dl < 63) { /*letter*/
        dflag = 0;
        kflag = 0;
        inw   = 0;
        do {
            fnw = (inw & (d6 - 1)) * d21;
            inw = inw / d6 + dl * d21;
            read_next_symbol();
        } while ((inw & (d3 - 1)) <= 0 && dl <= 62);
        if ((inw & (d3 - 1)) > 0) {
            dflag = 1;
            fnw += d23;
            marker = 0;
            while (marker == 0 && dl < 63) {
                marker = (fnw & (d6 - 1)) * d21;
                fnw    = fnw / 64 + dl * d21;
                read_next_symbol();
            }
            while (marker == 0) {
                marker = (fnw & (d6 - 1)) * d21;
                fnw    = fnw / d6 + d21 * 63;
            }
            while (dl < 62)
                read_next_symbol();
        }
        goto _L4;
    }
    kflag = 1;
    fnw   = 0;
    inw   = 0;
    dflag = 0;
    if ((rt = test2()) == retTRUE) { /*not (dl in [0..9,88,89])*/
        nflag = 0;
        if (dl == 116 || dl == 117) { /*true*/
            inw   = dl - 116;
            dflag = 0;
            kflag = 1;
            nflag = 1;
            read_next_symbol();
            goto _L4;
        }
        /*false*/
        goto _L5;
    } else if (rt == retGOTO) {
        goto _L1;
    }
    do {
        if (fnw < d22) {
            inw = inw * 10 + dl;
            fnw = fnw * 10 + inw / d26;
            inw &= d26 - 1;
            elsc -= dflag;
        } else
            elsc += 1 - dflag;
    } while ((rt = test3()) == retFALSE);
    if (rt == retGOTO)
        goto _L1;
    if (dflag == 0 && fnw == 0)
        goto _L4;
    goto _L3;
_L1:
    switch (test3()) {    /*not (dl in [0..9,88,89]*/
    case retTRUE:
        if (dl == 64) { /*plus*/
            read_next_symbol();
            dflag = dl;
        } else {
            read_next_symbol();
            dflag = -dl - 1;
        }
        break;
    case retFALSE:
        dflag = dl;
        break;
    case retGOTO:
        goto _L1;
    }
    while ((rt = test3()) == retFALSE) { /*dl in [0..9,88,89]*/
        if (dflag >= 0)
            dflag = dflag * 10 + dl;
        else
            dflag = dflag * 10 - dl + 9;
        if (labs(dflag) >= d26)
            stop(3, "Integer too large");
    }
    if (rt == retGOTO)
        goto _L1;
    if (dflag < 0)
        dflag++;
    elsc += dflag;
_L3: /*float*/
    if (inw == 0 && fnw == 0) {
        dflag = 0;
        goto _L4;
    }
    /*2**11 + 52; P9-characteristic*/
    while (fnw < d25) {
        inw *= 2;
        fnw = fnw * 2 + inw / d26;
        inw &= d26 - 1;
        bexp--;
    }
    if (elsc > 0) {
        do {
            fnw *= 5;
            inw = (fnw & 7) * d23 + inw * 5 / 8;
            fnw /= 8;
            if (fnw < d25) {
                inw *= 2;
                fnw = fnw * 2 + inw / d26;
                inw &= d26 - 1;
                bexp--;
            }
            bexp += 4;
            elsc--;
        } while (elsc != 0);
    } else if (elsc < 0) {
        do {
            if (fnw >= d23 * 5) {
                inw = inw / 2 + (fnw & 1) * d25;
                fnw /= 2;
                bexp++;
            }
            inw *= 8;
            fnw = fnw * 8 + inw / d26;
            inw = (inw & (d26 - 1)) + fnw % 5 * d26;
            fnw /= 5;
            inw /= 5;
            bexp -= 4;
            elsc++;
        } while (elsc != 0);
    }
    inw += 2048;
    if (inw >= d26) {
        inw = 0;
        fnw++;
        if (fnw == d26) {
            fnw = d25;
            bexp++;
        }
    }
    if (bexp > 4095)
        stop(4, "Real too large");
    inw   = inw / 4096 * 4096 + bexp;
    dflag = 1;
_L4:
    oflag = 0;
_L5:;
} /*read_until_next_delimiter*/

Static Void fill_t_list(int n)
{
    store[tlsc] = n;
    tlsc++;
} /*fill_t_list*/

/* Local variables for prescan: */
struct LOC_prescan {
    int bc, mbc;
};

Local Void fill_prescan_list(int n, struct LOC_prescan *LINK)
{ /*update plib and prescan_list chain:*/
    /*n = 0 or n = 1*/
    /*HF*/
    int i, j, k;

    k    = plib;
    plib = k - dflag - 1;
    j    = k;
    for (i = LINK->bc * 2 + n; i >= 1; i--) {
        k        = store[j];
        store[j] = k - dflag - 1;
        j        = k;
    }
    /*shift lower part of prescan_list down over dfag + 1 places:*/
    k = plib;
    if (dflag == 0) {
        for (i = j - plib; i >= 1; i--) {
            store[k] = store[k + 1];
            k++;
        }
    } else {
        for (i = j - plib - 1; i >= 1; i--) {
            store[k] = store[k + 2];
            k++;
        }
        /*enter fnw in prescan_list:*/
        store[k + 1] = fnw;
    }
    /*shift:*/
    /*enter inw in prescan_list:*/
    store[k] = inw;
} /*fill_prescan_list*/

Local Void augment_prescan_list(struct LOC_prescan *LINK)
{
    /*HH*/
    dflag = 1;
    inw   = plie;
    fnw   = plie - 1;
    fill_prescan_list(0, LINK);
} /*augment_prescan_list*/

Local Void block_introduction(struct LOC_prescan *LINK)
{
    /*HK*/
    fill_t_list(LINK->bc); /*block-begin marker*/
    fill_t_list(-1);
    LINK->mbc++;
    LINK->bc = LINK->mbc;
    augment_prescan_list(LINK);
} /*block_introduction*/

Static Void prescan()
{ /*body of prescan*/
    /*HK*/
    struct LOC_prescan V;

    plib        = plie;
    store[plie] = plie - 1;
    tlsc        = tlib;
    V.bc        = 0;
    V.mbc       = 0;
    qc          = 0;
    rht         = 0;
    vht         = 0;
    fill_t_list(dl); /*dl should be 'begin'*/
    augment_prescan_list(&V);
_L1:
    bflag = 0;
_L2:
    read_until_next_delimiter();
_L3:
    if (dl <= 84)
        /*+,-,*,/,_:,|^,>,>=,=,<=,<,|=,~,^,`,_~,_=,goto,if,then,else*/
        goto _L1;
    /*skip:*/
    if (dl == 85) { /*for*/
        block_introduction(&V);
        goto _L1;
    }
    if (dl <= 89) /*do,comma,period,ten*/
        goto _L1;
    /*skip:*/
    if (dl == 90) { /*:*/
        fill_prescan_list(0, &V);
        goto _L2;
    }
    if (dl == 91) {                   /*;*/
        while (store[tlsc - 1] < 0) { /*block-begin marker*/
            tlsc -= 2;
            V.bc = store[tlsc];
        }
        if (rht != 0)
            stop(22, "In prescan after ;");
        if (vht != 0)
            stop(23, "In prescan after ;");
        goto _L1;
    }
    if (dl <= 97) /*:=,step,until,while,comment*/
        goto _L1;
    /*skip:*/
    if (dl <= 99) { /*(,)*/
        if (dl == 98)
            rht++;
        else
            rht--;
        goto _L1;
    }
    if (dl <= 101) { /*[,]*/
        if (dl == 100)
            vht++;
        else
            vht--;
        goto _L1;
    }
    if (dl == 102) { /*|<*/
        do {
            if (dl == 102) /*|<*/
                qc++;
            if (dl == 103) /*|>*/
                qc--;
            if (qc > 0)
                read_next_symbol();
        } while (qc != 0);
        goto _L2;
    }
    if (dl == 104) { /*begin*/
        fill_t_list(dl);
        if (bflag != 0)
            goto _L1;
        read_until_next_delimiter();
        if (dl <= 105 || dl > 112)
            goto _L3;
        tlsc--;                 /*remove begin from t_list*/
        block_introduction(&V); /*add begin to t_list again*/
        fill_t_list(104);
        goto _L3;
    }
    if (dl == 105) {                  /*end*/
        while (store[tlsc - 1] < 0) { /*block-begin marker*/
            tlsc -= 2;
            V.bc = store[tlsc];
        }
        if (rht != 0)
            stop(22, "In prescan after end");
        if (vht != 0)
            stop(23, "In prescan after end");
        tlsc--; /*remove corresponding begin from t_list*/
        if (tlsc > tlib)
            goto _L1;
        goto _L7; /*end of prescan*/
    }
    if (dl <= 105) /*dl = |>*/
        goto _L1;
    if (dl == 111) {      /*switch*/
        if (bflag == 0) { /*declarator*/
            read_until_next_delimiter();
            fill_prescan_list(0, &V);
            goto _L6;
        }
        /*for switch identifier*/
        else
            goto _L5;
    }
    /*specifier*/
_L4:
    if (dl == 112) {      /*procedure*/
        if (bflag == 0) { /*declarator*/
            bflag = 1;    /*for procedure identifier*/
            read_until_next_delimiter();
            fill_prescan_list(1, &V);
            block_introduction(&V);
            goto _L6;
        } else
            goto _L5;
    }
    /*specificier*/
    if (dl > 117) /*false*/
        stop(8, "Unknown symbol");
_L5:
    read_until_next_delimiter();
_L6:
    if (dl != 91) /*;*/
        goto _L4;
    goto _L2;
_L7:;
} /*prescan*/

Static Void intro_new_block2()
{
    /*HW*/
    int i, w;

    inba = d17 + d15;
_L1:
    i    = plib;
    plib = store[i];
    i++;
    while (i != plib) {
        w = Rstore(i);
        if ((w & 7) == 0) /*at most 4 letters/digits*/
            i++;
        else {
            store[nlib + nlsc] = store[i + 1];
            i += 2;
            nlsc++;
        }
        store[nlib + nlsc] = w;
        nlsc += 2;
        if (nlib + nlsc > i)
            stop(15, "intro_new_block2: Store capacity too small");
        store[nlib + nlsc - 1] = bn * d19 + inba;
    }
    if (inba != d18 + d15) {
        inba = d18 + d15;
        goto _L1;
    }
    lvc = 0;
} /*intro_new_block2*/

Static Void intro_new_block1()
{
    /*HW*/
    fill_t_list(nlsc);
    fill_t_list(161);
    intro_new_block2();
} /*intro_new_block1*/

Static Void intro_new_block()
{
    /*HW*/
    bn++;
    intro_new_block1();
} /*intro_new_block*/

Static Void bit_string_maker(int w)
{
    /*LL*/
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
    store[rnsb] = bitstock;
    bitstock    = head;
    rnsb++;
    if (rnsb != rnsd)
        return;
    if (nlib + nlsc + 8 >= plib) { /*shift text, fli, kli and nli*/
        stop(25, "bit_string_maker: Stream overflow");
        return;
    }
    for (i = nlib + nlsc - rnsd - 1; i >= 0; i--)
        store[rnsd + i + 8] = store[rnsd + i];
    rnsd += 8;
    flib += 8;
    klib += 8;
    nlib += 8;
} /*bit_string_maker*/

Static Void address_coder(int a)
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

Static Void fill_result_list(int opc, int w)
{
    /*ZF*/
    char j;

    rlsc++;
    if (opc < 8) {
        address_coder(w);
        w = w / d15 * d15 + opc;
        if (w == 21495808) /*  2S   0 A  */
            w = 3076;       /*3*1024 +   4*/
        else if (w == 71827459) /*  2B   3 A  */
            w = 3077;       /*3*1024 +   5*/
        else if (w == 88080386) /*  2T 2X0    */
            w = 4108;       /*4*1024 +  12*/
        else if (w == 71827456) /*  2B   0 A  */
            w = 4109;       /*4*1024 +  13*/
        else if (w == 4718592) /*  2A   0 A  */
            w = 7280;       /*7*1024 + 112*/
        else if (w == 71303170) /*  2B 2X0    */
            w = 7281;       /*7*1024 + 113*/
        else if (w == 88604673) /*  2T   1 A  */
            w = 7282;       /*7*1024 + 114*/
        else if (w == 0)    /*  0A 0X0    */
            w = 7283;       /*7*1024 + 115*/
        else if (w == 524291)  /*  0A   3 A  */
            w = 7284;       /*7*1024 + 116*/
        else if (w == 88178690) /*N 2T 2X0    */
            w = 7285;       /*7*1024 + 117*/
        else if (w == 71827457) /*  2B   1 A  */
            w = 7286;       /*7*1024 + 118*/
        else if (w == 1048577) /*  0A 1X0 B  */
            w = 7287;       /*7*1024 + 119*/
        else if (w == 20971522) /*  2S 2X0    */
            w = 7288;       /*7*1024 + 120*/
        else if (w == 4784128) /*Y 2A   0 A  */
            w = 7289;       /*7*1024 + 121*/
        else if (w == 8388608) /*  4A 0X0    */
            w = 7290;       /*7*1024 + 122*/
        else if (w == 4390912) /*Y 2A 0X0   P*/
            w = 7291;       /*7*1024 + 123*/
        else if (w == 13172736) /*Y 6A   0 A */
            w = 7292;       /*7*1024 + 124*/
        else if (w == 1572865) /*  0A 1X0 C  */
            w = 7293;       /*7*1024 + 125*/
        else if (w == 524288)  /*  0A     0 A */
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

/* Local variables for main_scan: */
struct LOC_main_scan {
    jmp_buf _JL9801;
};

Local Void fill_t_list_with_delimiter()
{
    /*ZW*/
    fill_t_list(d8 * oh + dl);
} /*fill_t_list_with_delimiter*/

Local Void fill_future_list(int place, int value)
{
    /*FU*/
    int i, FORLIM;

    if (place >= klib) {
        if (nlib + nlsc + 16 >= plib)
            stop(6, "fill_future_list: Store capacity too small");
        FORLIM = klib;
        for (i = nlib + nlsc - 1; i >= FORLIM; i--)
            store[i + 16] = store[i];
        klib += 16;
        nlib += 16;
    }
    store[place] = value;
} /*fill_future_list*/

Local Void fill_constant_list(int n)
{
    /*KU*/
    int i, FORLIM;

    if (klib + klsc == nlib) {
        if (nlib + nlsc + 16 >= plib)
            stop(18, "Too many constants");
        FORLIM = nlib;
        for (i = nlib + nlsc - 1; i >= FORLIM; i--)
            store[i + 16] = store[i];
        nlib += 16;
    }
    if (n >= 0)
        store[klib + klsc] = n;
    else
        store[klib + klsc] = mz + n;
    /*one's complement representation*/
    klsc++;
} /*fill_constant_list*/

Local Void unload_t_list_element(int *variable)
{
    /*ZU*/
    tlsc--;
    *variable = store[tlsc];
} /*unload_t_list_element*/

Local Void fill_output(int c)
{
    pos_++;
    if (c < 10) {
        putchar((Char)(c + '0'));
        return;
    }
    if (c < 36) {
        putchar((Char)(c + 'a' - 10));
        return;
    }
    if (c < 64) {
        putchar((Char)(c + 'A' - 37));
        return;
    }
    if (c == 184) {
        putchar(' ');
        return;
    }
    if (c == 138) {
        printf("%*c", (int)(8 - ((pos_ - 1) & 7)), ' ');
        pos_ += 8 - ((pos_ - 1) & 7);
    } else {
        putchar('\n');
        pos_ = 0;
    }
} /*fill_output*/

Local Void offer_character_to_typewriter(int c)
{
    /*HS*/
    c &= 63;
    if (c < 63)
        fill_output(c);
} /*offer_character_to_typewriter*/

Local Void label_declaration()
{
    /*FY*/
    int id1, id2, i, w;

    id1 = store[nlib + nid];
    if (((id1 / d15) & 1) == 0) /*preceding applied occurrences*/
        fill_future_list(flib + (id1 & (d15 - 1)), rlsc);
    else
        store[nlib + nid] = id1 - d15 + d24 + rlsc;
    /*first occurrence*/
    id1 = store[nlib + nid - 1];
    if ((id1 & (d3 - 1)) == 0) { /*at most 4 letters/digits*/
        i = 4;
        id1 /= d3;
        while ((id1 & (d6 - 1)) == 0) { /*void*/
            i--;
            id1 /= d6;
        }
        do {
            offer_character_to_typewriter(id1);
            i--;
            id1 /= d6;
        } while (i != 0);
    } else {
        id2 = store[nlib + nid - 2];
        id2 = id2 / d3 + (id2 & (d3 - 1)) * d24;
        w   = (id2 & (d24 - 1)) * d3 + id1 / d24;
        id1 = (id1 & (d24 - 1)) * d3 + id2 / d24;
        id2 = w;
        i   = 9;
        do {
            offer_character_to_typewriter(id1);
            i--;
            w   = id2 / d6 + (id1 & (d6 - 1)) * d21;
            id1 = id1 / d6 + (id2 & (d6 - 1)) * d21;
            id2 = w;
        } while (i != 0);
    }
    /*TAB*/
    fill_output(138);
    w = rlsc;
    for (i = 1; i <= 3; i++) { /*NLCR*/
        offer_character_to_typewriter(w / d10 / 10);
        offer_character_to_typewriter(w / d10 % 10);
        w = (w & (d10 - 1)) * d5;
        if (i < 3) /*SPACE*/
            fill_output(184);
    }
    fill_output(139);
} /*label_declaration*/

Local Void test_first_occurrence()
{
    /*LF*/
    id = store[nlib + nid];
    if (((id / d15) & 1) != 1) /*first occurrence*/
        return;
    id += d24 * 2 - (id & (d15 - 1)) - d15 + flsc;
    if (nid <= nlsc0) /*MCP*/
        fill_future_list(flib + flsc, store[nlib + nid]);
    store[nlib + nid] = id;
    flsc++;
} /*test_first_occurrence*/

Local Void new_block_by_declaration1()
{ /*2B 'bn' A*/
    /*HU*/
    fill_result_list(0, bn + 71827456); /*SCC*/
    fill_result_list(89, 0);
    pnlv = bn + 160;
    vlam = pnlv;
} /*new_block_by_declaration1*/

Local Void new_block_by_declaration()
{
    /*HU*/
    if (store[tlsc - 2] == 161) /*block-begin marker*/
        return;
    tlsc--; /*remove 'begin'*/
    /*2A 0 A*/
    fill_result_list(0, 4718592);        /*2B 'rlsc+3' A*/
    fill_result_list(1, rlsc + 71827459); /*ETMP*/
    fill_result_list(9, 0);              /*2T 'flsc'*/
    fill_result_list(2, flsc + 88080384);
    fill_t_list(flsc);
    flsc++;
    intro_new_block(); /*begin*/
    fill_t_list(104);
    new_block_by_declaration1();
} /*new_block_by_declaration*/

Local Void fill_name_list()
{
    /*HN*/
    nlsc += dflag + 2;
    if (nlsc + nlib > plib)
        stop(16, "Too many names");
    store[nlib + nlsc - 1] = id;
    store[nlib + nlsc - 2] = inw;
    if ((inw & (d3 - 1)) > 0)
        store[nlib + nlsc - 3] = fnw;
} /*fill_name_list*/

Local Void reservation_of_local_variables()
{
    /*KY*/
    if (lvc <= 0) /*2A 'lvc' A*/
        return;
    fill_result_list(0, lvc + 4718592); /*4A 17X1*/
    fill_result_list(0, 8388657);       /*4A 18X1*/
    fill_result_list(0, 8388658);
} /*reservation_of_local_variables*/

Local Void address_to_register()
{
    /*ZR*/
    if (((id / d15) & 1) != 0) { /*static addressing*/
        /*2B 'static address' A*/
        fill_result_list(0, (id & (d15 - 1)) + 21495808);
        /*2S 'dynamic address' A*/
        return;
    }
    if (((id / d24) & (d2 - 1)) == 2) /*future list*/
        fill_result_list(2, (id & (d15 - 1)) + 71303168);
    /*2B 'FLI-address'*/
    else
        fill_result_list((id / d24) & 3, (id & (d15 - 1)) + 71827456);
} /*address_to_register*/

Local Void generate_address()
{
    /*ZH*/
    int opc = 14;

    address_to_register();
    if (((id / d16) & 1) == 1) { /*formal*/
        fill_result_list(18, 0);
        return;
    }
    /*TFA*/
    /*TRAD*/
    if (((id / d15) & 1) == 0)
        opc++; /*TRAS*/
    if (((id / d19) & 1) == 1)
        opc += 2; /*TIAD or TIAS*/
    fill_result_list(opc, 0);
} /*generate_address*/

Local Void reservation_of_arrays()
{
    /*KN*/
    if (vlam == 0)
        return;
    vlam = 0;
    if (store[tlsc - 1] == 161) /*block-begin marker*/
        rlaa = nlib + store[tlsc - 2];
    else
        rlaa = nlib + store[tlsc - 3];
    rlab = nlib + nlsc;
    while (rlab != rlaa) {
        id = Rstore(rlab - 1);
        if (id >= d26 && id < d25 + d26) { /*value array:*/
            address_to_register();
            if (((id / d19) & 1) == 0) /*RVA*/
                fill_result_list(92, 0);
            else
                fill_result_list(93, 0);
            /*IVA*/
            store[rlab - 1] = id / d15 * d15 - d16 + pnlv;
            pnlv += 256; /*at most 5 indices*/
        }
        if ((store[rlab - 2] & (d3 - 1)) == 0)
            rlab -= 2;
        else
            rlab -= 3;
    }
    rlab = nlib + nlsc;
    while (rlab != rlaa) {
        if (Rstore(rlab - 1) >= d26) {
            id = store[rlab - 1] - d26;
            if (id < d25) {
                address_to_register(); /*VAP*/
                fill_result_list(95, 0);
            } else { /*LAP*/
                id -= d25;
                address_to_register();
                fill_result_list(94, 0);
            }
        }
        if ((store[rlab - 2] & (d3 - 1)) == 0)
            rlab -= 2;
        else
            rlab -= 3;
    }
    if (nflag != 0)
        id = store[nlib + nid];
} /*reservation_of_arrays*/

Local Void procedure_statement(struct LOC_main_scan *LINK)
{
    /*LH*/
    if (eflag == 0)
        reservation_of_arrays();
    if (nid > nlscop) {
        if (fflag == 0)
            test_first_occurrence();
        address_to_register();
        return;
    }
    fill_t_list(store[nlib + nid] & (d12 - 1));
    if (dl == 98) { /*(*/
        eflag = 1;
        longjmp(LINK->_JL9801, 1);
    }
} /*procedure_statement*/

Local Void production_transmark()
{
    /*ZL*/
    fill_result_list(fflag * 2 - eflag + 9, 0);
} /*production_transmark*/

Local Void production_of_object_program(int opht, struct LOC_main_scan *LINK)
{
    /*ZS*/
    int operator_, block_number;

    oh = opht;
    if (nflag != 0) {
        nflag = 0;
        aflag = 0;
        if (pflag == 0) {
            if (jflag == 0) {
                address_to_register();
                if (oh > ((store[tlsc - 1] / d8) & 15))
                    operator_ = 315; /*5*63*/
                else {
                    operator_ = store[tlsc - 1] & (d8 - 1);
                    if (operator_ <= 63 || operator_ > 67)
                        operator_ = 315; /*5*63*/
                    else {
                        tlsc--;
                        operator_ *= 5;
                    }
                }
                if (fflag == 0) {
                    if (((id / d15) & 1) == 0)
                        operator_++;
                    if (((id / d19) & 1) != 0)
                        operator_ += 2;
                    fill_result_list(operator_ - 284, 0);
                } else
                    fill_result_list(operator_ - 280, 0);
            } else if (fflag == 0) {
                block_number = (id / d19) & (d5 - 1);
                if (block_number != bn) {
                    fill_result_list(0, block_number + 71827456); /*GTA*/
                    fill_result_list(28, 0);
                }
                test_first_occurrence();
                if (((id / d24) & 3) == 2) {
                    /*2T 'address'*/
                    fill_result_list(2, (id & (d15 - 1)) + 88080384);
                } else {
                    /*2T 'address' A*/
                    fill_result_list(1, (id & (d15 - 1)) + 88604672);
                }
            } else {
                address_to_register(); /*TFR*/
                fill_result_list(35, 0);
            }
        } else { /*2A 0 A*/
            procedure_statement(LINK);
            if (nid > nlscop) {
                fill_result_list(0, 4718592);
                production_transmark();
            }
        }
    } else if (aflag != 0) {
        aflag = 0; /*TAR*/
        fill_result_list(58, 0);
    }
    while (oh <= ((store[tlsc - 1] / d8) & 15)) {
        tlsc--;
        operator_ = store[tlsc] & (d8 - 1);
        if (operator_ > 63 && operator_ <= 80) {
            fill_result_list(operator_ - 5, 0);
            continue;
        }
        if (operator_ == 132) { /*NEG*/
            fill_result_list(57, 0);
            continue;
        }
        /*NEG*/
        if (operator_ < 132 && operator_ > 127) { /*ST,STA,STP,STAP*/
            if (operator_ > 129) {                /*STP,STAP*/
                tlsc--;                           /*2B 'BN' A*/
                fill_result_list(0, store[tlsc] + 71827456);
            }
            fill_result_list(operator_ - 43, 0);
            continue;
        }
        if (operator_ > 127 && operator_ <= 141)
            fill_result_list(operator_ - 57, 0);
        else if (operator_ > 141 && operator_ <= 152)
            fill_result_list(operator_ - 40, 0);
        else {
            stop(22, "Special function in object program");
            /*special function*/
        }
    }
    if (tfpflag) {
        nid = 99999;
        tfpflag = 0;
    }
} /*production_of_object_program*/

Local boolean thenelse()
{
    /*ZN*/
    if (store[tlsc - 1] % 255 == 83 || store[tlsc - 1] % 255 == 84) { /*then*/
        tlsc -= 2;
        fill_future_list(flib + store[tlsc], rlsc);
        unload_t_list_element(&eflag);
        return true;
    }
    /*else*/
    else
        return false;
} /*thenelse*/

Local Void empty_t_list_through_thenelse(struct LOC_main_scan *LINK)
{
    /*FR*/
    oflag = 1;
    do {
        production_of_object_program(1, LINK);
    } while (thenelse()); /*empty_t_list_through_thenelse*/
}

Local boolean do_in_t_list()
{
    /*ER*/
    if (store[tlsc - 1] % 255 == 86) {
        tlsc -= 5;
        nlsc = store[tlsc + 2];
        bn--;
        fill_future_list(flib + store[tlsc + 1], rlsc + 1); /*2T 0X0 A*/
        fill_result_list(1, store[tlsc] + 88604672);
        return true;
    } else
        return false;
} /*do_in_t_list*/

Void show_word1(int word)
{
    int s;
    int b = 0;
    for (s = 3; s <= 21; s += 6) {
        int c = (word >> s) & 077;
        b |= c;
        if (b)
            putchar(alnum_table[c]);
    }
}

Void show_word2(int word)
{
    int s;
    for (s = 3; s <= 21; s += 6) {
        int c = (word >> s) & 077;
        if (c == 077)
            break;
        putchar(alnum_table[c]);
    }
}

Void show_id(int word1, int word2)
{
    if (word1 % 8 == 0) {
        show_word1(word1);
    } else {
        putchar(alnum_table[(word1 & 7) << 3 | (word2 & 7)]);
        show_word1(word1);
        show_word2(word2);
    }
}

Local Void look_for_name()
{
    /*HZ*/
    int i, w;

    i = nlib + nlsc;
_L1:
    w = store[i - 2];
    if (w == inw) {
        if ((w & 7) == 0) /*at most 4 letters/digits*/
            goto _L2;
        if (store[i - 3] == fnw)
            goto _L2;
    }
    /*more than 4 letters/digits*/
    if ((w & 7) == 0)
        i -= 2;
    else
        i -= 3;
    if (i > nlib)
        goto _L1;
    show_id(inw, fnw);
    stop(7, "Identifier not declared");
_L2:
    nid   = i - nlib - 1;
    id    = store[i - 1];
    pflag = (id / d18) & 1;
    jflag = (id / d17) & 1;
    fflag = (id / d16) & 1;
} /*look_for_name*/

Local Void look_for_constant()
{
    /*FW*/
    int i;

    if (klib + klsc + dflag >= nlib) { /*move name list*/
        if (nlib + nlsc + 16 >= plib)
            stop(5, "Exhausted constant pool");
        for (i = nlsc - 1; i >= 0; i--)
            store[nlib + i + 16] = store[nlib + i];
        nlib += 16;
    }
    if (dflag == 0) { /*search integer constant*/
        store[klib + klsc] = inw;
        i                  = 0;
        while (store[klib + i] != inw)
            i++;
    } else {
        store[klib + klsc]     = fnw;
        store[klib + klsc + 1] = inw;
        i                      = 0;
        while (store[klib + i] != fnw || store[klib + i + 1] != inw)
            i++;
    }
    /*search floating constant*/
    if (i == klsc) /*first occurrence*/
        klsc += dflag + 1;
    id = d24 * 3 + i;
    if (dflag == 0)
        id += d19;
    jflag = 0;
    pflag = 0;
    fflag = 0;
} /*look_for_constant*/

Static Void main_scan()
{ /*body of main scan*/
    /*EL*/
    struct LOC_main_scan V;

    /*EL*/
    if (setjmp(V._JL9801))
        goto _L9801;
_L1:
    read_until_next_delimiter();
_L2:
    if (nflag != 0) {
        if (kflag == 0)
            look_for_name();
        else
            look_for_constant();
    } else {
        jflag = 0;
        pflag = 0;
        fflag = 0;
    }
_L3:
    if (dl <= 65) /*+,-*/
        goto _L64;
    /*EH*/
    if (dl <= 68) /**,/,_:*/
        goto _L66;
    if (dl <= 69) /*|^*/
        goto _L69;
    if (dl <= 75) /*<,_<,=,_>,>,|=*/
        goto _L70;
    if (dl <= 80) /*~,^,`,=>,_=*/
        goto _L76;
    switch (dl) {
    case 81: /*goto*/
        goto _L81;
        /*KR*/

    case 82: /*if*/
        goto _L82;
        /*EY*/

    case 83: /*then*/
        goto _L83;
        /*EN*/

    case 84: /*else*/
        goto _L84;
        /*FZ*/

    case 85: /*for*/
        goto _L85;
        /*FE*/

    case 86: /*do*/
        goto _L86;
        /*FL*/

    case 87: /*,*/
        goto _L87;
        /*EK*/

    case 90: /*:*/
        goto _L90;
        /*FN*/

    case 91: /*;*/
        goto _L91;
        /*FS*/

    case 92: /*:=*/
        goto _L92;
        /*EZ*/

    case 94: /*step*/
        goto _L94;
        /*FH*/

    case 95: /*until*/
        goto _L95;
        /*FK*/

    case 96: /*while*/
        goto _L96;
        /*FF*/

    case 98: /*(*/
        goto _L98;
        /*EW*/

    case 99: /*)*/
        goto _L99;
        /*EU*/

    case 100: /*[*/
        goto _L100;
        /*EE*/

    case 101: /*]*/
        goto _L101;
        /*EF*/

    case 102: /*|<*/
        goto _L102;
        /*KS*/

    case 104: /*begin*/
        goto _L104;
        /*LZ*/

    case 105: /*end*/
        goto _L105;
        /*FS*/

    case 106: /*own*/
        goto _L106;
        /*KH*/

    case 107: /*Boolean*/
        goto _L107;
        /*KZ*/

    case 108: /*integer*/
        goto _L108;
        /*KZ*/

    case 109: /*real*/
        goto _L109;
        /*KE*/

    case 110: /*array*/
        goto _L110;
        /*KF*/

    case 111: /*switch*/
        goto _L111;
        /*HE*/

    case 112: /*procedure*/
        goto _L112;
        /*HY*/
    }

_L64: /*+,-*/
    /*ES*/
    if (oflag == 0) {
        production_of_object_program(9, &V);
        fill_t_list_with_delimiter();
    } else if (dl == 65) { /*-*/
        oh = 10;
        dl = 132; /*NEG*/
        fill_t_list_with_delimiter();
    }
    goto _L1;
_L66: /**,/,_:*/
    /*ET*/
    production_of_object_program(10, &V);
    fill_t_list_with_delimiter();
    goto _L1;

_L69: /*|^*/
    /*KT*/
    production_of_object_program(11, &V);
    fill_t_list_with_delimiter();
    goto _L1;

_L70: /*<,_<,=,_>,>,|=*/
    /*KK*/
    oflag = 1;
    production_of_object_program(8, &V);
    fill_t_list_with_delimiter();
    goto _L1;

_L76: /*~,^,`,=>,_=*/
    /*KL*/
    if (dl == 76) { /*~*/
        oh = 83 - dl;
        goto _L8202;
    }
    production_of_object_program(83 - dl, &V);
    fill_t_list_with_delimiter();
    goto _L1;

_L81: /*goto*/
    /*KR*/
    reservation_of_arrays();
    goto _L1;

_L82: /*if*/
    /*EY*/
    if (eflag == 0)
        reservation_of_arrays();
    fill_t_list(eflag);
    eflag = 1;
_L8201:
    oh = 0;
_L8202:
    fill_t_list_with_delimiter();
    oflag = 1;
    goto _L1;

_L83: /*then*/
    /*EN*/
    do {
        production_of_object_program(1, &V);
    } while (thenelse());
    tlsc--;
    eflag = store[tlsc - 1];   /*CAC*/
    fill_result_list(30, 0); /*N 2T 'flsc'*/
    fill_result_list(2, flsc + 88178688);
_L8301:
    fill_t_list(flsc);
    flsc++;
    goto _L8201;

_L84: /*else*/
    /*FZ*/
    production_of_object_program(1, &V);
    if ((store[tlsc - 1] & (d8 - 1)) == 84) { /*else*/
        if (thenelse())
            goto _L84;
    }
_L8401:
    if (do_in_t_list())
        goto _L8401;
    if (store[tlsc - 1] == 161) { /*block-begin marker*/
        tlsc -= 3;
        nlsc = store[tlsc + 1];
        fill_future_list(flib + store[tlsc], rlsc + 1); /*RET*/
        fill_result_list(12, 0);
        bn--;
        goto _L8401;
    }
    /*2T 'flsc'*/
    fill_result_list(2, flsc + 88080384);
    if (thenelse()) /*finds 'then'!*/
        tlsc++;       /*keep eflag in t_list*/
    goto _L8301;

_L85: /*for*/
    /*FE*/
    reservation_of_arrays(); /*2T 'flsc'*/
    fill_result_list(2, flsc + 88080384);
    fora = flsc;
    flsc++;
    fill_t_list(rlsc);
    vflag = 1;
    bn++;
_L8501:
    oh = 0;
    fill_t_list_with_delimiter();
    goto _L1;

_L86: /*do*/
    /*FL*/
    empty_t_list_through_thenelse(&V);
    goto _L8701; /*execute part of DDEL ,*/
_L8601:          /*returned from DDEL ,*/
    vflag = 0;
    tlsc--; /*2S 'flsc'*/
    fill_result_list(2, flsc + 20971520);
    fill_t_list(flsc);
    flsc++; /*FOR8*/
    fill_result_list(27, 0);
    fill_future_list(flib + fora, rlsc); /*FOR0*/
    fill_result_list(19, 0);               /*2T 0X0 A*/
    fill_result_list(1, store[tlsc - 2] + 88604672);
    fill_future_list(flib + forc, rlsc);
    eflag = 0;
    intro_new_block1();
    goto _L8501;

_L87: /*,*/
    /*EK*/
    oflag = 1;
    if (iflag == 1) { /*subscript separator:*/
        do {
            production_of_object_program(1, &V);
        } while (thenelse());
        goto _L1;
    }
    if (vflag == 0)
        goto _L8702;
    /*for-list separator:*/
    do {
        production_of_object_program(1, &V);
    } while (thenelse());
_L8701:
    if ((store[tlsc - 1] & (d8 - 1)) == 85) /*for*/
        fill_result_list(21, 0);
    /*for2*/
    else { /*while*/
        tlsc--;
        if ((store[tlsc] & (d8 - 1)) == 96) /*for4*/
            fill_result_list(23, 0);
        else {
            fill_result_list(26, 0);
            /*for7*/
        }
    }
    if (dl == 86) /*do*/
        goto _L8601;
    goto _L1;
_L8702:
    if (mflag == 0)
        goto _L8705;
    /*actual parameter separator:*/
    if ((store[tlsc - 1] & (d8 - 1)) == 87) { /*,*/
        if (aflag == 0) {
            if (store[tlsc - 2] == rlsc && (fflag == 0 || pass_formals) &&
                jflag == 0 && nflag == 1) {
                if (nid > nlscop) {
                    if (pflag == 1 && fflag == 0) /*non-formal procedure:*/
                        test_first_occurrence();
                    /*PORD construction:*/
                    if (((id / d15) & 1) == 0) { /*static addressing*/
                        pstb = ((id / d24) & (d2 - 1)) * d24 + (id & (d15 - 1));
                        if (((id / d24) & (d2 - 1)) == 2)
                            pstb += d17;
                    } else {
                        pstb = d16 + (id & (d5 - 1)) * d22 + ((id / d5) & (d10 - 1));
                        if (((id / d16) & 1) == 1) {
                            store[tlsc - 2] = pstb + d17;
                            goto _L8704;
                        }
                    }
                    /*dynamic addressing*/
                    if (((id / d18) & 1) == 1)
                        store[tlsc - 2] = pstb + d20;
                    else if (((id / d19) & 1) == 1)
                        store[tlsc - 2] = pstb + d19;
                    else
                        store[tlsc - 2] = pstb;
                    goto _L8704;
                } else {
                    tfpflag = 1;
                    fill_result_list(98, 0);
                    goto _L8703;
                }
            } else {
                /*TFP*/
                goto _L8703;
            }
        } else {
            store[tlsc - 2] += d19 + d20 + d24; /*EIS*/
            fill_result_list(13, 0);
            goto _L8704;
        }
    }
    /*completion of implicit subroutine:*/
_L8703: /*completion of implicit subroutine:*/
    do {
        production_of_object_program(1, &V);
    } while (thenelse() | do_in_t_list());
    store[tlsc - 2] += d20 + d24; /*EIS*/
    fill_result_list(13, 0);
_L8704:
    if (dl == 87)    /*,*/
        goto _L9804; /*prepare next parameter*/
    /*production of PORDs:*/
    psta = 0;
    unload_t_list_element(&pstb);
    while ((pstb & (d8 - 1)) == 87) { /*,*/
        psta++;
        unload_t_list_element(&pstb);
        if (((pstb / d16) & 1) == 0)
            fill_result_list(pstb / d24, pstb & (d24 - 1));
        else
            fill_result_list(0, pstb);
        unload_t_list_element(&pstb);
    }
    tlsc--;
    fill_future_list(flib + store[tlsc], rlsc); /*2A 'psta' A*/
    fill_result_list(0, psta + 4718592);
    bn--;
    unload_t_list_element(&fflag);
    unload_t_list_element(&eflag);
    production_transmark();
    aflag = 0;
    unload_t_list_element(&mflag);
    unload_t_list_element(&vflag);
    unload_t_list_element(&iflag);
    goto _L1;
_L8705:
    empty_t_list_through_thenelse(&V);
    if (sflag == 0) /*array declaration*/
        goto _L1;
    /*switch declaration:*/
    oh = 0;
    dl = 160;
    fill_t_list(rlsc);
    fill_t_list_with_delimiter();
    goto _L1;

_L90: /*:*/
    /*FN*/
    if (jflag == 0) { /*array declaration*/
        ic++;
        empty_t_list_through_thenelse(&V);
    } else {
        reservation_of_arrays();
        label_declaration();
    }
    /*label declaration*/
    goto _L1;

_L91:
    goto _L105; /*end*/

_L92: /*:=*/
    /*EZ*/
    reservation_of_arrays();
    dl    = 128; /*ST*/
    oflag = 1;
    if (vflag == 0) {
        if (sflag == 0) { /*assignment statement*/
            if (eflag == 0)
                eflag = 1;
            else
                dl = 129; /*STA*/
            oh = 2;
            if (pflag == 0) {   /*assignment to variable*/
                if (nflag != 0) /*assignment to scalar*/
                    generate_address();
            } else {
                dl += 2; /*STP or STAP*/
                /*bn from id*/
                fill_t_list((id / d19) & (d5 - 1));
            }
            /*assignment to function identifier*/
            fill_t_list_with_delimiter();
        } else {
            fill_result_list(2, flsc + 88080384);
            fill_t_list(flsc);
            flsc++;
            fill_t_list(nid);
            oh = 0;
            fill_t_list_with_delimiter();
            dl = 160;
            fill_t_list(rlsc);
            fill_t_list_with_delimiter();
        }
    } else { /*simple variable*/
        /*switch declaration*/
        /*2T 'flsc'*/
        eflag = 1;
        if (nflag != 0) /*FOR1*/
            generate_address();
        fill_result_list(20, 0);
        forc = flsc; /*2T 'flsc'*/
        fill_result_list(2, flsc + 88080384);
        flsc++;
        fill_future_list(flib + fora, rlsc); /*2A 0 A*/
        fill_result_list(0, 4718592);
        fora = flsc; /*2B 'flsc*/
        fill_result_list(2, flsc + 71303168);
        flsc++; /*ETMP*/
        fill_result_list(9, 0);
    }
    /*for statement*/
    goto _L1;

_L94: /*step*/
    /*FH*/
    empty_t_list_through_thenelse(&V); /*FOR5*/
    fill_result_list(24, 0);
    goto _L1;

_L95: /*until*/
    /*FK*/
    empty_t_list_through_thenelse(&V); /*FOR6*/
    fill_result_list(25, 0);
    goto _L8501;

_L96: /*while*/
    /*FF*/
    empty_t_list_through_thenelse(&V); /*FOR3*/
    fill_result_list(22, 0);
    goto _L8501;

_L98: /*(*/
    /*EW*/
    oflag = 1;
    if (pflag == 1)
        goto _L9803;
_L9801: /*parenthesis in expression:*/
    fill_t_list(mflag);
    mflag = 0;
_L9802:
    oh = 0;
    fill_t_list_with_delimiter();
    goto _L1;
_L9803:                      /*begin of parameter list:*/
    procedure_statement(&V); /*2T 'flsc'*/
    fill_result_list(2, flsc + 88080384);
    fill_t_list(iflag);
    fill_t_list(vflag);
    fill_t_list(mflag);
    fill_t_list(eflag);
    fill_t_list(fflag);
    fill_t_list(flsc);
    iflag = 0;
    vflag = 0;
    mflag = 1;
    eflag = 1;
    flsc++;
    oh = 0;
    bn++;
    fill_t_list_with_delimiter();
    dl = 87; /*,*/
_L9804:      /*prepare parsing of actual parameter:*/
    fill_t_list(rlsc);
    aflag = 0;
    goto _L9802;

_L99: /*)*/
    /*EU*/
    if (mflag == 1)
        goto _L8702;
    do {
        production_of_object_program(1, &V);
    } while (thenelse());
    tlsc--;
    unload_t_list_element(&mflag);
    goto _L1;

_L100: /*[*/
    /*EE*/
    if (eflag == 0)
        reservation_of_arrays();
    oflag = 1;
    oh    = 0;
    fill_t_list(eflag);
    fill_t_list(iflag);
    fill_t_list(mflag);
    fill_t_list(fflag);
    fill_t_list(jflag);
    fill_t_list(nid);
    eflag = 1;
    iflag = 1;
    mflag = 0;
    fill_t_list_with_delimiter();
    if (jflag == 0) /*of storage function*/
        generate_address();
    goto _L1;

_L101: /*]*/
    /*EF*/
    do {
        production_of_object_program(1, &V);
    } while (thenelse());
    tlsc--;
    if (iflag == 0)                            /*2S 'aic' A*/
    {                                          /*array declaration:*/
        fill_result_list(0, aic + 21495808); /*RSF*/
        /*RSF or ISF*/
        fill_result_list(ibd + 90, 0);
        arrb = d15 + d25 + d26;
        if (ibd == 1)
            arrb += d19;
        arra = nlib + nlsc;
        do {
            Wstore(arra - 1, arrb + pnlv);
            if ((store[arra - 2] & (d3 - 1)) == 0)
                arra -= 2;
            else
                arra -= 3;
            pnlv += (ic + 3) * d5;
            aic--;
        } while (aic != 0);
        read_until_next_delimiter();
        if (dl != 91)
            goto _L1103;
        eflag = 0;
        goto _L1;
    }
    unload_t_list_element(&nid);
    unload_t_list_element(&jflag);
    unload_t_list_element(&fflag);
    unload_t_list_element(&mflag);
    unload_t_list_element(&iflag);
    unload_t_list_element(&eflag);
    if (jflag == 0) { /*subscripted variable:*/
        aflag = 1;    /*IND*/
        fill_result_list(56, 0);
        goto _L1;
    }
    /*switch designator:*/
    nflag = 1; /*SSI*/
    fill_result_list(29, 0);
    read_next_symbol();
    id    = store[nlib + nid];
    pflag = 0;
    goto _L3;

_L102: /*|<*/
    /*KS*/
    qc = 1;
    qb = 0;
    qa = 1;
    do { /*end marker*/
        read_next_symbol();
        if (dl == 102) /*|<*/
            qc++;
        if (dl == 103) /*|>*/
            qc--;
        if (qc > 0) {
            qb += dl * qa;
            qa *= d8;
            if (qa == d24) {
                fill_result_list(0, qb);
                qb = 0;
                qa = 1;
            }
        }
    } while (qc != 0);
    fill_result_list(0, qb + qa * 255);
    oflag = 0;
    goto _L1;

_L104: /*begin*/
    /*LZ*/
    if (store[tlsc - 1] != 161) /*block-begin marker*/
        reservation_of_arrays();
    goto _L8501;

_L105: /*end*/
    /*FS*/
    reservation_of_arrays();
    do {
        empty_t_list_through_thenelse(&V);
    } while (do_in_t_list());
    if (sflag == 0) {
        if (store[tlsc - 1] == 161) { /*blok-begin marker*/
            tlsc -= 3;
            nlsc = store[tlsc + 1];
            fill_future_list(flib + store[tlsc], rlsc + 1); /*RET*/
            fill_result_list(12, 0);
            bn--;
            goto _L105;
        }
    } else {
        sflag = 0;
        do {
            tlsc -= 2;
            /*2T 'stacked RLSC' A*/
            fill_result_list(1, store[tlsc] + 88604672); /*switch comma*/
        } while (store[tlsc - 1] == 160);
        tlsc--;
        unload_t_list_element(&nid);
        label_declaration(); /*1T 16X1*/
        fill_result_list(0, 85983280);
        tlsc--;
        fill_future_list(flib + store[tlsc], rlsc);
    }
    /*end of switch declaration*/
    eflag = 0;
    if (dl != 105) /*end*/
        goto _L1;
    tlsc--;
    if (tlsc == tlib + 1)
        goto _L1052;
    do {
        read_next_symbol(); /*;*/
                            /*else*/
                            /*end*/
    } while (dl != 91 && dl != 84 && dl != 105);
    jflag = 0;
    pflag = 0;
    fflag = 0;
    nflag = 0;
    goto _L2;

_L106: /*own*/
    /*KH*/
    new_block_by_declaration();
    read_next_symbol();
    if (dl == 109) /*real*/
        ibd = 0;
    else
        ibd = 1;
    read_until_next_delimiter();
    if (nflag == 0)
        goto _L1102;
    goto _L1082;

_L107: /*Boolean*/
    /*KZ*/
    goto _L108; /*integer*/

_L108: /*integer*/
    /*KZ*/
    ibd = 1;
    new_block_by_declaration();
    read_until_next_delimiter();
_L1081:
    if (nflag == 0) {
        if (dl == 110) /*array*/
            goto _L1101;
        goto _L112; /*procedure*/
    }
    /*scalar:*/
    if (bn != 0)
        goto _L1083;
_L1082: /*static addressing*/
    id = gvc;
    if (ibd == 1) {
        id += d19;
        gvc++;
    } else
        gvc += 2;
    fill_name_list();
    if (dl == 87) { /*,*/
        read_until_next_delimiter();
        goto _L1082;
    }
    goto _L1;
_L1083: /*dynamic addressing*/
    id = pnlv + d15;
    if (ibd == 1) {
        id += d19;
        pnlv += 32;
        lvc++;
    } else {
        pnlv += 64;
        lvc += 2;
    }
    fill_name_list();
    if (dl == 87) { /*,*/
        read_until_next_delimiter();
        goto _L1083;
    }
    read_until_next_delimiter();
    if (dl <= 106 || dl > 109) { /*own*/
        reservation_of_local_variables();
        goto _L2;
    }
    /*real*/
    if (dl == 109) /*real*/
        ibd = 0;
    else
        ibd = 1;
    read_until_next_delimiter();
    if (nflag == 1)
        goto _L1083; /*more scalars*/
    reservation_of_local_variables();
    if (dl == 110) /*array*/
        goto _L1101;
    goto _L3;

_L109: /*real*/
    /*KE*/
    ibd = 0;
    new_block_by_declaration();
    read_until_next_delimiter();
    if (nflag == 1)
        goto _L1081;
    goto _L2;

_L110: /*array*/
    /*KF*/
    ibd = 0;
    new_block_by_declaration();
_L1101:
    if (bn != 0)
        goto _L1103;
_L1102: /*static bounds, constants only:*/
    id = d24 * 3;
    if (ibd != 0)
        id += d19;
    do {
        arra = nlsc;
        arrb = tlsc;
        do { /*read identifier list:*/
            read_until_next_delimiter();
            fill_name_list(); /*[*/
        } while (dl != 100);
        arrc = 0;
        fill_t_list(2 - ibd); /*delta[0]*/
        do {                  /*read bound-pair list:*/
            /*lower bound:*/
            read_until_next_delimiter();
            if (dl != 90) {     /*:*/
                if (dl == 64) { /*+*/
                    read_until_next_delimiter();
                    arrd = inw;
                } else {
                    read_until_next_delimiter();
                    arrd = -inw;
                }
            } else
                arrd = inw;
            arrc = (arrc - arrd * store[tlsc - 1]) & (d26 - 1);
            /*upper bound:*/
            read_until_next_delimiter();
            if (nflag == 0) {
                if (dl == 65) { /*-*/
                    read_until_next_delimiter();
                    arrd = -inw - arrd;
                } else {
                    read_until_next_delimiter();
                    arrd = inw - arrd;
                }
            } else
                arrd = inw - arrd;
            if (dl == 101) /*[*/
                fill_t_list(-(((arrd + 1) * store[tlsc - 1]) & (d26 - 1)));
            else
                fill_t_list(((arrd + 1) * store[tlsc - 1]) & (d26 - 1));
        } while (dl != 101);
        /*]*/
        arrd = nlsc;
        do { /*construction of storage function in constant list:*/
            store[nlib + arrd - 1] += klsc;
            fill_constant_list(gvc);
            fill_constant_list(gvc + arrc);
            tlsc = arrb;
            do {
                fill_constant_list(store[tlsc]);
                tlsc++;
            } while (store[tlsc - 1] > 0);
            gvc -= store[tlsc - 1];
            tlsc = arrb;
            if ((store[nlib + arrd - 2] & (d3 - 1)) == 0)
                arrd -= 2;
            else
                arrd -= 3;
        } while (arrd != arra);
        read_until_next_delimiter(); /*,*/
    } while (dl == 87);
    goto _L91; /*;*/
_L1103:        /*dynamic bounds,arithmetic expressions:*/
    ic  = 0;
    aic = 0;
    id  = 0;
    do {
        aic++;
        read_until_next_delimiter();
        fill_name_list(); /*,*/
    } while (dl == 87);
    eflag = 1;
    oflag = 1;
    goto _L8501;

_L111: /*switch*/
    /*HE*/
    reservation_of_arrays();
    sflag = 1;
    new_block_by_declaration();
    goto _L1;

_L112: /*procedure*/
    /*HY*/
    reservation_of_arrays();
    new_block_by_declaration(); /*2T 'flsc'*/
    fill_result_list(2, flsc + 88080384);
    fill_t_list(flsc);
    flsc++;
    read_until_next_delimiter();
    look_for_name();
    label_declaration();
    intro_new_block();
    new_block_by_declaration1();
    if (dl == 91) /*;*/
        goto _L1;
    /*formal parameter list:*/
    do {
        read_until_next_delimiter();
        id = pnlv + d15 + d16;
        fill_name_list();
        pnlv += d5 * 2; /*reservation PARD*/
    } while (dl == 87);
    read_until_next_delimiter(); /*for ; after )*/
_L1121:
    read_until_next_delimiter();
    if (nflag == 1)
        goto _L2;
    if (dl == 104) /*begin*/
        goto _L3;
    if (dl != 115)   /*value*/
        goto _L1123; /*specification part*/
    /*value part:*/
    spe = d26; /*value flag*/
_L1122:
    do {
        read_until_next_delimiter();
        look_for_name();
        store[nlib + nid] += spe;
    } while (dl == 87);
    goto _L1121;
_L1123:                           /*specification part:*/
    if (dl == 113 || dl == 110) { /*string*/
        spe = 0;
        goto _L1122;
    }
    /*array*/
    if (dl == 114 || dl == 111) { /*label*/
        spe = d17;
        goto _L1122;
    }
    /*switch*/
    if (dl == 112) { /*procedure*/
        spe = d18;
        goto _L1122;
    }
    if (dl == 109) /*real*/
        spe = 0;
    else
        spe = d19;
    if (dl <= 106 || dl > 109) /*if,for,goto*/
        goto _L3;
    read_until_next_delimiter();
    /*for delimiter following real/integer/boolean*/
    if (dl == 112) { /*procedure*/
        spe = d18;
        goto _L1122;
    }
    if (dl == 110) /*array*/
        goto _L1122;
_L1124:
    look_for_name();
    store[nlib + nid] += spe;
    if (store[nlib + nid] >= d26) {
        id                = store[nlib + nid] - d26;
        id                = id / d17 * d17 + (id & (d16 - 1));
        store[nlib + nid] = id;
        address_to_register(); /*generates 2S 'PARD position' A*/
        if (spe == 0)            /*TRAD*/
            fill_result_list(14, 0);
        else
            fill_result_list(16, 0);
        /*TIAD*/
        address_to_register(); /*generates 2S 'PARD position' A*/
        /*TFR*/
        fill_result_list(35, 0); /*ST*/
        fill_result_list(85, 0);
    }
    if (dl == 87) { /*,*/
        read_until_next_delimiter();
        goto _L1124;
    }
    goto _L1121;

_L1052:;
} /*main_scan*/

/* Local variables for program_loader: */
struct LOC_program_loader {
    int ll, list_address, crfa, heptade_count, parity_word, read_location, stock;
    char from_store;
};

Local int logical_sum(int n, int m)
{
    return n ^ m;
} /*logical_sum*/

Local int read_byte_from_lib_tape()
{
    int w;
    if (binary_lib_tape) {
        w = getc(lib_tape);
    } else if (fscanf(lib_tape, "%d", &w) <= 0) {
        return EOF;
    }
    if (w >= 128) {
        printf("Stray punch on library tape, current offset is %d\n", (int)ftell(lib_tape));
        exit(EXIT_FAILURE);
    }
    return w;
}

Local Void complete_bitstock(struct LOC_program_loader *LINK)
{
    /*RW*/
    int i, w = 0, FORLIM;

    while (bitcount > 0) { /*i.e., at most 20 bits in stock*/
        LINK->heptade_count++;
        switch (LINK->from_store) {
        case 0: /*bit string read from store:*/
            if (LINK->heptade_count > 0) {
                bitcount++;
                LINK->heptade_count = -3;
                LINK->read_location--;
                LINK->stock = store[LINK->read_location];
                w           = LINK->stock / d21;
                LINK->stock = (LINK->stock & (d21 - 1)) * 64;
            } else {
                w           = LINK->stock / d20;
                LINK->stock = (LINK->stock & (d20 - 1)) * 128;
            }
            break;

        case 1: /*bit string read from tape:*/
            w = read_byte_from_lib_tape();
            if (w == EOF) {
                printf("Unexpected end of library tape\n");
                exit(EXIT_FAILURE);
            }
            if (LINK->heptade_count > 0) {
                /*test parity of the previous 4 heptades*/
                bitcount++;
                LINK->parity_word =
                    logical_sum(LINK->parity_word, LINK->parity_word / d4) & (d4 - 1);
                if (LINK->parity_word < 32 &&
                    ((1 << LINK->parity_word) & 0x9669) != 0)
                    stop(105, "Parity error on library tape");
                LINK->heptade_count = -3;
                LINK->parity_word   = w;
                w /= 2;
            } else
                LINK->parity_word = logical_sum(LINK->parity_word, w);
            break;
        }
        FORLIM = bitcount;
        for (i = 1; i < FORLIM; i++)
            w *= 2;
        bitstock += w;
        bitcount -= 7;
    }
} /*complete_bitstock*/

Local int read_bit_string(int n, struct LOC_program_loader *LINK)
{
    /*RW*/
    int Result, i;
    int w = 0;

    for (i = 1; i <= n; i++) {
        w        = w * 2 + bitstock / d26;
        bitstock = (bitstock & (d26 - 1)) * 2;
    }
    Result = w;
    bitcount += n;
    complete_bitstock(LINK);
    return Result;
} /*read_bit_string*/

Local Void prepare_read_bit_string1(struct LOC_program_loader *LINK)
{
    int i, FORLIM;

    FORLIM = 27 - bitcount;
    for (i = 1; i <= FORLIM; i++)
        bitstock *= 2;
    bitcount            = 21 - bitcount;
    LINK->heptade_count = 0;
    LINK->from_store    = 0;
    complete_bitstock(LINK);
} /*prepare_read_bit_string1*/

Local Void prepare_read_bit_string2(struct LOC_program_loader *LINK)
{
    bitstock            = 0;
    bitcount            = 21;
    LINK->heptade_count = 0;
    LINK->from_store    = 0;
    complete_bitstock(LINK);
    do {
    } while (read_bit_string(1, LINK) != 1); /*prepare_read_bit_string2*/
}

Local Void prepare_read_bit_string3(struct LOC_program_loader *LINK)
{
    int w;

    LINK->from_store = 1;
    bitstock         = 0;
    bitcount         = 21;
    do {
        if ((w = read_byte_from_lib_tape()) == EOF)
            break;
    } while (w == 0);
    if (w == EOF)
        return;
    if (w != 30) /*D*/
        stop(106, "Bad tape header, expecting D");
    LINK->heptade_count = 0;
    LINK->parity_word   = 1;
    complete_bitstock(LINK);
    do {
    } while (read_bit_string(1, LINK) != 1); /*prepare_read_bit_string3*/
}

Local int address_decoding(struct LOC_program_loader *LINK)
{
    /*RY*/
    int w, a, n;

    w = bitstock;
    if (w < d26) { /*code starts with 0*/
        n = 1;
        a = 0;
        w *= 2;
    }
    /*0*/
    else {
        n = 6;
        a = (w / d21) & (d5 - 1);
        w = (w & (d21 - 1)) * d6;
    }
    /*1xxxxx*/
    if (w < d25) { /*00*/
        n += 2;
        a *= 32;
        w *= 4;
    }
    /*00*/
    else {
        if (w < d26) { /*01*/
            n += 4;
            a = a * 32 + w / d23;
            if ((a & (d5 - 1)) < 6) /*010x*/
                a -= 3;
            else
                a -= 2;
            /*011x*/
            w = (w & (d23 - 1)) * d4;
        }
        /*01xx*/
        else {
            n += 6;
            a = a * 32 + ((w / d21) & (d5 - 1));
            w = (w & (d21 - 1)) * d6;
            /*1xxxxx*/
        }
    }
    if (w < d25) { /*00*/
        n += 2;
        a = a * 32 + 1;
    }
    /*00*/
    else {
        if (w < d26) { /*01*/
            n += 3;
            a = a * 32 + w / d24;
        }
        /*01x*/
        else {
            n += 6;
            a = a * 32 + ((w / d21) & (d5 - 1));
            /*1xxxxx*/
        }
    }
    w = read_bit_string(n, LINK);
    return a;
} /*address_decoding*/

Local int read_mask(struct LOC_program_loader *LINK)
{
    /*RN*/
    int Result = -1;
    char c;

    if (bitstock < d26) /*code starts with 0*/
        c = read_bit_string(2, LINK);
    /*0x*/
    else {
        if (bitstock < d26 + d25) /*01*/
            c = read_bit_string(3, LINK) - 2;
        /*10x*/
        else {
            c = read_bit_string(6, LINK) - 44;
            /*11xxxx*/
        }
    }
    switch (c) {
    case 0: /*0,   2S 0   A  */
        Result = 656;
        break;

    case 1: /*3,   2B 0   A  */
        Result = 14480;
        break;

    case 2: /*2,   2T 0 X0   */
        Result = 10880;
        break;

    case 3: /*0,   2B 0   A  */
        Result = 2192;
        break;

    case 4: /*0,   2A 0   A  */
        Result = 144;
        break;

    case 5: /*2,   2B 0 X0   */
        Result = 10368;
        break;

    case 6: /*1,   2T 0   A  */
        Result = 6800;
        break;

    case 7: /*0,   0A 0 X0   */
        Result = 0;
        break;

    case 8: /*3,   0A 0   A  */
        Result = 12304;
        break;

    case 9: /*2, N 2T 0 X0   */
        Result = 10883;
        break;

    case 10: /*1,   2B 0   A  */
        Result = 6288;
        break;

    case 11: /*1,   0A 0 X0 B */
        Result = 4128;
        break;

    case 12: /*2,   2S 0 X0   */
        Result = 8832;
        break;

    case 13: /*0, Y 2A 0   A  */
        Result = 146;
        break;

    case 14: /*0,   4A 0 X0   */
        Result = 256;
        break;

    case 15: /*0, Y 2A 0 X0  P*/
        Result = 134;
        break;

    case 16: /*0, Y 6A 0   A  */
        Result = 402;
        break;

    case 17: /*1,   0A 0 X0 C */
        Result = 4144;
        break;

    case 18: /*0,   0A 0   A  */
        Result = 16;
        break;

    case 19:
        Result = address_decoding(LINK);
        break;
    }
    return Result;
} /*read_mask*/

Local int read_binary_word(struct LOC_program_loader *LINK)
{
    /*RF*/
    int w;
    char opc;

    if (bitstock < d26)         /*code starts with 0*/
    {                           /*OPC >= 8*/
        if (bitstock < d25) {   /*00*/
            if (bitstock < d24) /*000*/
                w = 4;          /*code is 000x*/
            else
                w = 5;                     /*code is 001xx*/
        } else if (bitstock < d25 + d24) { /*010*/
            if (bitstock < d25 + d23)      /*0100*/
                w = 6;                     /*0100xx*/
            else
                w = 7; /*0101xxx*/
        } else
            w = 10; /*011xxxxxxx*/
        w = read_bit_string(w, LINK);
        if (w < 2) /*000x*/
            return (opc_table[w]);
        /*no change*/
        if (w < 8) { /*001xx*/
            w -= 2;
            return (opc_table[w]);
        }
        if (w < 24) { /*010xx*/
            w -= 10;
            return (opc_table[w]);
        }
        if (w < 48) /*0101xxx*/
            w -= 30;
        else {
            w -= 366;
            /*011xxxxxxx*/
        }
        return (opc_table[w]);
    } /*0*/
    else {
        w   = read_bit_string(1, LINK);
        w   = read_mask(LINK);
        opc = w / d12;
        w   = (w & (d12 - 1)) * d15 + address_decoding(LINK);
        switch (opc) {
        case 0:
            /* blank case */
            break;

        case 1:
            w += LINK->list_address;
            break;

        case 2:
            if (((w / d17) & 1) == 1) /*d17 = 1*/
                w -= d17;
            else
                w += d19;
            w += Rstore(flib + (w & (d15 - 1))) - (w & (d15 - 1));
            break;

        case 3:
            if (klib == crfb)
                w += Rstore(mlib + (w & (d15 - 1))) - (w & (d15 - 1));
            else
                w += klib;
            break;
        }
        return w;
        /*1*/
    }
} /*read_binary_word*/

Local Void test_bit_stock()
{
    /*RH*/
    if (bitstock != d21 * 63) {
        if (verbose_tape)
            printf("Terminator: %09o\n", bitstock);
        stop(107, "Bad tape terminator, expecting 77");
    }
} /*test_bit_stock*/

Local Void typ_address(int a)
{
    /*RT*/
    printf("\n%2d %2d %2d", a / 1024, (a & 1023) / 32, a & 31);
} /*typ_address*/

Local Void read_list(struct LOC_program_loader *LINK)
{
    /*RL*/
    int i, j, w, FORLIM1;

    for (i = LINK->ll - 1; i >= 0; i--) {
        w = read_binary_word(LINK);
        if (LINK->list_address + i <= flib + flsc) { /*shift FLI downwards*/
            if (flib <= LINK->read_location)
                stop(98, "Store too small for the program");
            FORLIM1 = flsc;
            for (j = 0; j < FORLIM1; j++)
                Wstore(LINK->read_location + j, Rstore(flib + j));
            flib = LINK->read_location;
        }
        Wstore(LINK->list_address + i, w);
    } /*for i*/
    test_bit_stock();
} /*read_list*/

Local int read_crf_item(struct LOC_program_loader *LINK)
{
    /*RS*/
    int Result;

    if ((LINK->crfa & 1) == 0)
        Result = store[LINK->crfa / 2] / d13;
    else
        Result = store[LINK->crfa / 2] & (d13 - 1);
    LINK->crfa++;
    return Result;
} /*read_crf_item*/

Local void put_crf_item(int addr, int val)
{
    if ((addr & 1) == 0)
        store[addr/2] = val << 13;
    else
        store[addr/2] |= val;
}

Local void put_crf_entry(int * addr, ...)
{
    va_list ap;
    int val;
    va_start(ap, addr);
    do {
        val = va_arg(ap, int);
        put_crf_item((*addr)++, val);
    } while (val != 7680);
    va_end(ap);
}

void print_mcp_name(int mcp)
{
    printf("MCP %d: %s\n", mcp, mcp_names[mcp] ? mcp_names[mcp] : "internal, unnamed");
}

void list_undefined_references()
{
    int i;
    printf("\nUndefined external references:\n");
    for (i = 0; i < 128; ++i) {
        if (store[crfb+i] != 0) {
            print_mcp_name(i);
        }
    }
}

Static Void program_loader()
{ /*program loader*/
    /*RZ*/
    struct LOC_program_loader V;
    int i, j, idm;
    int mcp_count = 0;
    boolean use;
    int FORLIM;

    rlib   = (klie - rlsc - klsc) / 32 * 32;
    FORLIM = flsc;
    /*increment entries in future list:*/
    for (i = 0; i < FORLIM; i++)
        store[flib + i] += rlib;
    /*move KLI to final position:*/
    for (i = klsc - 1; i >= 0; i--)
        store[rlib + rlsc + i] = store[klib + i];
    klib = rlib + rlsc;
    /*prepare mcp-need analysis:*/
    mcpe = rlib;
    for (i = 0; i <= 127; i++)
        store[mlib + i] = 0;
    /*determine primary need of MCP's from name list:*/
    i = nlsc0;
    while (i > nlscop) {
        idm = store[nlib + i - 1];
        if ((store[nlib + i - 2] & (d3 - 1)) == 0)
            /*at most 4 letter/digit identifier*/
            i -= 2;
        else
            i -= 3;
        /*at least 5 letters or digits*/
        if (((idm / d15) & 1) == 0) { /*MCP is used*/
            mcp_count++;
            store[mlib + ((store[flib + (idm & (d15 - 1))] - rlib) & (d15 - 1))] =
                -(flib + (idm & (d15 - 1)));
        }
    }
    /*determine secondary need using the cross-reference list:*/
    V.crfa = crfb * 2;
    V.ll   = read_crf_item(&V);  /*for MCP length*/
    while (V.ll != 7680) {       /*end marker*/
        i   = read_crf_item(&V); /*for MCP number*/
        use = (store[mlib + i] != 0);
        j   = read_crf_item(&V); /*for number of MCP needing the current one*/
        while (j != 7680) {      /*end marker*/
            use = (use || store[mlib + j] != 0);
            j   = read_crf_item(&V);
        }
        if (use) {
            mcpe -= V.ll;
            if (mcpe <= mcpb)
                stop(25, "Corrupted cross-reference list?");
            if (store[mlib + i] < 0) /*primary need*/
                store[-store[mlib + i]] = mcpe;
            else
                mcp_count++;
            /*only secondary need*/
            store[mlib + i] = mcpe;
        }
        V.ll = read_crf_item(&V);
    }
    /*load result list RLI:*/
    V.ll            = rlsc;
    V.read_location = rnsb;
    prepare_read_bit_string1(&V);
    V.list_address = rlib;
    read_list(&V);
    if (store[rlib] != opc_table[89]) /*START*/
        stop(101, "Start OPC overwritten");
    typ_address(rlib);
    /*copy MLI:*/
    for (i = 0; i <= 127; i++)
        store[crfb + i] = store[mlib + i];
    klib = crfb;
    flsc = 0;
    /*load MCP's from store:*/
    prepare_read_bit_string2(&V);
    V.ll = read_bit_string(13, &V); /*for length or end marker*/
    while (V.ll < 7680) {
        i              = read_bit_string(13, &V); /*for MCP number*/
        V.list_address = store[crfb + i];
        if (verbose_tape) {
            printf("Seeing MCP %d, length %d in store: ", i, V.ll);
        }
        if (V.list_address != 0) {
            if (verbose_tape)
                printf("loading\n");
            read_list(&V);
            test_bit_stock();
            mcp_count--;
            store[crfb + i] = 0;
        } else {
            if (verbose_tape)
                printf("skipping\n");
            do {
                V.read_location--;
            } while (store[V.read_location] != d21 * 63);
        }
        prepare_read_bit_string2(&V);
        V.ll = read_bit_string(13, &V);
    }
    /*load MCP's from tape:*/
    while (mcp_count != 0) {
        if (P_argc > optind) {
            lib_tape = fopen(P_argv[optind++], "r");
            if (!lib_tape) {
                perror(P_argv[optind-1]);
                exit(EXIT_FAILURE);
            }
        } else if (!lib_tape || P_eof(lib_tape)) {
            list_undefined_references();
            exit(EXIT_FAILURE);
        }
        prepare_read_bit_string3(&V);
        V.ll = read_bit_string(13, &V); /*for length or end marker*/
        while (V.ll < 7680) {
            i              = read_bit_string(13, &V); /*for MCP number*/
            if (verbose_tape)
                printf("Reading MCP %d, length %d\n", i, V.ll);
            if (i > 50) {
                printf("Bad MCP number %d\n", i);
                exit(EXIT_FAILURE);
            }
            // TODO: compare with the proper length in crf
            if (V.ll == 0 || V.ll > 1024) {
                printf("Bad MCP length %d\n", V.ll);
                exit(EXIT_FAILURE);
            }
            V.list_address = Rstore(crfb + i);
            if (V.list_address != 0) {
                read_list(&V);
                test_bit_stock();
                mcp_count--;
                store[crfb + i] = 0;
            } else {
                if (verbose_tape) {
                    printf("Tape %s: skipping unneeded procedure %d\n",
                           P_argv[optind-1], i);
                }
                do {
                    do {
                        if (EOF == (V.ll = read_byte_from_lib_tape()))
                            break;
                    } while (V.ll != 0);
                    if (EOF == (V.ll = read_byte_from_lib_tape()))
                        break;
                } while (V.ll != 0);
            }
            prepare_read_bit_string3(&V);
            if (feof(lib_tape))
                break;          /* The tape has ended */
            V.ll = read_bit_string(13, &V);
        }
        fclose(lib_tape);
    }
    /*program loading completed:*/
    typ_address(mcpe);
} /*program_loader*/

int word1(const char * ss) {
    const unsigned char * s = (const unsigned char*)ss;
    switch (strlen(ss)) {
    case 1:
        return ascii_table[s[0]] << 21;
    case 2:
        return ascii_table[s[1]] << 21 |
               ascii_table[s[0]] << 15;
    case 3:
        return ascii_table[s[2]] << 21 |
               ascii_table[s[1]] << 15 |
               ascii_table[s[0]] << 9;
    case 4:
        return ascii_table[s[3]] << 21 |
               ascii_table[s[2]] << 15 |
               ascii_table[s[1]] << 9 |
               ascii_table[s[0]] << 3;
    default:
        return ascii_table[s[4]] << 21 |
               ascii_table[s[3]] << 15 |
               ascii_table[s[2]] << 9 |
               ascii_table[s[1]] << 3 |
               ascii_table[s[0]] >> 3;
    }
}

int word2(const char * ss) {
    const unsigned char * s = (const unsigned char*)ss;
    int c = ascii_table[s[0]] & 7;
    switch (strlen(ss)) {
    case 1: case 2: case 3: case 4:
        printf("Must not request word2 for short names\n");
        exit(1);
    case 5:
        return 0777777770 | c;
    case 6:
        return 0777777000 | c |
            ascii_table[s[5]] << 3;
    case 7:
        return 0777700000 | c |
            ascii_table[s[5]] << 3 |
            ascii_table[s[6]] << 9;
    case 8:
        return 0770000000 | c |
            ascii_table[s[5]] << 3 |
            ascii_table[s[6]] << 9 |
            ascii_table[s[7]] << 15;
    default:
        return c |
            ascii_table[s[5]] << 3 |
            ascii_table[s[6]] << 9 |
            ascii_table[s[7]] << 15 |
            ascii_table[s[8]] << 21;
    }
}

static int prefill_cnt = 0;

void prefill_op_proc(const char * name, int o) {
    int n;
    if (76 <= o && o <= 84)
        n = 57;
    else if (102 <= o && o <= 112)
        n = 40;
    else {
        printf("Bad OPC %d for op proc\n", o);
        exit(1);
    }
    if (strlen(name) >= 5)
        Wstore(nlib + prefill_cnt++, word2(name));
    Wstore(nlib + prefill_cnt++, word1(name));
    Wstore(nlib + prefill_cnt++, d18 + 12*256 + n + o);
}

void prefill_lib_proc(const char * name, int m) {
    mcp_names[m] = name;
    if (strlen(name) >= 5)
        Wstore(nlib + prefill_cnt++, word2(name));
    Wstore(nlib + prefill_cnt++, word1(name));
    Wstore(nlib + prefill_cnt++, d18 + d15 + m);
}

/*
 * main program
 */
int main(int argc, char *argv[])
{
    int FORLIM;

    const char * prog_name = strrchr(argv[0], '/');
    if (prog_name == NULL) {
        prog_name = argv[0];
    } else {
        prog_name++;
    }
    for (;;) {
        switch (getopt(argc, argv, "hbvf")) {
        case EOF:
            break;
        case 'h':
            printf("X1 Algol Compiler, converted from Pascal\n");
            printf("Usage:\n");
            printf("    %s [options...] [source [library]]\n", prog_name);
            printf("Input files:\n");
            printf("    source                  Algol-60 program source\n");
            printf("    library                 Library tape\n");
            printf("    (if no arguments are given, stdin is read as the source)\n");
            printf("Options:\n"
                   "    -h                      Display available options\n"
                   "    -v                      Verbose library tape actions\n"
                   "    -f                      Use 'passing of formals'\n"
                   "    -b                      The library tape is raw binary (default: text decimal)\n");
            exit(EXIT_SUCCESS);
        case 'b':
            binary_lib_tape = true;
            continue;
        case 'f':
            pass_formals = true;
            break;
        case 'v':
            verbose_tape = true;
            continue;
        }
        break;
    }
    if (argc == optind) {
        source_tape = stdin;
    } else {
        source_tape = fopen(argv[optind], "r");
        if (source_tape == NULL) {
            perror(argv[optind]);
            exit(EXIT_FAILURE);
        }
        ++optind;
    }

    PASCAL_MAIN(argc, argv);
    lib_tape = NULL;
    input_line = calloc(1, 1);

    /*initialization of word_del_table*/
    /*HT*/
    word_del_table[0]  = 15086;
    word_del_table[1]  = 43;
    word_del_table[2]  = 1;
    word_del_table[3]  = 86;
    word_del_table[4]  = 13353;
    word_del_table[5]  = 10517;
    word_del_table[6]  = 81;
    word_del_table[7]  = 10624;
    word_del_table[8]  = 44;
    word_del_table[9]  = 0;
    word_del_table[10] = 0;
    word_del_table[11] = 10866;
    word_del_table[12] = 0;
    word_del_table[13] = 0;
    word_del_table[14] = 106;
    word_del_table[15] = 112;
    word_del_table[16] = 0;
    word_del_table[17] = 14957;
    word_del_table[18] = 2;
    word_del_table[19] = 2;
    word_del_table[20] = 95;
    word_del_table[21] = 115;
    word_del_table[22] = 14304;
    word_del_table[23] = 0;
    word_del_table[24] = 0;
    word_del_table[25] = 0;
    word_del_table[26] = 0;
    word_del_table[27] = 0;
    word_del_table[28] = 107;

    /*initialization of ascii_table*/
    for (ii = 0; ii <= 127; ii++)
        ascii_table[ii] = -1;
    ascii_table['0']  = 0;
    ascii_table['1']  = 1;
    ascii_table['2']  = 2;
    ascii_table['3']  = 3;
    ascii_table['4']  = 4;
    ascii_table['5']  = 5;
    ascii_table['6']  = 6;
    ascii_table['7']  = 7;
    ascii_table['8']  = 8;
    ascii_table['9']  = 9;
    ascii_table['a']  = 10;
    ascii_table['b']  = 11;
    ascii_table['c']  = 12;
    ascii_table['d']  = 13;
    ascii_table['e']  = 14;
    ascii_table['f']  = 15;
    ascii_table['g']  = 16;
    ascii_table['h']  = 17;
    ascii_table['i']  = 18;
    ascii_table['j']  = 19;
    ascii_table['k']  = 20;
    ascii_table['l']  = 21;
    ascii_table['m']  = 22;
    ascii_table['n']  = 23;
    ascii_table['o']  = 24;
    ascii_table['p']  = 25;
    ascii_table['q']  = 26;
    ascii_table['r']  = 27;
    ascii_table['s']  = 28;
    ascii_table['t']  = 29;
    ascii_table['u']  = 30;
    ascii_table['v']  = 31;
    ascii_table['w']  = 32;
    ascii_table['x']  = 33;
    ascii_table['y']  = 34;
    ascii_table['z']  = 35;
    ascii_table['A']  = 37;
    ascii_table['B']  = 38;
    ascii_table['C']  = 39;
    ascii_table['D']  = 40;
    ascii_table['E']  = 41;
    ascii_table['F']  = 42;
    ascii_table['G']  = 43;
    ascii_table['H']  = 44;
    ascii_table['I']  = 45;
    ascii_table['J']  = 46;
    ascii_table['K']  = 47;
    ascii_table['L']  = 48;
    ascii_table['M']  = 49;
    ascii_table['N']  = 50;
    ascii_table['O']  = 51;
    ascii_table['P']  = 52;
    ascii_table['Q']  = 53;
    ascii_table['R']  = 54;
    ascii_table['S']  = 55;
    ascii_table['T']  = 56;
    ascii_table['U']  = 57;
    ascii_table['V']  = 58;
    ascii_table['W']  = 59;
    ascii_table['X']  = 60;
    ascii_table['Y']  = 61;
    ascii_table['Z']  = 62;
    ascii_table['+']  = 64;
    ascii_table['-']  = 65;
    ascii_table['*']  = 66; /*also ×*/
    ascii_table['/']  = 67;
    ascii_table['>']  = 70;
    ascii_table['=']  = 72;
    ascii_table['<']  = 74;
    ascii_table[',']  = 87;
    ascii_table['.']  = 88;
    ascii_table['@']  = 89;
    ascii_table[';']  = 91;
    ascii_table['(']  = 98;
    ascii_table[')']  = 99;
    ascii_table['[']  = 100;
    ascii_table[']']  = 101;
    ascii_table['`']  = 102;
    ascii_table['\''] = 103; /*'*/
    ascii_table[' ']  = 119;
    ascii_table[9]    = 118; /*tab*/
    ascii_table[10]   = 119; /*newline*/
    ascii_table['"']  = 121;
    ascii_table['?']  = 122;
    ascii_table[' ']  = 123; /*space*/
    ascii_table[':']  = 124;
    ascii_table['|']  = 162;
    ascii_table['_']  = 163;

    for (ii = 0; ii < 128; ++ii)
        if (ascii_table[ii] >= 0 && ascii_table[ii] < 64)
            alnum_table[ascii_table[ii]] = ii;

    /*preparation of prescan*/
    /*LE*/
    rns_state = virginal;
    scan      = 1;
    read_until_next_delimiter();

    prescan(); /*HK*/

    /*writeln;
    for bn:= plib to plie do writeln(bn:5,store[bn]:10);
    writeln;*/

    /*preparation of main scan:*/
    /*HL*/
    rns_state = virginal;
    scan      = -1;
    iflag     = 0;
    mflag     = 0;
    vflag     = 0;
    bn        = 0;
    aflag     = 0;
    sflag     = 0;
    eflag     = 0;
    rlsc      = 0;
    flsc      = 0;
    klsc      = 0;
    vlam      = 0;
    flib      = rnsb + 1;
    klib      = flib + 16;
    nlib      = klib + 16;
    if (nlib + nlsc0 >= plib)
        stop(25, "Too many pre-fills desired");
    nlsc = nlsc0;
    tlsc = tlib;
    gvc  = gvc0;
    fill_t_list(161);
    /*prefill of name list:*/
    prefill_op_proc("read", 102);
    prefill_op_proc("print", 103);
    prefill_op_proc("TAB", 104);
    prefill_op_proc("NLCR", 105);
    prefill_op_proc("XEEN", 106);
    prefill_op_proc("SPACE", 107);
    prefill_op_proc("stop", 108);
    prefill_op_proc("abs", 76);
    prefill_op_proc("sign", 77);
    prefill_op_proc("sqrt", 78);
    prefill_op_proc("sin", 79);
    prefill_op_proc("cos", 80);
    prefill_op_proc("arctan", 81);    // using OPC again for convenience
    prefill_op_proc("ln", 82);
    prefill_op_proc("exp", 83);
    prefill_op_proc("entier", 84);
    prefill_op_proc("FLOT", 110);     // using OPC for FLOT, FIXT, ABSFIXT
    prefill_op_proc("FIXT", 111);     // temporarily for convenience
    prefill_op_proc("ABSFIXT", 112);

    if (prefill_cnt != nlscop) {
        printf("Internal error: nlscop = %d, should be %d\n", nlscop, prefill_cnt);
        exit(EXIT_FAILURE);
    }

    prefill_lib_proc("SUM", 0);
    prefill_lib_proc("PRINTTEXT", 1);
    prefill_lib_proc("EVEN", 2);
    prefill_lib_proc("RESYM", 7);
    prefill_lib_proc("RANDOM", 17);
    prefill_lib_proc("SETRANDOM", 18);
    prefill_lib_proc("TIMEOFDAY", 77);
    // prefill_lib_proc("arctan", 3);
    // prefill_lib_proc("FLOT", 5);
    // prefill_lib_proc("FIXT", 6);
    // prefill_lib_proc("ABSFIXT", 24);

    if (prefill_cnt != nlsc0) {
        printf("Internal error: nlsc0 = %d, should be %d\n", nlsc0, prefill_cnt);
        exit(EXIT_FAILURE);
    }

    intro_new_block2();
    bitcount = 0;
    bitstock = 0;
    rnsb     = bim; /*START*/
    fill_result_list(96, 0);
    pos_ = 0;
    main_scan(); /*EL*/
    /*STOP*/
    fill_result_list(97, 0);

    /*writeln; writeln('FLI:');
    for bn:= 0 to flsc-1 do
    writeln(bn:5,store[flib+bn]:10);*/
    /*writeln; writeln('KLI:');
    for bn:= 0 to klsc-1 do
    writeln(bn:5,store[klib+bn]:10,
           (store[klib+bn] mod 134217728) div 16777216 : 10,
                       (store[klib+bn] mod 16777216) div  2097152 : 2,
                       (store[klib+bn] mod  2097152) div   524288 : 3,
                       (store[klib+bn] mod   524288) div   131072 : 2,
                       (store[klib+bn] mod   131072) div    32768 : 2,
                       (store[klib+bn] mod    32768) div     1024 : 4,
                       (store[klib+bn] mod     1024) div       32 : 3,
                       (store[klib+bn] mod       32) div        1 : 3);*/

    /*preparation of program loader*/
    opc_table[0]  = 33;
    opc_table[1]  = 34;
    opc_table[2]  = 16;
    opc_table[3]  = 56;
    opc_table[4]  = 58;
    opc_table[5]  = 85;
    opc_table[6]  = 9;
    opc_table[7]  = 14;
    opc_table[8]  = 18;
    opc_table[9]  = 30;
    opc_table[10] = 13;
    opc_table[11] = 17;
    opc_table[12] = 19;
    opc_table[13] = 20;
    opc_table[14] = 31;
    opc_table[15] = 35;
    opc_table[16] = 39;
    opc_table[17] = 61;
    opc_table[18] = 8;
    opc_table[19] = 10;
    opc_table[20] = 11;
    opc_table[21] = 12;
    opc_table[22] = 15;
    for (ii = 23; ii <= 31; ii++)
        opc_table[ii] = ii - 2;
    opc_table[32] = 32;
    opc_table[33] = 36;
    opc_table[34] = 37;
    opc_table[35] = 38;
    for (ii = 36; ii <= 51; ii++)
        opc_table[ii] = ii + 4;
    opc_table[52] = 57;
    opc_table[53] = 59;
    opc_table[54] = 60;
    for (ii = 55; ii <= 105; ii++)
        opc_table[ii] = ii + 7;

    ii = crfb*2;
    /* Length of MCP0 is 54, not needed by any other MCP, etc. */
    put_crf_entry(&ii, 54, 0, 7680); /* SUM */
    put_crf_entry(&ii, 20, 1, 7680); /* PRINTTEXT */
    put_crf_entry(&ii, 14, 2, 7680); /* EVEN */
    put_crf_entry(&ii, 7, 7, 7680);  /* RESYM */
    put_crf_entry(&ii, 7, 17, 7680); /* RANDOM */
    put_crf_entry(&ii, 7, 18, 7680); /* SETRANDOM */
    put_crf_entry(&ii, 7, 77, 7680); /* TIMEOFDAY */
    // put_crf_entry(&ii, 63, 3, 7680); /* arctan - back to OPC*/
    // put_crf_entry(&ii, 15, 4, 3, 7680); /* MCP4 is needed by MCP3 */
    // put_crf_entry(&ii, 100, 5, 7680);   /* FLOT - now an OPC */
    // put_crf_entry(&ii, 134, 6, 24, 7680); /* MCP6 (FIXT) is needed by MCP24 - now an OPC */
    // put_crf_entry(&ii, 21, 24, 7680);     /* ABSFIXT - now an OPC */
    put_crf_entry(&ii, 7680);
    put_crf_entry(&ii, 7680);

    store[mcpb]     = d21 * 63; /* MCP list terminator */

    /* output of objfile -w 2 library/even.src */
    store[mcpb + MCP_EVEN_BASE + 1] = 0770000000;
    store[mcpb + MCP_EVEN_BASE + 2] = 0070700640;
    store[mcpb + MCP_EVEN_BASE + 3] = 0777777266;
    store[mcpb + MCP_EVEN_BASE + 4] = 0203764750;
    store[mcpb + MCP_EVEN_BASE + 5] = 0770734740;
    store[mcpb + MCP_EVEN_BASE + 6] = 0623777763;
    store[mcpb + MCP_EVEN_BASE + 7] = 0353003763;
    store[mcpb + MCP_EVEN_BASE + 8] = 0362037707;
    store[mcpb + MCP_EVEN_BASE + 9] = 0477515460;
    store[mcpb + MCP_EVEN_BASE + 10] = 0554077777;
    store[mcpb + MCP_EVEN_BASE + 11] = 0766420376;
    store[mcpb + MCP_EVEN_BASE + 12] = 0000114066;
    store[mcpb + MCP_EVEN_BASE + 13] = 0000020016;

    /* output of objfile -w 77 library/timeofday.src */
    store[mcpb + MCP_TIMEOFDAY_BASE + 1] = 0770000000;
    store[mcpb + MCP_TIMEOFDAY_BASE + 2] = 0772700640;
    store[mcpb + MCP_TIMEOFDAY_BASE + 3] = 0030733777;
    store[mcpb + MCP_TIMEOFDAY_BASE + 4] = 0015575504;
    store[mcpb + MCP_TIMEOFDAY_BASE + 5] = 0001601153;
    store[mcpb + MCP_TIMEOFDAY_BASE + 6] = 0000000004;

    /* output of objfile -w 1 library/printtext.src */
    store[mcpb + 1] = 0770000000;
    store[mcpb + 2] = 0146700640;
    store[mcpb + 3] = 0777046401;
    store[mcpb + 4] = 0742360022;
    store[mcpb + 5] = 0544011177;
    store[mcpb + 6] = 0001017742;
    store[mcpb + 7] = 0600377553;
    store[mcpb + 8] = 0753100101;
    store[mcpb + 9] = 0736200427;
    store[mcpb + 10] = 0630117777;
    store[mcpb + 11] = 0477777607;
    store[mcpb + 12] = 0777036162;
    store[mcpb + 13] = 0633231031;
    store[mcpb + 14] = 0010471277;
    store[mcpb + 15] = 0224376644;
    store[mcpb + 16] = 0377077500;
    store[mcpb + 17] = 0633422024;
    store[mcpb + 18] = 0320044777;
    store[mcpb + 19] = 0060017742;
    store[mcpb + 20] = 0620447761;
    store[mcpb + 21] = 0400005407;
    store[mcpb + 22] = 0000002002;

    store[bim - 1] = d21 * 63;  /* Program code terminator */

    program_loader();

    printf("\n\n\n");
    FORLIM = rlib + rlsc + klsc;
    for (ii = mcpe; ii < FORLIM; ii++)
        printf("%5d%10d\n", ii, store[ii]);
    free(input_line);
    exit(EXIT_SUCCESS);
}

/* End. */
