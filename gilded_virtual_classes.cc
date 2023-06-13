

#include "Item.hh"
#include "util.hh"
#include <climits>

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
    // if desired.  (E.g. separate functions to increase quality by standard and accelerated amounts, test sellIn for "past sell by date" etc.)

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

    int get_quality()
    {
      return m_item.quality;
    }

    int get_sellIn()
    {
      return m_item.sellIn;
    }

    void set_quality(int value)
    {
      m_item.quality = value;
    }


public:
    explicit UpdatableItem(Item& item) noexcept : m_item(item) {}

    // Subclasses should override this to update a certain type of item:
    virtual void update() = 0;
};


// Subclasses for item types:


class TimeLimitedUpdatableItem : public virtual UpdatableItem 
{
public:
    explicit TimeLimitedUpdatableItem(Item& item) : UpdatableItem(item) {}

    virtual void update() override 
    { 
      if (get_sellIn() <= 0) {
          set_quality(0);
      } else if (get_sellIn() <= 5) {
          decrement_quality(3);
      } else if (get_sellIn() <= 10) {
          decrement_quality(2);
      } else {
          decrement_quality(1);
      }
      decrement_sellIn();
    }

};

class AgesWellUpdatableItem : public virtual UpdatableItem 
{
public:
    explicit AgesWellUpdatableItem(Item& item) : UpdatableItem(item) {}

    virtual void update() override 
    { 
      increment_quality(1);
      decrement_sellIn();
      // TODO check original implementation to see if the above is correct (does quality increase by two if it's past sellIn?)
    }
};

class NeverDegradesUpdatableItem : public virtual UpdatableItem 
{
public:
    explicit NeverDegradesUpdatableItem(Item& item) : UpdatableItem(item) {}

    virtual void update() override 
    { 
      decrement_sellIn();
    }
};

class DegradesFastUpdatableItem : public virtual UpdatableItem 
{
public:
    explicit DegradesFastUpdatableItem(Item& item) : UpdatableItem(item) {}

    virtual void update() override 
    { 
      if (get_sellIn() <= 0) {
        decrement_quality(4);
      } else {
        decrement_quality(2);
      }
      decrement_sellIn();
    }
};

class NormalUpdatableItem : public virtual UpdatableItem 
{
public:
    explicit NormalUpdatableItem(Item& item) : UpdatableItem(item) {}

    virtual void update() override 
    { 
      if (get_sellIn() <= 0) {
        decrement_quality(2);
      } else {
        decrement_quality(1);
      }
      decrement_sellIn();
    }
};



// Factory function to make unique_ptr for a subclass of UpdatableItem based on the name
// of the item matching different patterns:

#include <memory>

std::unique_ptr<UpdatableItem> makeUpdatableItem(Item& item) 
{
    // TODO instead of checking each string pattern here, we could keep a 
    // static table of patterns and subclass or factory function for each,
    // or map to ItemType value and swith on that here.
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
