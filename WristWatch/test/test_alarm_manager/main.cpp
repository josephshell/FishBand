#include <unity.h>
#include <time_provider/TimeProvider.h>
#include <alarms/AlarmManager.h>

// Thursday, November 19, 2026 at 12:00:00 PM UTC-05:00
const time_t NOON_THURSDAY = 1795107600;

// Seconds in an hour
const long ONE_HOUR = 3600;
// Seconds in a day
const long ONE_DAY = 86400;

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

void alarm_triggers_at_correct_time()
{
    TestTimeProvider tp = TestTimeProvider();
    AlarmManager manager = AlarmManager(&tp);
    Alarm noonAlarm = Alarm(12, 00, false, ONCE);

    tp.changeTimeForTests(NOON_THURSDAY - ONE_HOUR);
    manager.registerAlarm(noonAlarm);
    
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
    tp.changeTimeForTests(NOON_THURSDAY);
    TEST_ASSERT_FALSE(manager.alarmsMustTrigger().empty());
}

void everyday_alarms_trigger_everyday()
{
    TestTimeProvider tp = TestTimeProvider();
    AlarmManager manager = AlarmManager(&tp);
    Alarm noonAlarm = Alarm(12, 00, false, EVDY);

    tp.changeTimeForTests(NOON_THURSDAY - ONE_HOUR);
    manager.registerAlarm(noonAlarm);
    // assert alarm not triggered early
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
    // assert alarm triggers at alarm time
    tp.changeTimeForTests(NOON_THURSDAY);
    TEST_ASSERT_FALSE(manager.alarmsMustTrigger().empty());
    // assert that recalculating triggers causes alarm to disengage
    manager.recalculateTriggers();
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
    // assert that alarm doesn't trigger early the next day
    tp.changeTimeForTests(NOON_THURSDAY + ONE_DAY - ONE_HOUR);
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
    // assert that alarm triggers on time the next day
    tp.changeTimeForTests(NOON_THURSDAY + ONE_DAY);
    TEST_ASSERT_FALSE(manager.alarmsMustTrigger().empty());
}

void one_off_alarms_do_not_retrigger()
{
    TestTimeProvider tp = TestTimeProvider();
    AlarmManager manager = AlarmManager(&tp);
    Alarm noonAlarm = Alarm(12, 00, false, ONCE);

    tp.changeTimeForTests(NOON_THURSDAY - ONE_HOUR);
    manager.registerAlarm(noonAlarm);
    // assert alarm not triggered early
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
    // assert alarm triggers at alarm time
    tp.changeTimeForTests(NOON_THURSDAY);
    TEST_ASSERT_FALSE(manager.alarmsMustTrigger().empty());
    // assert that recalculating triggers causes alarm to disengage
    manager.recalculateTriggers();
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
    // assert that alarm doesn't trigger the next day
    tp.changeTimeForTests(NOON_THURSDAY + ONE_DAY);
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(alarm_triggers_at_correct_time);
    RUN_TEST(everyday_alarms_trigger_everyday);
    RUN_TEST(one_off_alarms_do_not_retrigger);

    UNITY_END();
}