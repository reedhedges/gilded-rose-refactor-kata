
#include <cassert>
#include <cstdio>


#define TESTONEF 1
#define TESTONEFOPT 1
#define TESTGUF 1
#define TESTCLOSURE 1
//  #define TESTIU1 1
#define TESTIU2


#include "Item.hh"


// The different update rules for each type,
// and some utilities for updating values in the Item class since we can't add accessors:
// (If we wanted to, we could define a wrapper object around the given Item objects.) 

#include "util.hh"

void Item_increment_quality(Item& item, int amount)
{
  increment_clamp(item.quality, amount, 50);
}

void Item_decrement_quality(Item& item, int amount)
{
  decrement_clamp(item.quality, amount, 0);
}

void Item_decrement_sellIn(Item& item)
{
  decrement_clamp(item.sellIn, 1, INT_MIN);
}



void updateNormalItem(Item& item) {
    if(item.sellIn <= 0) {
        Item_decrement_quality(item, 2);
    } else {
        Item_decrement_quality(item, 1);
    }
    Item_decrement_sellIn(item);
}

void updateAgesWellItem(Item& item) {
    Item_increment_quality(item, 1);
    Item_decrement_sellIn(item);
    // TODO check original implementation's tests to see if the above is correct (does quality increase by two if it's past sellIn?)
}

void updateNeverDegradesItem(Item& item) {
    Item_decrement_sellIn(item);
}

void updateTimeLimitedItem(Item& item) {
    if(item.sellIn <= 0) {
        item.quality = 0;
    } else if(item.sellIn <= 5) {
        Item_decrement_quality(item, 3);
    } else if(item.sellIn <= 10) {
        Item_decrement_quality(item, 2);
    } else {
        Item_decrement_quality(item, 1);
    }
    Item_decrement_sellIn(item);
}

void updateDegradesFastItem(Item& item) {
    if(item.sellIn <= 0) {
        Item_decrement_quality(item, 4);
    } else {
        Item_decrement_quality(item, 2);
    }
    Item_decrement_sellIn(item);
}


// Given 'type' do the apropriate update to 'item'.
void updateItemWithType(Item& item, ItemType type)
{
  switch(type) 
  {
      case TimeLimited:
          updateTimeLimitedItem(item);
          break;
      case AgesWell:
          updateAgesWellItem(item);
          break;
      case DegradesFast:
          updateDegradesFastItem(item);
          break;
      case NeverDegrades:
          updateNeverDegradesItem(item);
          break;
      case Normal:
      default:
          updateNormalItem(item);
  }
}





// -------------

// Different structural implementations:

// -------------

#ifdef TESTONEF

