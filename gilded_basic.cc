
#include "Item.hh"
#include "ItemTypes.hh"
#include "ItemUpdateRules.hh"


// Downside: checking item name strings all the time.
void updateItem(Item& item) 
{
    if (item.name.starts_with("Backstage pass")) {
        updateTimeLimitedItem(item);
    } else if (item.name.starts_with("Aged")) {
        updateAgesWellItem(item);
    } else if (item.name.starts_with("Sulfuras")){
        updateNeverDegradesItem(item);
    } else if (item.name.starts_with("Conjured")) {
        updateDegradesFastItem(item);
    } else {
        updateNormalItem(item);
    }
}

int main() 
{
    // Call updateItem() for each item, which just checks the name of the item and updates accordingly.
    // Checking the strings each time is slow and unneccesary, see next implementation.
    Item itemA{"itemA"};
    Item itemB{"itemB"};
    Item someItem{"something"};
    updateItem(itemA);
    updateItem(itemB);
    updateItem(someItem);
}

