
#include "Item.hh"
#include "ItemTypes.hh"
#include "ItemUpdateRules.hh"


#include <vector>
#include <tuple>

int main() {

    // Store with respective known type.
    // If we were instead given a bunch of Item objects with no types, we could use itemTypeFromName() to check
    // once and store in the vector.  Could also perhaps, given container of Item objects, create a derived range rather than a new vector.

    Item itemA{"itemA"};
    Item itemB{"itemB"}; 
    Item someItem{"something"};

    std::vector<std::tuple<Item&, ItemType>> items {
        {itemA, Normal},
        {itemB, Normal},
        {someItem, Normal}
    };

    // Now we just check ItemType value instead of matching strings. If this is done repeatedly then it's much faster.
    // Could also be std::for_each, etc. with std::execution::par etc. if desired.
    // Another opproach that may also be a further optimization given very many items would be to keep separate vectors or ranges for each type.

    for(auto t : items) 
    {
        Item& item = std::get<0>(t);
        ItemType type = std::get<1>(t);
        updateItemWithType(item, type);
    }
}
