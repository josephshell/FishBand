#include <unity.h>
#include <set_time/TimeSetter.h>

// Set initial time to a known value 9/15/2020 12:00:00 AM EST
time_t initialTime = 1600142400;

void setup(void)
{

}

void tearDown(void) {}

void test_change_hour()
{
    TimeSetter timeSetter;
    timeSetter.initialize(initialTime);

    timeSetter.incrementTimeSetting();

    tm* localTime = timeSetter.getLocalTime();
    TEST_ASSERT_EQUAL(1, localTime->tm_hour);
    timeSetter.decrementTimeSetting();
    TEST_ASSERT_EQUAL(0, localTime->tm_hour);
    for (int i = 0; i < 12; i++)
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(12, localTime->tm_hour);
    for (int i = 0; i < 12; i++)
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(0, localTime->tm_hour);
}

void test_change_minute()
{
    TimeSetter timeSetter;
    timeSetter.initialize(initialTime);
    timeSetter.incrementCursor(); // Move cursor to minute

    timeSetter.incrementTimeSetting();
    tm* localTime = timeSetter.getLocalTime();
    TEST_ASSERT_EQUAL(1, localTime->tm_min);
    timeSetter.decrementTimeSetting();
    timeSetter.decrementTimeSetting();
    TEST_ASSERT_EQUAL(59, localTime->tm_min);
    timeSetter.incrementTimeSetting();
    for (int i = 0; i < 30; i++)
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(30, localTime->tm_min);
    for (int i = 0; i < 30; i++)
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(0, localTime->tm_min);
}

void test_change_second()
{
    TimeSetter timeSetter;
    timeSetter.initialize(initialTime);
    timeSetter.incrementCursor(); // Move cursor to minute
    timeSetter.incrementCursor(); // Move cursor to second

    timeSetter.incrementTimeSetting();
    tm* localTime = timeSetter.getLocalTime();
    TEST_ASSERT_EQUAL(1, localTime->tm_sec);
    timeSetter.decrementTimeSetting();
    timeSetter.decrementTimeSetting();
    TEST_ASSERT_EQUAL(59, localTime->tm_sec);
    timeSetter.incrementTimeSetting();
    for (int i = 0; i < 30; i++)
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(30, localTime->tm_sec);
    for (int i = 0; i < 30; i++)
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(0, localTime->tm_sec);
}

void test_change_month()
{
    TimeSetter timeSetter;
    timeSetter.initialize(initialTime);
    timeSetter.incrementCursor(); // Move cursor to minute
    timeSetter.incrementCursor(); // Move cursor to second
    timeSetter.incrementCursor(); // Move cursor to month

    tm* localTime = timeSetter.getLocalTime();
    TEST_ASSERT_EQUAL(8, localTime->tm_mon); // September is month 8 because month is 0-indexed
    for (int i = 0; i < 4; i++)
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(0, localTime->tm_mon); // January is month 0
    timeSetter.decrementTimeSetting();
    TEST_ASSERT_EQUAL(11, localTime->tm_mon); // December is month 11
}

void test_change_day()
{
    TimeSetter timeSetter;
    timeSetter.initialize(initialTime);
    timeSetter.incrementCursor(); // Move cursor to minute
    timeSetter.incrementCursor(); // Move cursor to second
    timeSetter.incrementCursor(); // Move cursor to month
    timeSetter.incrementCursor(); // Move cursor to day

    tm* localTime = timeSetter.getLocalTime();
    TEST_ASSERT_EQUAL(15, localTime->tm_mday); // Day is 15
    for (int i = 0; i < 16; i++)               // Sepetmber has 30 days, so incrementing 16 times will advnance to October 1st
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(1, localTime->tm_mday); // Day is 1
    TEST_ASSERT_EQUAL(9, localTime->tm_mon); // Month is October, which is month 9 0-indexed
    timeSetter.decrementTimeSetting();
    TEST_ASSERT_EQUAL(30, localTime->tm_mday); // Day is 30
    TEST_ASSERT_EQUAL(8, localTime->tm_mon); // Month is September, which is month 8 0-indexed
}

void test_change_year()
{
    TimeSetter timeSetter;
    timeSetter.initialize(initialTime);
    timeSetter.incrementCursor(); // Move cursor to minute
    timeSetter.incrementCursor(); // Move cursor to second
    timeSetter.incrementCursor(); // Move cursor to month
    timeSetter.incrementCursor(); // Move cursor to day
    timeSetter.incrementCursor(); // Move cursor to year

    tm* localTime = timeSetter.getLocalTime();
    TEST_ASSERT_EQUAL(120, localTime->tm_year); // Year is 2020, but tm_year is years since 1900, so 2020 - 1900 = 120
    for (int i = 0; i < 5; i++)
    {
        timeSetter.incrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(125, localTime->tm_year); // Year is 2025
    for (int i = 0; i < 10; i++)
    {
        timeSetter.decrementTimeSetting();
    }
    TEST_ASSERT_EQUAL(115, localTime->tm_year); // Year is 2015
}

void test_cancel_and_done()
{
    TimeSetter timeSetter;
    timeSetter.initialize(initialTime);
    timeSetter.incrementCursor(); // Move cursor to minute
    timeSetter.incrementCursor(); // Move cursor to second
    timeSetter.incrementCursor(); // Move cursor to month
    timeSetter.incrementCursor(); // Move cursor to day
    timeSetter.incrementCursor(); // Move cursor to year
    timeSetter.incrementCursor(); // Move cursor to cancel

    int result = timeSetter.incrementTimeSetting();
    TEST_ASSERT_EQUAL(-1, result); // Cancel should return -1

    timeSetter.incrementCursor(); // Move cursor to done
    result = timeSetter.incrementTimeSetting();
    TEST_ASSERT_EQUAL(1, result); // Done should return 1
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_change_hour);
    RUN_TEST(test_change_minute);
    RUN_TEST(test_change_second);
    RUN_TEST(test_change_month);
    RUN_TEST(test_change_day);
    RUN_TEST(test_change_year);
    RUN_TEST(test_cancel_and_done);

    UNITY_END();
}