

#include "Item.hh"
#include "ItemTypes.hh"
#include "ItemUpdateRules.hh"


// Upside: simple separated logic, no class machinery. String comparisons to check name are hoisted into 
// getUpdateFunction() function rather than being in update functions, which actually do no tests at all.
// Downsides: calling via std::function objects is slow (cf virtual classes below), can't be inlined,
// you must supply the item when calling the update function. (See next implementation.)
// TODO: is there a way to return a generic lamda object instead?

#include <functional>


//auto updateA = [](Item& item) { item.code = 'a'; };
//auto updateB = [](Item& item) { item.code = 'b'; };
//auto defaultUpdate = [](Item& item) { item.code = ' '; };

using UpdateFunctionType = std::function<void(Item&)>;

UpdateFunctionType getUpdateFunction(const Item& item) 
{
    // TODO instead of checking each string pattern here, we could keep a 
    // static table of patterns mapping to function.
    if (item.name.starts_with("Backstage pass"))
        return updateTimeLimitedItem;
    else if (item.name.starts_with("Aged"))
        return updateAgesWellItem;
    else if (item.name.starts_with("Sulfuras"))
        return updateNeverDegradesItem;
    else if (item.name.starts_with("Conjured"))
        return updateDegradesFastItem;
    else
        return updateNormalItem;
}


int main() {

    // using getUpdateFunction() to get a std::function or similar. These can be stored for later and used without rematching strings:
    // Downside: the function *must* be called with the correct item. (E.g. store as list of pairs or tuples.)
    Item itemA{"itemA"};
    Item itemB{"itemB"};
    Item someItem{"something"};
    auto f = getUpdateFunction(itemA);
    auto f2 = getUpdateFunction(itemB);
    auto f3 = getUpdateFunction(someItem);
    f2(itemB);
    f(itemA);
    f3(someItem);

    return 0;
}