// Downside: checking item name strings all the time.
void updateItem(Item& item) {
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
#endif

// -------------


// TESTONEFOPT, see main() below.

// ----------

#ifdef TESTGUF

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

#endif


// -------------

#ifdef TESTCLOSURE

// Upside: simple separated logic, no class machinery. String comparisons to check name are hoisted into 
// getUpdateFunction() function rather than being in update functions, which actually do no tests at all,
// improvement over TESTGUF above because the closure contains the correct item reference.
// Downsides: calling via std::function objects with captured value as argument is slow.
// Also closure stores an extra reference (pointer), if memory cost matters in some application.
// TODO is there a way to return a generic lamda object instead?
// This one benchmarks the slowest.
#include <functional>


std::function<void(void)> getUpdateClosure(Item& item) {
    
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

#endif




// --------------

#ifdef TESTIU1

// Downsides: it includes a reference dependency on the given item
// (potential object lifetime issue), but OK for ephemeral use, just to
// encapsulate the checking logic in an object.
// Instead, the update() method could just take an Item& argument.
// makeItemUpdater() could take a const Item& or just a string_view of the name.
// Or, we could combine this and the quality and sellIn increment/decrement functions above
// into wrappers around given (not refactorable) Item objects.
// Uses virtual inheritance, update calls may be a bit slower.
// Uses a bit of extra storage for the virtual objects and item reference, if memory use is a concern.
// Upside: Cases are in separate classes, not just separate functions (maybe more modular). String comparison is hoisted into makeItemUpdater1(). Forces use of unique_ptr.
struct ItemUpdater1 {
    Item& m_item;
    explicit ItemUpdater1(Item& item) noexcept : m_item(item) {}
    ItemUpdater1(const ItemUpdater1&) = delete;
    ItemUpdater1(ItemUpdater1&&) noexcept = default;
    ItemUpdater1& operator=(const ItemUpdater1&) = delete;
    ItemUpdater1& operator=(ItemUpdater1&&) = default;
    virtual void update() = 0;
};

// Subclasses for item types. The updater functions above could be moved into these subclasses if desired.
struct TimeLimitedItemUpdater1 : public virtual ItemUpdater1 {
    virtual void update() override { updateTimeLimitedItem(m_item); }
};

struct AgesWellItemUpdater1 : public virtual ItemUpdater1 {
    virtual void update() override { updateAgesWellItem(m_item); }
};

struct NeverDegradesItemUpdater1 : public virtual ItemUpdater1 {
    virtual void update() override { updateNeverDegradesItem(m_item); }
};

struct DegradesFastItemUpdater1 :public virtual ItemUpdater1 {
    virtual void update() override { updateDegradesFastItem(m_item); }
};

struct NormalItemUpdater1 :public virtual ItemUpdater1 {
    virtual void update() override { updateNormalItem(m_item); }
};

#include <memory>

std::unique_ptr<ItemUpdater1> makeItemUpdater1(Item& item) {
    if (item.name.starts_with("Backstage pass"))
        return std::make_unique<TimeLimitedItemUpdater1>(item);
    else if (item.name.starts_with("Aged"))
        return std::make_unique<AgesWellItemUpdater1>(item);
    else if (item.name.starts_with("Sulfuras"))
        return std::make_unique<NeverDegradesItemUpdater1>(item);
    else if (item.name.starts_with("Conjured"))
        return std::make_unique<DegradesFastItemUpdater1>(item);
    else
        return std::make_unique<NormalItemUpdater1>(item);
}
#endif

// ----------------

#ifdef TESTIU2
// Downsides: includes a reference dependency on the given item
// (potential object lifetime issue), but OK for ephemeral use, or
// we could merge in item updater function and the item quality and sellIn increment/decrement functions above into this class as more of a full wrapper around the given (not refactorable or extensible) Item object.
// (Or, The Item reference could be removed, and update() could take an Item&
// argument, and the ItemUpdater2() constructor could take a const Item&
// argument, or just a string_view for the name.)
// Uses a bit of extra storage for the item reference and type value, if memory use is a concern.
// Upside: theoretically inlinable by compiler. Hoists string comparison out of update into constructor.
// This one benchmarks the fastest of all the others.

struct ItemUpdater2 {
    Item& m_item;
    ItemType m_type;
    explicit ItemUpdater2(Item& item) : m_item(item), m_type(itemTypeFromName(item.name)) {
    }
    // todo delete copy (and move?) ctors
    void update() {
      updateItemWithType(m_item, m_type);
    }

    // The updater functions could be member functions if desired, and could incorporate the increment/decrement functions above as well.
};
#endif


// ------------

int main() {

#ifdef TESTONEF
    // Call updateItem() for each item, which just checks the name of the item and updates accordingly.
    // Checking the strings each time is slow and unneccesary, see next implementation.
    {
        Item itemA{"itemA"};
        Item itemB{"itemB"};
        Item someItem{"something"};
        updateItem(itemA);
        updateItem(itemB);
        updateItem(someItem);

    }
#endif

#ifdef TESTONEFOPT
    // Store with respective known type.
    // If we were instead given a bunch of Item objects with no types, we could use itemTypeFromName() to check
    // once and store in the vector.
    {
        Item itemA{"itemA"};
        Item itemB{"itemB"}; 
        Item someItem{"something"};

        std::vector<std::tuple<Item&, ItemType>> items {
            {itemA, Normal},
            {itemB, Normal},
            {someItem, Normal}
        };
    
        // Now we just check ItemType value instead of matching strings. If this is done repeatedly then it's much faster.
        for(auto t : items) 
        {
            Item& item = std::get<0>(t);
            ItemType type = std::get<1>(t);
            updateItemWithType(item, type);
        }
    }
  #endif

#ifdef TESTGUF
    // using getUpdateFunction() to get a std::function or similar. These can be stored for later and used without rematching strings:
    // Downside: the function *must* be called with the correct item. (E.g. store as list of pairs or tuples.)
    {
        Item itemA{"itemA"};
        Item itemB{"itemB"};
        Item someItem{"something"};
        auto f = getUpdateFunction(itemA);
        auto f2 = getUpdateFunction(itemB);
        auto f3 = getUpdateFunction(someItem);
        f2(itemB);
        f(itemA);
        f3(someItem);

    }
#endif

#ifdef TESTCLOSURE
    // using getUpdateClosure() to get a std::function or similar. These can be stored for later and used without rematching strings. Advantage over previous is that we don't need to call the right function with the right item object, just call the function which has stored a reference to the item object.
    // downside: slow.
    {
        Item itemA{"itemA"};
        Item itemB{"itemB"};
        Item someItem{"something"};
        auto f = getUpdateClosure(itemA);
        auto f2 = getUpdateClosure(itemB);
        auto f3 = getUpdateClosure(someItem);
        f();
        f2();
        f3();

  }
#endif

#ifdef TESTIU1
    // using ItemUpdater1 objects (via unique_ptrs):
    {
        Item itemA{"itemA"};
        Item itemB{"itemB"};
        Item someItem{"something"};
        auto u1 = makeItemUpdater1(itemA);
        auto u2 = makeItemUpdater1(itemB);
        auto u3 = makeItemUpdater1(someItem);
        u1->update();
        u2->update();
        u3->update();

    }
#endif

#ifdef TESTIU2
    // Using ItemUpdater2 objects:
    // This implementation benchmarks as the fastest, by the way.
    {
        Item itemA{"itemA"};
        Item itemB{"itemB"};
        Item someItem{"something"};
        auto u1 = ItemUpdater2(itemA);
        auto u2 = ItemUpdater2(itemB);
        auto u3 = ItemUpdater2(someItem);
        u1.update();
        u2.update();
        u3.update();

    }
#endif


    return 0;
}

