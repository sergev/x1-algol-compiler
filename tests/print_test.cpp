#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, procedure_PRINTTEXT)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ prline(s); s̲t̲r̲i̲n̲g̲ s;
            b̲e̲g̲i̲n̲
                PRINTTEXT(s); NLCR
            e̲n̲d̲;
            prline(`Hello `Algol'');
            prline(``0123456789'');
            prline(``abcdefghijklmnopqrstuvwxyz'');
            prline(``ABCDEFGHIJKLMNOPQRSTUVWXYZ'');
            prline(``+-×/÷↑>≥=≤<≠¬∧∨⊃≡,.⏨:;:= '');
            prline(``()[]`''');
        e̲n̲d̲
    )");
    const std::string expect = R"(Hello `Algol'
`0123456789'
`abcdefghijklmnopqrstuvwxyz'
`ABCDEFGHIJKLMNOPQRSTUVWXYZ'
`+-×/÷↑>≥=≤<≠¬∧∨⊃≡,.⏨:;:= '
`()[]`''
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, procedure_FIXT)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ pr(n, m, x); v̲a̲l̲u̲e̲ n, m, x; i̲n̲t̲e̲g̲e̲r̲ n, m; r̲e̲a̲l̲ x; b̲e̲g̲i̲n̲
                PRINTTEXT(`/');
                FIXT(n, m, x);
                PRINTTEXT(`/');
                NLCR;
            e̲n̲d̲;
            pr(1, 0, 1.234);
            pr(4, 0, -56.789);
            pr(21, 0, 123.456);
            pr(1, 1, -1.234);
            pr(4, 1, 56.789);
            pr(20, 1, -123.456);
            pr(1, 20, 8 / 7);
            pr(4, 17, 9999 / 7);
            pr(0, 21, -1 / 7);
            pr(2, 0, 0.0);
            pr(3, 0, -0.0);
        e̲n̲d̲
    )");
    const std::string expect = R"(/+1 /
/-  57 /
/+                  123 /
/-1.2 /
/+  56.8 /
/-                 123.5 /
/+1.14285714285688300151 /
/+1428.42857142910361289 /
/-.142857142857110375189 /
/+ 0 /
/-  0 /
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, procedure_ABSFIXT)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ pr(n, m, x); v̲a̲l̲u̲e̲ n, m, x; i̲n̲t̲e̲g̲e̲r̲ n, m; r̲e̲a̲l̲ x; b̲e̲g̲i̲n̲
                PRINTTEXT(`/');
                ABSFIXT(n, m, x);
                PRINTTEXT(`/');
                NLCR;
            e̲n̲d̲;
            pr(1, 0, 1.234);
            pr(4, 0, -56.789);
            pr(21, 0, 123.456);
            pr(1, 1, -1.234);
            pr(4, 1, 56.789);
            pr(20, 1, -123.456);
            pr(1, 20, 8 / 7);
            pr(4, 17, 9999 / 7);
            pr(0, 21, -1 / 7);
            pr(2, 0, 0.0);
            pr(3, 0, -0.0);
        e̲n̲d̲
    )");
    const std::string expect = R"(/ 1 /
/   57 /
/                   123 /
/ 1.2 /
/   56.8 /
/                  123.5 /
/ 1.14285714285688300151 /
/ 1428.42857142910361289 /
/ .142857142857110375189 /
/  0 /
/   0 /
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, procedure_FLOT)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ pr(n, m, x); v̲a̲l̲u̲e̲ n, m, x; i̲n̲t̲e̲g̲e̲r̲ n, m; r̲e̲a̲l̲ x; b̲e̲g̲i̲n̲
                PRINTTEXT(`/');
                FLOT(n, m, x);
                PRINTTEXT(`/');
                NLCR;
            e̲n̲d̲;
            pr(1, 1, 1.234);
            pr(4, 1, -56.789);
            pr(13, 1, 123.456);
            pr(4, 2, -1.234);
            pr(5, 2, 56.789);
            pr(13, 2, -123.456);
            pr(4, 3, 8⏨555 / 7);
            pr(5, 3, 9999⏨444 / 7);
            pr(13, 3, -1⏨-111 / 7);
            pr(4, 1, 9.999);
            pr(4, 1, 9.9999);
            pr(1, 1, 0.0);
            pr(3, 1, -0.0);
        e̲n̲d̲
    )");
    const std::string expect = R"(/+.1⏨+1 /
/-.5679⏨+2 /
/+.1234560000000⏨+3 /
/-.1234⏨+ 1 /
/+.56789⏨+ 2 /
/-.1234560000000⏨+ 3 /
/+.1143⏨+556 /
/+.14284⏨+448 /
/-.1428571428571⏨-111 /
/+.9999⏨+1 /
/+.1000⏨+2 /
/+.0⏨+0 /
/-.000⏨+0 /
)";
    EXPECT_EQ(output, expect);
}
