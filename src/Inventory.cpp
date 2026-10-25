
#include "Inventory.h"
#include <iostream>
#include <algorithm>

void Inventory::addItem(std::shared_ptr<Item> item) {
    if (item != nullptr) {
        items_.push_back(item);
        std::cout << item->getName() << " added to inventory.\n";
    }
}

bool Inventory::removeItem(const std::string& itemName) {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&itemName](const std::shared_ptr<Item>& item) {
            return item->getName() == itemName;
        });

    if (it != items_.end()) {
        items_.erase(it);
        return true;
    }

    return false;
}

bool Inventory::hasItem(const std::string& itemName) const {
    for (const auto& item : items_) {
        if (item->getName() == itemName) {
            return true;
        }
    }

    return false;
}


void Inventory::displayInventory() const {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "           PLAYER INVENTORY             \n";
    std::cout << "========================================\n";

    std::cout << "Total Items: " << items_.size() << "\n";

    if (items_.empty()) {
        std::cout << "\nYour inventory is empty.\n";
        std::cout << "Explore rooms to collect useful items!\n";
        std::cout << "========================================\n";
        return;
    }

    std::cout << "\nCollected Items:\n";

    for (std::size_t i = 0; i < items_.size(); ++i) {
        std::cout << "\n" << i + 1 << ". "
                  << items_[i]->getName() << '\n';

        std::cout << "   Description: "
                  << items_[i]->getDescription() << '\n';

        std::cout << "   Usable: "
                  << (items_[i]->isUsable() ? "Yes" : "No") << '\n';
    }

    std::cout << "\n========================================\n";
}


int Inventory::getItemCount() const {
    return static_cast<int>(items_.size());
}
