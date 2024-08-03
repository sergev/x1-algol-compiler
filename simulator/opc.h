//
// The OPC table, from Appendix C.
//
enum {
    OPC_ETMR = 8,  //  extransmark result
    OPC_ETMP = 9,  //  extransmark procedure
    OPC_FTMR = 10, // formtransmark result
    OPC_FTMP = 11, // formtransmark procedure
    OPC_RET  = 12, // return
    OPC_EIS  = 13, // end of implicit subroutine

    OPC_TRAD = 14, // take real address dynamic
    OPC_TRAS = 15, // take real address static
    OPC_TIAD = 16, // take integer address dynamic
    OPC_TIAS = 17, // take integer address static
    OPC_TFA  = 18, // take formal address

    OPC_FOR0 = 19,
    OPC_FOR1 = 20,
    OPC_FOR2 = 21,
    OPC_FOR3 = 22,
    OPC_FOR4 = 23,

    OPC_FOR5 = 24,
    OPC_FOR6 = 25,
    OPC_FOR7 = 26,
    OPC_FOR8 = 27,

    OPC_GTA = 28, // goto adjustment
    OPC_SSI = 29, // store switch index
    OPC_CAC = 30, // copy boolean acc. into condition

    OPC_TRRD = 31, // take real result dynamic
    OPC_TRRS = 32, // take real result static
    OPC_TIRD = 33, // take integer result dynamic
    OPC_TIRS = 34, // take integer result static
    OPC_TFR  = 35, // take formal result

    OPC_ADRD = 36, // add real dynamic
    OPC_ADRS = 37, // add real static
    OPC_ADID = 38, // add integer dynamic
    OPC_ADIS = 39, // add integer static
    OPC_ADF  = 40, // add formal

    OPC_SURD = 41, // subtract real dynamic
    OPC_SURS = 42, // subtract real static
    OPC_SUID = 43, // subtract integer dynamic
    OPC_SUIS = 44, // subtract integer static
    OPC_SUF  = 45, // subtract formal

    OPC_MURD = 46, // multiply real dynamic
    OPC_MURS = 47, // multiply real static
    OPC_MUID = 48, // multiply integer dynamic
    OPC_MUIS = 49, // multiply integer
    OPC_MUF  = 50, // static multiply formal

    OPC_DIRD = 51, // divide real dynamic
    OPC_DIRS = 52, // divide real static
    OPC_DIID = 53, // divide integer dynamic
    OPC_DIIS = 54, // divide integer static
    OPC_DIF  = 55, // divide formal

    OPC_IND = 56, // indexer

    OPC_NEG = 57, // invert sign accumulator

    OPC_TAR = 58, // take result
    OPC_ADD = 59, // add
    OPC_SUB = 60, // subtract
    OPC_MUL = 61, // multiply
    OPC_DIV = 62, // divide
    OPC_IDI = 63, // integer division
    OPC_TTP = 64, // to the power

    OPC_MOR = 65, // more >
    OPC_LST = 66, // at least ≥
    OPC_EQU = 67, // equal =
    OPC_MST = 68, // at most ≤
    OPC_LES = 69, // less <
    OPC_UQU = 70, // unequal ̸=

    OPC_NON = 71, // non ¬
    OPC_AND = 72, // and ∧
    OPC_OR  = 73, // or ∨
    OPC_IMP = 74, // implies →
    OPC_QVL = 75, // equivalent ≡

    OPC_abs    = 76,
    OPC_sign   = 77,
    OPC_sqrt   = 78,
    OPC_sin    = 79,
    OPC_cos    = 80,
    OPC_ln     = 82,
    OPC_exp    = 83,
    OPC_entier = 84,

    OPC_ST   = 85, // store
    OPC_STA  = 86, // store also
    OPC_STP  = 87, // store procedure value
    OPC_STAP = 88, // store also procedure value

    OPC_SCC = 89, // short circuit
    OPC_RSF = 90, // real arrays storage function frame
    OPC_ISF = 91, // integer arrays storage function frame
    OPC_RVA = 92, // real value array storage function frame
    OPC_IVA = 93, // integer value array storage function frame
    OPC_LAP = 94, // local array positioning
    OPC_VAP = 95, // value array positioning

    OPC_START = 96, // start of the object program
    OPC_STOP  = 97, // end of the object program

    OPC_TFP     = 98,  // take formal parameter
    OPC_TAS     = 99,  // type algol symbol
    OPC_OBC6    = 100, // output buffer class 6
    OPC_FLOATER = 101,
    OPC_read    = 102,
    OPC_print   = 103,
    OPC_TAB     = 104,
    OPC_NLCR    = 105,
    OPC_XEEN    = 106,
    OPC_SPACE   = 107,
    OPC_stop    = 108,
    OPC_P21     = 109,
};
