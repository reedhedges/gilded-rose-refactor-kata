#pragma once

// Given type interface, cannot change:

#include <string>

struct Item {
    std::string name;
    int sellIn = 30; // Number of days left until sell-by date. Is decremented each update. May be negative if past sell-by date.  If at INT_MIN, it will no longer be decremented, and will stay at INT_MIN for all further updates.
    int quality = 50; // Quality value. Always in range [0, 50].
};



// We add the concept of a type or category...

#include <string_view>

enum ItemType {
    Normal,
    AgesWell,           // e.g. "Aged Brie". Note sellIn not used.
    NeverDegrades,      // e.g. Legendary "Sulfuras"
    TimeLimited,        // e.g. "Backstage passes"
    DegradesFast        // e.g. "Conjured"
};

constexpr ItemType itemTypeFromName(std::string_view name)
{
    if (name.starts_with("Backstage pass"))
        return TimeLimited;
    else if (name.starts_with("Aged"))
        return AgesWell;
    else if (name.starts_with("Sulfuras"))
        return NeverDegrades;
    else if (name.starts_with("Conjured"))
        return DegradesFast;
    else   
        return Normal;
}


