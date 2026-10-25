
#include "Trap.h"
#include "Player.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

Trap::Trap(const std::string& name,
           const std::string& description,
           int damage)
    : name_(name),
      description_(description),
      damage_(std::max(0, damage)),
      triggered_(false) {}

std::string Trap::getName() const { return name_; }

std::string Trap::getDescription() const { return description_; }

int Trap::getDamage() const { return damage_; }

bool Trap::isTriggered() const { return triggered_; }


void Trap::trigger(Player& player) {
    if (triggered_) {
        std::cout << "\n====================================\n";
        std::cout << "TRAP STATUS: ALREADY TRIGGERED\n";
        std::cout << "Trap Name: " << name_ << '\n';
        std::cout << "====================================\n";
        return;
    }

    triggered_ = true;

    std::cout << "\n====================================\n";
    std::cout << "          TRAP ENCOUNTER             \n";
    std::cout << "====================================\n";
    std::cout << "Trap Name : " << name_ << '\n';
    std::cout << "Details   : " << description_ << '\n';
    std::cout << "Damage    : " << damage_ << " HP\n";

    
if (std::rand() % 2 == 0) {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "     CONGRATULATIONS, PLAYER!\n";
    std::cout << "========================================\n";
    std::cout << "  YOU ARE SAFE FROM THE TRAP!\n";
    std::cout << "  Great escape! No damage taken.\n";

    player.addScore(2);

    std::cout << "  Health Remaining : "
              << player.getHealth() << "/100 HP\n";
    std::cout << "  Score Reward     : +2 points\n";
    std::cout << "  Current Score    : "
              << player.getScore() << " points\n";
    std::cout << "========================================\n";

} else {
    int oldHealth = player.getHealth();

    player.takeDamage(damage_);
    player.addScore(-5);

    int healthLost = oldHealth - player.getHealth();

    std::cout << "\n";
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    std::cout << "             TRAP HIT!\n";
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    std::cout << "  WARNING! You triggered a trap!\n";
    std::cout << "  Health Lost      : -" << healthLost << " HP\n";
    std::cout << "  Remaining Health : "
              << player.getHealth() << "/100 HP\n";
    std::cout << "  Score Penalty    : -5 points\n";
    std::cout << "  Current Score    : "
              << player.getScore() << " points\n";

    if (!player.isAlive()) {
        std::cout << "\n  GAME OVER! You have been defeated!\n";
    } else if (player.getHealth() <= 30) {
        std::cout << "  CRITICAL HEALTH! Use a Health Potion!\n";
    } else {
        std::cout << "  Be careful! Watch out for more traps!\n";
    }

    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
}


    std::cout << "====================================\n";
}
