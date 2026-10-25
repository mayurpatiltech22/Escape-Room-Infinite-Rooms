
#include "Item.h"

Item::Item(const std::string& name,
           const std::string& description,
           bool usable)
    : name_(name),
      description_(description),
      usable_(usable) {
}

std::string Item::getName() const {
    return name_;
}

std::string Item::getDescription() const {
    return description_;
}

bool Item::isUsable() const {
    return usable_;
}

bool Item::use() const {
    return usable_;
}
