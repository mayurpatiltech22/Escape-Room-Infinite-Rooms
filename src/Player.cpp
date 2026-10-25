
#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(const std::string& name, int startingHealth)
    : name_(name),
      health_(std::max(0, startingHealth)),
      score_(10) {
}

std::string Player::getName() const {
    return name_;
}

int Player::getHealth() const {
    return health_;
}

int Player::getScore() const {
    return score_;
}

bool Player::isAlive() const {
    return health_ > 0;
}

void Player::takeDamage(int damage) {
    if (damage <= 0 || !isAlive()) {
        return;
    }

    health_ = std::max(0, health_ - damage);
    std::cout << name_ << " took " << damage
              << " damage. Health: " << health_ << '\n';
}

void Player::heal(int amount) {
    if (amount <= 0 || !isAlive()) {
        return;
    }

    health_ = std::min(100, health_ + amount);
    std::cout << name_ << " healed. Health: "
              << health_ << '\n';
}

void Player::addScore(int points) {
    score_ = std::max(0, score_ + points);
}

Inventory& Player::getInventory() {
    return inventory_;
}

const Inventory& Player::getInventory() const {
    return inventory_;
}


void Player::displayStatus() const {
    std::cout << "\n";
    std::cout << "====================================\n";
    std::cout << "          PLAYER STATUS             \n";
    std::cout << "====================================\n";

    std::cout << "Player Name      : " << name_ << '\n';
    std::cout << "Health           : " << health_ << "/100 HP\n";
    
    if (health_ > 70) {
        std::cout << "Health Status    : Excellent\n";
    } else if (health_ > 40) {
        std::cout << "Health Status    : Good\n";
    } else if (health_ > 0) {
        std::cout << "Health Status    : Critical\n";
    } else {
        std::cout << "Health Status    : Game Over\n";
    }

    std::cout << "Score            : " << score_ << " points\n";
    std::cout << "Items Collected  : "
              << inventory_.getItemCount() << '\n';

    std::cout << "Player Condition : "
              << (isAlive() ? "Alive" : "Defeated") << '\n';

    std::cout << "====================================\n";
}

