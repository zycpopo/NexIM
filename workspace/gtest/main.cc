#include <gtest/gtest.h>

int add(int a, int b)
{
    return a + b;
}

TEST(DemoTest, addDemo)
{
    EXPECT_EQ(add(1, 1), 2);
    printf("测试用例成功执行了！\n");
    ASSERT_EQ(add(2, 4), 6);
    printf("测试用例执行失败了！\n");
}

int main(int argc, char *argv[])
{
    //单元测试框架的初始化
    testing::InitGoogleTest(&argc, argv);

    //开始单元测试
    return RUN_ALL_TESTS();
}