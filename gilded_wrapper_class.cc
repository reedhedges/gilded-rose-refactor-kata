

#include "Item.hh"
#include "ItemTypes.hh"
#include "ItemUpdateRules.hh"


// Wrapper around given (unchangable) Item class, use different update implementation for different types of objects. 
// Uses the utility and accessor functions from ItemUpdateRules.hh but these could also be moved into the class.
// This implementation happens to benchmark the fastest.

class UpdatableItem 
{
private:
    Item& m_item;
    ItemType m_type;

public:
    explicit UpdatableItem(Item& item) : m_item(item), m_type(itemTypeFromName(item.name))  {}

    void update()  
    {
      updateItemWithType(m_item, m_type);
    }
};


int main() 
{
    Item itemA{"itemA"};
    Item itemB{"itemB"};
    Item someItem{"something"};
    auto u1 = UpdatableItem(itemA);
    auto u2 = UpdatableItem(itemB);
    auto u3 = UpdatableItem(someItem);
    u1.update();
    u2.update();
    u3.update();

    return 0;
}
