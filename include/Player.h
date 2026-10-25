
#ifndef PLAYER_H
#define PLAYER_H

#include "Inventory.h"
#include <string>

class Player {
private:
    std::string name_;
    int health_;
    int score_;
    Inventory inventory_;

public:
    explicit Player(const std::string& name, int startingHealth = 100);

    std::string getName() const;
    int getHealth() const;
    int getScore() const;
    bool isAlive() const;

    void takeDamage(int damage);
    void heal(int amount);
    void addScore(int points);

    Inventory& getInventory();
    const Inventory& getInventory() const;

    void displayStatus() const;
};

#endif
