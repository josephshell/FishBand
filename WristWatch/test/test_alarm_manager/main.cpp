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

// Thursday, November 19, 2026 at 12:00:00 PM UTC
const time_t NOON_THURSDAY = 1795089600;
// Sunday, November 15, 2026 at 12:00:00 PM UTC
const time_t NOON_SUNDAY = 1794744000;
// Seconds in an hour
const long ONE_HOUR = 3600;
// Seconds in a day
const long ONE_DAY = 86400;

TestTimeProvider tp = TestTimeProvider();
AlarmManager manager = AlarmManager(&tp);

void setUp(void) {
    tp = TestTimeProvider();
    manager = AlarmManager(&tp);
}

void tearDown(void) {}

void assertTriggersAtTime(time_t targetTime, bool recalculateTriggerAfter = true)
{
    tp.changeTimeForTests(targetTime);
    TEST_ASSERT_FALSE(manager.alarmsMustTrigger().empty());
    if (recalculateTriggerAfter)
        manager.recalculateTriggers();
}

void assertNotTriggersAtTime(time_t targetTime, bool recalculateTriggerAfter = true)
{
    tp.changeTimeForTests(targetTime);
    TEST_ASSERT_TRUE(manager.alarmsMustTrigger().empty());
    if (recalculateTriggerAfter)
        manager.recalculateTriggers();
}

void alarm_triggers_at_correct_time()
{
    tp.changeTimeForTests(NOON_THURSDAY - ONE_HOUR);
    Alarm alarm = Alarm(12, 00, false, ONCE);
    manager.registerAlarm(alarm);

    assertNotTriggersAtTime(NOON_THURSDAY - ONE_HOUR);
    assertTriggersAtTime(NOON_THURSDAY);
}

void everyday_alarms_trigger_everyday()
{
    tp.changeTimeForTests(NOON_THURSDAY - ONE_HOUR);
    Alarm alarm = Alarm(12, 00, false, EVDY);
    manager.registerAlarm(alarm);
    
    assertNotTriggersAtTime(NOON_THURSDAY - ONE_HOUR);           // alarm doesn't trigger early
    assertTriggersAtTime(NOON_THURSDAY);                         // alarm triggers on first day
    assertNotTriggersAtTime(NOON_THURSDAY);                      // alarm doesn't trigger twice on first day
    assertNotTriggersAtTime(NOON_THURSDAY + ONE_DAY - ONE_HOUR); // alarm doesn't trigger early the next day
    assertTriggersAtTime(NOON_THURSDAY + ONE_DAY);               // alarm triggers on time the next day
}

void one_off_alarms_do_not_retrigger()
{
    tp.changeTimeForTests(NOON_THURSDAY - ONE_HOUR);
    Alarm alarm = Alarm(12, 00, false, ONCE);
    manager.registerAlarm(alarm);

    assertNotTriggersAtTime(NOON_THURSDAY - ONE_HOUR); // alarm not triggered early
    assertTriggersAtTime(NOON_THURSDAY);               // alarm triggers at alarm time
    assertNotTriggersAtTime(NOON_THURSDAY + ONE_DAY);  // assert that alarm doesn't trigger the next day
}

void workday_alarms_trigger_on_workdays()
{
    tp.changeTimeForTests(NOON_SUNDAY - ONE_HOUR);
    Alarm alarm = Alarm(12, 00, false, WKDY);
    manager.registerAlarm(alarm);

    assertTriggersAtTime(NOON_SUNDAY);               // Sunday
    assertTriggersAtTime(NOON_SUNDAY + ONE_DAY * 1); // Monday
    assertTriggersAtTime(NOON_SUNDAY + ONE_DAY * 2); // Tuesday
    assertTriggersAtTime(NOON_SUNDAY + ONE_DAY * 3); // Wednesday
    assertTriggersAtTime(NOON_SUNDAY + ONE_DAY * 4); // Thursday
}

void workday_alarms_do_not_trigger_on_weekend_days()
{
    tp.changeTimeForTests(NOON_THURSDAY + ONE_HOUR);
    Alarm alarm = Alarm(12, 00, false, WKDY);
    manager.registerAlarm(alarm);

    assertNotTriggersAtTime(NOON_THURSDAY + ONE_DAY * 1);   // Friday
    assertNotTriggersAtTime(NOON_THURSDAY + ONE_DAY * 2);   // Saturday
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(alarm_triggers_at_correct_time);
    RUN_TEST(everyday_alarms_trigger_everyday);
    RUN_TEST(one_off_alarms_do_not_retrigger);
    RUN_TEST(workday_alarms_trigger_on_workdays);
    RUN_TEST(workday_alarms_do_not_trigger_on_weekend_days);

    UNITY_END();
}