
#ifndef INVENTORY_H
#define INVENTORY_H

#include "Item.h"
#include <vector>
#include <memory>
#include <string>

class Inventory {
private:
    std::vector<std::shared_ptr<Item>> items_;

public:
    void addItem(std::shared_ptr<Item> item);
    bool removeItem(const std::string& itemName);
    bool hasItem(const std::string& itemName) const;
    void displayInventory() const;
    int getItemCount() const;
};

#endif
