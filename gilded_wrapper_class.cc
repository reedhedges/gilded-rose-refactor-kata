
#include <cassert>
#include <cstdio>



#include "Item.hh"



// Wrapper around given (unchangable) Item class, use virtual inheritance to implement different update behaviors.
// Uses virtual inheritance, update calls may be a bit slower.
// Uses a bit of extra storage for the virtual objects and item reference, if memory use is a concern.
// Upside: Cases are in separate classes, not just separate functions (maybe more modular). String comparison is hoisted into makeUpdatableItem(). Forces use of unique_ptr.

class UpdatableItem 
{
private:
    Item& m_item;

protected:
    // Subclasses use these to modify item.
    // We could add more abstraction here related to the required behavior of the updates here 
    // if desired.  (E.g. separate functions to increase quality by standard and accelerated amounts, etc.)

    void increment_quality(int amount)
    {
      util::increment_clamp(m_item.quality, amount, 50);
    }

    void decrement_quality(int amount)
    {
      util::decrement_clamp(m_item.quality, amount, 0);
    }

    void decrement_sellIn()
    {
      util::decrement_clamp(m_item.sellIn, 1, INT_MIN);
    }


public:
    explicit UpdatableItem(Item& item) noexcept : m_item(item) {}

    UpdatableItem(const UpdatableItem&) = delete;
    UpdatableItem(UpdatableItem&&) noexcept = default;
    UpdatableItem& operator=(const UpdatableItem&) = delete;
    UpdatableItem& operator=(UpdatableItem&&) = default;

    // Subclasses should override this to update a type of item:
    virtual void update() = 0;


};


// Subclasses for item types:

class TimeLimitedUpdatableItem : private virtual UpdatableItem 
{
public:
    virtual void update() override 
    { 
      if(item.sellIn <= 0) {
          item.quality = 0;
      } else if(item.sellIn <= 5) {
          decrement_quality(item, 3);
      } else if(item.sellIn <= 10) {
          decrement_quality(item, 2);
      } else {
          decrement_quality(item, 1);
      }
      decrement_sellIn(item);
    }
};

class AgesWellUpdatableItem : private virtual UpdatableItem 
{
public:
    virtual void update() override 
    { 
      increment_quality(item, 1);
      decrement_sellIn(item);
      // TODO check original implementation's tests to see if the above is correct (does quality increase by two if it's past sellIn?)
    }
};

class NeverDegradesUpdatableItem : private virtual UpdatableItem 
{
public:
    virtual void update() override 
    { 
      decrement_sellIn(item);
    }
};

class DegradesFastUpdatableItem : private virtual UpdatableItem 
{
public:
    virtual void update() override 
    { 
      if(item.sellIn <= 0) {
        decrement_quality(item, 4);
      } else {
        decrement_quality(item, 2);
      }
      decrement_sellIn(item);
    }
};

class NormalUpdatableItem : private virtual UpdatableItem 
{
public:
    virtual void update() override 
    { 
      if(item.sellIn <= 0) {
        decrement_quality(item, 2);
      } else {
        decrement_quality(item, 1);
      }
      decrement_sellIn(item);
    }
};



// Factory function to make unique_ptr for a subclass of UpdatableItem based on the name
// of the item matching different patterns:

#include <memory>

std::unique_ptr<UpdatableItem> makeUpdatableItem(Item& item) 
{
    if (item.name.starts_with("Backstage pass"))
        return std::make_unique<TimeLimitedUpdatableItem>(item);
    else if (item.name.starts_with("Aged"))
        return std::make_unique<AgesWellUpdatableItem>(item);
    else if (item.name.starts_with("Sulfuras"))
        return std::make_unique<NeverDegradesUpdatableItem>(item);
    else if (item.name.starts_with("Conjured"))
        return std::make_unique<DegradesFastUpdatableItem>(item);
    else
        return std::make_unique<NormalUpdatableItem>(item);
}



int main() 
{
    Item itemA{"itemA"};
    Item itemB{"itemB"};
    Item someItem{"something"};
    auto u1 = makeUpdatableItem(itemA);
    auto u2 = makeUpdatableItem(itemB);
    auto u3 = makeUpdatableItem(someItem);
    u1->update();
    u2->update();
    u3->update();

    return 0;
}
