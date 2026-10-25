
#ifndef ITEM_H
#define ITEM_H

#include <string>

class Item {
private:
    std::string name_;
    std::string description_;
    bool usable_;

public:
    Item(const std::string& name,
         const std::string& description,
         bool usable = true);

    virtual ~Item() = default;

    std::string getName() const;
    std::string getDescription() const;
    bool isUsable() const;

    virtual bool use() const;
};

#endif
