#pragma once

// We add the concept of a type or category...

#include "Item.hh"
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


