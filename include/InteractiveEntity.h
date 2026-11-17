#ifndef INTERACTIVE_ENTITY_H
#define INTERACTIVE_ENTITY_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

enum class EntityType {
    Door,
    PuzzlePedestal,
    TrapZone,
    ItemPickup
};

enum class DoorSide {
    North,
    South,
    East,
    West
};

class InteractiveEntity {
protected:
    EntityType type_;
    sf::Vector2f position_;
    sf::Vector2f size_;
    std::string name_;
    bool active_;

public:
    InteractiveEntity(EntityType type, const sf::Vector2f& pos, const sf::Vector2f& size, const std::string& name);
    virtual ~InteractiveEntity() = default;

    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget& target, const sf::Font& font) = 0;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    const std::string& getName() const;
    EntityType getType() const;
    bool isActive() const;
    void setActive(bool active);

    bool isNear(const sf::Vector2f& point, float radius = 65.0f) const;
};

// ==========================================
// DOOR ENTITY
// ==========================================
class DoorEntity : public InteractiveEntity {
private:
    int doorId_;
    int destinationRoomId_;
    int requiredPuzzleId_;
    bool unlocked_;
    DoorSide side_;
    float openProgress_; // 0.0 to 1.0 animation

public:
    DoorEntity(int doorId, const std::string& name, int destRoomId, int puzzleId,
               const sf::Vector2f& pos, DoorSide side);

    void update(float dt) override;
    void render(sf::RenderTarget& target, const sf::Font& font) override;

    int getDoorId() const;
    int getDestinationRoomId() const;
    int getRequiredPuzzleId() const;
    bool isUnlocked() const;
    void setUnlocked(bool unlocked);
    DoorSide getSide() const;
};

// ==========================================
// PUZZLE PEDESTAL ENTITY
// ==========================================
class PuzzlePedestalEntity : public InteractiveEntity {
private:
    int puzzleId_;
    bool solved_;
    float rotationTimer_;

public:
    PuzzlePedestalEntity(int puzzleId, const std::string& title, const sf::Vector2f& pos);

    void update(float dt) override;
    void render(sf::RenderTarget& target, const sf::Font& font) override;

    int getPuzzleId() const;
    bool isSolved() const;
    void setSolved(bool solved);
};

// ==========================================
// TRAP ZONE ENTITY
// ==========================================
class TrapZoneEntity : public InteractiveEntity {
private:
    std::string trapName_;
    int damage_;
    bool triggered_;
    float hazardTimer_;
    float flashTimer_;

public:
    TrapZoneEntity(const std::string& name, int damage, const sf::Vector2f& pos, const sf::Vector2f& size);

    void update(float dt) override;
    void render(sf::RenderTarget& target, const sf::Font& font) override;

    bool isTriggered() const;
    void trigger();
    int getDamage() const;
};

// ==========================================
// ITEM PICKUP ENTITY
// ==========================================
class ItemPickupEntity : public InteractiveEntity {
private:
    std::string itemName_;
    std::string description_;
    int healAmount_;
    float floatTimer_;
    bool collected_;

public:
    ItemPickupEntity(const std::string& name, const std::string& desc, int healAmount, const sf::Vector2f& pos);

    void update(float dt) override;
    void render(sf::RenderTarget& target, const sf::Font& font) override;

    bool isCollected() const;
    void collect();
    int getHealAmount() const;
    const std::string& getDescription() const;
};

#endif // INTERACTIVE_ENTITY_H
