#ifndef VIBRATION_PATTERNS_H
#define VIBRATION_PATTERNS_H

class VibrationPattern
{
    public:
    virtual void setStartTime(unsigned long startTimeMillis) = 0;

    virtual bool isActive(unsigned long currentTimeMillis) = 0;

    ~VibrationPattern() = default;
};

class SteadyVibrationPattern : public VibrationPattern
{
    public:
    void setStartTime(unsigned long startTimeMillis) override
    {
        // No action needed for steady vibration
    }

    bool isActive(unsigned long currentTimeMillis) override
    {
        return true; // Always active
    }

};

#endif