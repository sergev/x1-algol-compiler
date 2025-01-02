#include "fixture.h"

//
// Conversion from X1 to IEEE floating point format.
//
TEST(arith, x1_to_ieee)
{
    // clang-format off
    //                    4-sign             exponent
    //                     | ___mantissa_____  __
    //                     |/          x     \/  |
    EXPECT_EQ(x1_to_ieee(0'00'00'00000'00'00'00000ull), 0.0L);                 // 0.0
    EXPECT_EQ(x1_to_ieee(0'20'00'00000'00'00'04001ull), 1.0L);                 // 1.0
    EXPECT_EQ(x1_to_ieee(0'20'00'00000'00'00'14001ull), 0x8.000000001p-3L);    // epsilon = 1.0000000000018
#if __LDBL_MAX_EXP__ >= 2043
    EXPECT_EQ(x1_to_ieee(0'20'00'00000'00'00'00000ull), 0x8p-2052L);           // minreal = 1.547173023691⏨-617
    EXPECT_EQ(x1_to_ieee(0'37'77'77777'37'77'77777ull), 0xf.fffffffffp+2043L); // maxreal = 1.615850303564⏨616
#else
    FAIL() << "Exponent range of long double is too small";
#endif

    EXPECT_EQ(x1_to_ieee(0'77'77'77777'77'77'77777ull), -0.0L);                 // -0.0
    EXPECT_EQ(x1_to_ieee(0'57'77'77777'77'77'73776ull), -1.0L);                 // -1.0
    EXPECT_EQ(x1_to_ieee(0'57'77'77777'77'77'63776ull), -0x8.000000001p-3L);    // -epsilon
#if __LDBL_MAX_EXP__ >= 2043
    EXPECT_EQ(x1_to_ieee(0'57'77'77777'77'77'77777ull), -0x8p-2052L);           // -minreal
    EXPECT_EQ(x1_to_ieee(0'40'00'00000'40'00'00000ull), -0xf.fffffffffp+2043L); // -maxreal
#endif
    // clang-format on
}

//
// Conversion from IEEE to X1 floating point format.
//
TEST(arith, ieee_to_x1)
{
    // clang-format off
    //                                            4-sign             exponent
    //                                             | ___mantissa_____  __
    //                                             |/          x     \/  |
    EXPECT_EQ(ieee_to_x1(0.0L),                  0'00'00'00000'00'00'00000ull); // 0.0
    EXPECT_EQ(ieee_to_x1(1.0L),                  0'20'00'00000'00'00'04001ull); // 1.0
    EXPECT_EQ(ieee_to_x1(0x8.000000001p-3L),     0'20'00'00000'00'00'14001ull); // epsilon = 1.0000000000018
#if __LDBL_MAX_EXP__ >= 2043
    EXPECT_EQ(ieee_to_x1(0x8p-2052L),            0'20'00'00000'00'00'00000ull); // minreal = 1.547173023691⏨-617
    EXPECT_EQ(ieee_to_x1(0xf.fffffffffp+2043L),  0'37'77'77777'37'77'77777ull); // maxreal = 1.615850303564⏨616
#else
    FAIL() << "Exponent range of long double is too small";
#endif

    EXPECT_EQ(ieee_to_x1(-0.0L),                 0'77'77'77777'77'77'77777ull); // -0.0
    EXPECT_EQ(ieee_to_x1(-1.0L),                 0'57'77'77777'77'77'73776ull); // -1.0
    EXPECT_EQ(ieee_to_x1(-0x8.000000001p-3L),    0'57'77'77777'77'77'63776ull); // -epsilon
#if __LDBL_MAX_EXP__ >= 2043
    EXPECT_EQ(ieee_to_x1(-0x8p-2052L),           0'57'77'77777'77'77'77777ull); // -minreal
    EXPECT_EQ(ieee_to_x1(-0xf.fffffffffp+2043L), 0'40'00'00000'40'00'00000ull); // -maxreal
#endif
    // clang-format on
}

#include <cmath>

TEST(arith, log10l)
{
    // Positive numbers
    EXPECT_FLOAT_EQ(log10l(1e111L), 111.0L);
#if __LDBL_MAX_10_EXP__ >= 555
    EXPECT_FLOAT_EQ(log10l(1e444L), 444.0L);
    EXPECT_FLOAT_EQ(log10l(1e555L), 555.0L);
#else
    FAIL() << "Exponent range of long double is too small";
#endif
    EXPECT_FLOAT_EQ(log10l(1e-222L), -222.0L);
#if __LDBL_MIN_10_EXP__ <= -777
    EXPECT_FLOAT_EQ(log10l(1e-666L), -666.0L);
    EXPECT_FLOAT_EQ(log10l(1e-777L), -777.0L);
#endif

    // Zero
    // cppcheck-suppress invalidFunctionArg
    // cppcheck-suppress wrongmathcall
    EXPECT_FLOAT_EQ(log10l(0.0), - std::numeric_limits<long double>::infinity());

    // Negative numbers
    // cppcheck-suppress invalidFunctionArg
    // cppcheck-suppress wrongmathcall
    EXPECT_TRUE(std::isnan(log10l(-1.0L)));
#if __LDBL_MAX_10_EXP__ >= 888
    EXPECT_TRUE(std::isnan(log10l(-1e888L)));
#endif
#if __LDBL_MIN_10_EXP__ <= -999
    EXPECT_TRUE(std::isnan(log10l(-1e-999L)));
#endif
}
