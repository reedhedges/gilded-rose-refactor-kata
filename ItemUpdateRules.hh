#pragma once


// Functions implementing the different update rules for each type,
// and some utilities for updating values in the Item class since we can't add accessors:
// (Alternatively, we could define a wrapper object around the given Item objects. The
// object oriented refactorings (gilded_wrapper_class.cc and gilded_virtual_classes.cc could
// then derive from or delegate to that, perhaps.) 

#include "Item.hh"
#include "ItemTypes.hh"
#include "util.hh"

#include <climits>

void Item_increment_quality(Item& item, int amount)
{
  util::increment_clamp(item.quality, amount, 50);
}

void Item_decrement_quality(Item& item, int amount)
{
  util::decrement_clamp(item.quality, amount, 0);
}

void Item_decrement_sellIn(Item& item)
{
  util::decrement_clamp(item.sellIn, 1, INT_MIN);
}



void updateNormalItem(Item& item) 
{
    if(item.sellIn <= 0) {
        Item_decrement_quality(item, 2);
    } else {
        Item_decrement_quality(item, 1);
    }
    Item_decrement_sellIn(item);
}

void updateAgesWellItem(Item& item) 
{
    Item_increment_quality(item, 1);
    Item_decrement_sellIn(item);
    // TODO check original implementation's tests to see if the above is correct (does quality increase by two if it's past sellIn?)
}

void updateNeverDegradesItem(Item& item) 
{
    Item_decrement_sellIn(item);
}

void updateTimeLimitedItem(Item& item) 
{
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

void updateDegradesFastItem(Item& item) 
{
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



