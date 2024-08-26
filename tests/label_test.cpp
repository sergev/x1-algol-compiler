#include <fstream>

#include "fixture.h"

//TODO: implement GTA
TEST_F(x1_machine, DISABLED_label_arg)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ jump(x); l̲a̲b̲e̲l̲ x;
                g̲o̲t̲o̲ x;

        print(123);
        jump(L);
        print(456);
    L:  print(789);
        e̲n̲d̲
    )");
    const std::string expect = R"(123
789
)";
    EXPECT_EQ(output, expect);
}
