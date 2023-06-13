#pragma once

#ifdef __STDC_VERSION_STDCKDINT_H__
#include <stdckdint.h>
#endif

namespace util {

void decrement_clamp(int& value, int amount, int min_value)
{
    if(value == min_value)
        return;
    int temp;
#ifdef __STDC_VERSION_STDCKDINT_H__
    if(ckd_sub(temp, value, amount))
        if(temp < min_value)
            value = min_value;
        else
            value = temp;
    else    
        value = min_value;
#else
    temp = value - amount;
    if(temp > value)
        value = min_value;
    else if(temp < min_value)
        value = min_value;
    else
        value = temp;
#endif
}

void increment_clamp(int& value, int amount, int max_value)
{
    if(value == max_value)
        return;
    int temp;
#ifdef __STDC_VERSION_STDCKDINT_H__
    if(ckd_add(temp, value, amount))
        if(temp > max_value)
            value = max_value;
        else
            value = temp;
    else    
        value = max_value;
#else
    temp = value + amount;
    if(temp < value)
        value = max_value;
    else if(temp > max_value)
        value = max_value;
    else
        value = temp;
#endif
}

}

