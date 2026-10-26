#ifndef UPDATE_AGENTS_H
#define UPDATE_AGENTS_H

#include <functional>

/**
 * @brief Manages agents that perform actions on a schedule
 */
namespace UpdateAgents
{
    /**
     * @brief Add a new agent to the list
     *
     * @param p_agentName A for referncing the agent
     * @param p_delay A time in milliseconds to delay between calls to callback
     * @param p_callback A callback function that is called when `pollAgents` is called,
     * as long as `p_delay` milliseonds have passed since the last call.
     *
     */
    void addAgent(const char *p_agentName, const unsigned long p_delay, std::function<void()> p_callback);

    /**
     * @brief Calls the callback for each agent that hasn't called their callback for a time longer
     * than their set delay.
     *
     * @param currentMillis The current time since the device woke up
     */
    void pollAgents(unsigned long currentMillis);

    /**
     * @brief Removes agents from polling list
     */
    void clearAllAgents();
}

#endif