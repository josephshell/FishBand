#include <unity.h>
#include <time_provider/TimeProvider.h>
#include <sleep/IdleTimer.h>

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

constexpr long IDLE_TIMEOUT = 30;

TestTimeProvider tp = TestTimeProvider();
IdleTimer idleTimer = IdleTimer(IDLE_TIMEOUT, &tp);

void setUp(void)
{
    tp = TestTimeProvider();
    idleTimer = IdleTimer(IDLE_TIMEOUT, &tp);
}

void tearDown(void) {}

void user_is_inactive_at_idle_timeout()
{
    tp.changeTimeForTests(NOON_THURSDAY);
    idleTimer.noteActivity();
    tp.changeTimeForTests(NOON_THURSDAY + IDLE_TIMEOUT);
    TEST_ASSERT_TRUE(idleTimer.isUserIdle());
}

void user_is_not_inactive_before_idle_timeout()
{
    tp.changeTimeForTests(NOON_THURSDAY);
    idleTimer.noteActivity();
    tp.changeTimeForTests(NOON_THURSDAY + IDLE_TIMEOUT - 1);
    TEST_ASSERT_FALSE(idleTimer.isUserIdle());
}

void first_idle_check_notes_current_time_as_activity()
{
    tp.changeTimeForTests(NOON_THURSDAY);
    TEST_ASSERT_FALSE(idleTimer.isUserIdle());
    tp.changeTimeForTests(NOON_THURSDAY + IDLE_TIMEOUT - 1);
    TEST_ASSERT_FALSE(idleTimer.isUserIdle());
    tp.changeTimeForTests(NOON_THURSDAY + IDLE_TIMEOUT);
    TEST_ASSERT_TRUE(idleTimer.isUserIdle());
}

void user_is_not_inactive_when_activity_is_noted_before_timeout()
{
    tp.changeTimeForTests(NOON_THURSDAY);
    idleTimer.noteActivity();
    tp.changeTimeForTests(NOON_THURSDAY + IDLE_TIMEOUT - 1);
    TEST_ASSERT_FALSE(idleTimer.isUserIdle());
    idleTimer.noteActivity();
    tp.changeTimeForTests(NOON_THURSDAY + IDLE_TIMEOUT);
    TEST_ASSERT_FALSE(idleTimer.isUserIdle());
    tp.changeTimeForTests(NOON_THURSDAY + IDLE_TIMEOUT + IDLE_TIMEOUT);
    TEST_ASSERT_TRUE(idleTimer.isUserIdle());
}

void users_is_never_inactive_when_time_provider_is_null()
{
    idleTimer = IdleTimer(IDLE_TIMEOUT, nullptr);
    TEST_ASSERT_FALSE(idleTimer.isUserIdle());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(user_is_inactive_at_idle_timeout);
    RUN_TEST(user_is_not_inactive_before_idle_timeout);
    RUN_TEST(first_idle_check_notes_current_time_as_activity);
    RUN_TEST(user_is_not_inactive_when_activity_is_noted_before_timeout);
    RUN_TEST(users_is_never_inactive_when_time_provider_is_null);

    UNITY_END();
}