#include <async/AsyncDuration.h>

void AsyncDuration::start(int durationMs, unsigned long currentTimeMillis)
{
    startTimeMillis = currentTimeMillis;
    this->durationMs = durationMs;
}

bool AsyncDuration::tick(unsigned long currentTimeMillis)
{
    return (currentTimeMillis - startTimeMillis) >= durationMs;
}