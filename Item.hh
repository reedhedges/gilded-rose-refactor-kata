#pragma once

// Given type interface, cannot change:

#include <string>

struct Item {
    std::string name;
    int sellIn = 30; // Number of days left until sell-by date. Is decremented each update. May be negative if past sell-by date.  If at INT_MIN, it will no longer be decremented, and will stay at INT_MIN for all further updates.
    int quality = 50; // Quality value. Always in range [0, 50].
};




