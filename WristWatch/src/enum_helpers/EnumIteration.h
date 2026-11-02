#ifndef ENUM_ITERATION_H
#define ENUM_ITERATION_H

/**
 * @brief Increments an enum value, wrapping around to the first value if it reaches the sentinel. The sentinel value is treated as the end of the enum and is not a valid value to be assigned to the enum variable.
 * 
 * @tparam T The enum type.
 * @param value The current enum value to be incremented.
 * @param sentinel The sentinel value indicating the end of the enum.
 */
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

/**
 * @brief Decrements an enum value, wrapping around to the last value if it reaches the first value. The sentinel value is treated as the end of the enum and is not a valid value to be assigned to the enum variable.
 * 
 * @tparam T The enum type.
 * @param value The current enum value to be decremented.
 * @param sentinel The sentinel value indicating the end of the enum.
 */
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