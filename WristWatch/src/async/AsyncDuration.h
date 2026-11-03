#ifndef ASYNC_DURATION_H
#define ASYNC_DURATION_H

/**
 * @brief A class for creating asynchronous timers that don't block the main loop (much)
 */
class AsyncDuration
{
    private:
    int durationMs = 0;
    unsigned long startTimeMillis = 0;
    
    public:
    /**
     * @brief Starts a timer for [durationMs] milliseconds
     */
    void start(int durationMs, unsigned long currentTimeMillis);
    
    /**
     * @brief Returns true if the timer has expired, false otherwise
     */
    bool tick(unsigned long currentTimeMillis);
};

#endif