#include <unity.h>
#include <update_agents/UpdateAgents.h>

void setup(void)
{
}

void tearDown(void)
{
    UpdateAgents::clearAllAgents();
}

void test_agent_updates_on_first_poll()
{
    bool hasRan = false;
    UpdateAgents::addAgent("Test Agent 1", 1000, [&hasRan]()
                           { hasRan = true; });
    UpdateAgents::pollAgents(0);

    TEST_ASSERT_TRUE(hasRan);
}

void test_agent_not_updates_before_delay_time()
{
    bool hasRan = false;
    UpdateAgents::addAgent("Test Agent 2", 1000, [&hasRan]()
                           { hasRan = true; });
    UpdateAgents::pollAgents(0);
    hasRan = false;

    UpdateAgents::pollAgents(999);
    TEST_ASSERT_FALSE(hasRan);
}

void test_agent_updates_at_delay_time()
{
    bool hasRan = false;
    UpdateAgents::addAgent("Test Agent 2", 1000, [&hasRan]()
                           { hasRan = true; });
    UpdateAgents::pollAgents(0);
    hasRan = false;

    UpdateAgents::pollAgents(1000);
    TEST_ASSERT_TRUE(hasRan);
}

void test_agent_updates_after_delay_time_after_second_update()
{
    bool hasRan = false;
    UpdateAgents::addAgent("Test Agent 2", 1000, [&hasRan]()
                           { hasRan = true; });
    UpdateAgents::pollAgents(0);
    hasRan = false;

    UpdateAgents::pollAgents(1500);
    TEST_ASSERT_TRUE(hasRan);

    hasRan = false;
    UpdateAgents::pollAgents(2000);
    TEST_ASSERT_FALSE(hasRan);

    UpdateAgents::pollAgents(2500);
    TEST_ASSERT_TRUE(hasRan);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_agent_updates_on_first_poll);
    RUN_TEST(test_agent_not_updates_before_delay_time);
    RUN_TEST(test_agent_updates_at_delay_time);
    RUN_TEST(test_agent_updates_after_delay_time_after_second_update);

    UNITY_END();
}