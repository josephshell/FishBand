#include <unity.h>
#include <time_provider/TimeProvider.h>
#include <alarms/AlarmManager.h>

class TestTimeProvider : public TimeProvider
{
    private:
    time_t currentTime = 0;
    
    public:
    void initialize() override {}

    time_t getCurrentTime() override
    {
        return currentTime;
    }

    void setCurrentTime(time_t newTime) override
    {
        currentTime = newTime;
    }

    void changeTimeForTests(time_t newTime)
    {
        currentTime = newTime;
    }
};

void setup(void) {}

void tearDown(void) {}

void test1()
{
    TestTimeProvider tp = TestTimeProvider();
    tp.changeTimeForTests(1000);
    AlarmManager manager = AlarmManager(&tp);
    
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test1);

    UNITY_END();
}