#ifndef ENUM_ITERATION_H
#define ENUM_ITERATION_H

template <typename T>
void incrementEnum(T &value, T sentinel)
{
    if (static_cast<int>(value) + 1 == sentinel)
    {
        value = static_cast<T>(0);
    }
    else
    {
        value = static_cast<T>(static_cast<int>(value) + 1);
    }
}

template <typename T>
void decrementEnum(T &value, T sentinel)
{
    if (static_cast<int>(value) == 0)
    {
        value = static_cast<T>(static_cast<int>(sentinel) - 1);
    }
    else
    {
        value = static_cast<T>(static_cast<int>(value) - 1);
    }
}

#endif