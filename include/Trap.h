
#ifndef TRAP_H
#define TRAP_H

#include <string>

class Player;

class Trap {
private:
    std::string name_;
    std::string description_;
    int damage_;
    bool triggered_;

public:
    Trap(const std::string& name,
         const std::string& description,
         int damage);

    std::string getName() const;
    std::string getDescription() const;
    int getDamage() const;
    bool isTriggered() const;

    void trigger(Player& player);
};

#endif
