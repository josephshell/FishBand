#include <unity.h>
#include <vibration_patterns/VibrationPattern.h>

void setup(void)
{
}

void tearDown(void)
{
}

void test_steady_vibration() {
    VibrationPattern *vib = new SteadyVibrationPattern();
    vib->setStartTime(0);
    TEST_ASSERT_TRUE(vib->isActive(0));
    TEST_ASSERT_TRUE(vib->isActive(1000));
    delete vib;
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_steady_vibration);

    UNITY_END();
}