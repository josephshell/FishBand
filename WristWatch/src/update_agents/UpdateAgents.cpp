#include <update_agents/UpdateAgents.h>
#include <functional>
#include <vector>

namespace
{
    /**
     * @brief Represents an agent that performs a callback regularly
     */
    struct Agent
    {
        const char *name;
        const unsigned long delay;
        std::function<void()> callback;
        unsigned long lastCall = 0;
        bool hasBeenPolled = false;

        Agent(const char *p_agentName, const unsigned long p_delay, std::function<void()> p_callback) : name(p_agentName), delay(p_delay), callback(p_callback) {}
    };

    std::vector<Agent> agents;

    bool shouldPollAgent(Agent agent, unsigned long currentMillis)
    {
        return (currentMillis - agent.lastCall >= agent.delay) || !agent.hasBeenPolled;
    }
}

namespace UpdateAgents
{

    void addAgent(const char *p_agentName, const unsigned long p_delay, std::function<void()> p_callback)
    {
        Agent agent = Agent(p_agentName, p_delay, p_callback);
        agents.push_back(agent);
    }

    void pollAgents(unsigned long currentMillis)
    {
        for (Agent &agent : agents)
        {

            if (shouldPollAgent(agent, currentMillis))
            {
                agent.hasBeenPolled = true;
                agent.lastCall = currentMillis;
                agent.callback();
            }
        }
    }

    void clearAllAgents()
    {
        agents.clear();
    }
}