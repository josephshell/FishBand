#include <unity.h>
#include <enum_helpers/EnumIteration.h>

enum ExampleEnum
{
    VALUE_1,
    VALUE_2,
    VALUE_3,
    SENTINEL
};

void setup(void) {}

void tearDown(void) {}

void test_increment()
{
    ExampleEnum cursor = VALUE_1;
    incrementEnum(cursor, SENTINEL);
    TEST_ASSERT_EQUAL(VALUE_2, cursor);
    incrementEnum(cursor, SENTINEL);
    TEST_ASSERT_EQUAL(VALUE_3, cursor);
}

void test_decrement()
{
    ExampleEnum cursor = VALUE_3;
    decrementEnum(cursor, SENTINEL);
    TEST_ASSERT_EQUAL(VALUE_2, cursor);
    decrementEnum(cursor, SENTINEL);
    TEST_ASSERT_EQUAL(VALUE_1, cursor);
}

void test_decrement_wrap()
{
    ExampleEnum cursor = VALUE_1;
    decrementEnum(cursor, SENTINEL);
    TEST_ASSERT_EQUAL(VALUE_3, cursor);
}

void test_increment_wrap()
{
    ExampleEnum cursor = VALUE_3;
    incrementEnum(cursor, SENTINEL);
    TEST_ASSERT_EQUAL(VALUE_1, cursor);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_increment);
    RUN_TEST(test_decrement);
    RUN_TEST(test_decrement_wrap);
    RUN_TEST(test_increment_wrap);

    UNITY_END();
}