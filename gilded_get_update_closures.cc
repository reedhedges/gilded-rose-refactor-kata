

#include "Item.hh"
#include "ItemTypes.hh"
#include "ItemUpdateRules.hh"


#include <functional>
using UpdateFunctionType = std::function<void(void)>;

// Upside: simple separated logic, no class machinery. String comparisons to check name are hoisted into 
// getUpdateFunction() function rather than being in update functions, which actually do no tests at all,
// improvement over TESTGUF above because the closure contains the correct item reference.
// Downsides: calling via std::function objects with captured value as argument is slow.
// Also closure stores an extra reference (pointer), if memory cost matters in some application.
// TODO is there a way to return a generic lamda object instead?
// This one benchmarks the slowest.


UpdateFunctionType getUpdateClosure(Item& item) 
{
    // TODO instead of checking each string pattern here, we could keep a 
    // static table of patterns mapping to lambda.
    if (item.name.starts_with("Backstage pass"))
        return [& item](){ updateTimeLimitedItem(item); };
    else if (item.name.starts_with("Aged"))
        return [& item](){ updateAgesWellItem(item); };
    else if (item.name.starts_with("Sulfuras"))
        return [& item](){ updateNeverDegradesItem(item); };
    else if (item.name.starts_with("Conjured"))
        return [& item](){ updateDegradesFastItem(item); };
    else
        return [& item](){ updateNormalItem(item); };
}

int main() 
{

    // using getUpdateClosure() to get a std::function or similar. These can be stored for later and used without rematching strings. Advantage over previous is that we don't need to call the right function with the right item object, just call the function which has stored a reference to the item object.
    // downside: slow.
    Item itemA{"itemA"};
    Item itemB{"itemB"};
    Item someItem{"something"};
    auto f = getUpdateClosure(itemA);
    auto f2 = getUpdateClosure(itemB);
    auto f3 = getUpdateClosure(someItem);
    f();
    f2();
    f3();

    return 0;
}

