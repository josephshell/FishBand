#include <unity.h>
#include <async/AsyncDuration.h>

enum ExampleEnum
{
    VALUE_1,
    VALUE_2,
    VALUE_3,
    SENTINEL
};

void setup(void) {}

void tearDown(void) {}

void test_timer_expires()
{
    int durationMs = 1000;
    unsigned long startTimeMillis = 0;
    AsyncDuration timer;
    timer.start(durationMs, startTimeMillis);
    TEST_ASSERT_FALSE(timer.tick(500));
    TEST_ASSERT_TRUE(timer.tick(1000));
    TEST_ASSERT_TRUE(timer.tick(1500));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_timer_expires);

    UNITY_END();
}