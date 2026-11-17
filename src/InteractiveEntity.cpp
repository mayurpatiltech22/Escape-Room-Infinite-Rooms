#include "InteractiveEntity.h"
#include <cmath>
#include <algorithm>

InteractiveEntity::InteractiveEntity(EntityType type, const sf::Vector2f& pos, const sf::Vector2f& size, const std::string& name)
    : type_(type), position_(pos), size_(size), name_(name), active_(true) {}

sf::FloatRect InteractiveEntity::getBounds() const {
    return sf::FloatRect({position_.x - size_.x * 0.5f, position_.y - size_.y * 0.5f}, size_);
}

sf::Vector2f InteractiveEntity::getPosition() const {
    return position_;
}

const std::string& InteractiveEntity::getName() const {
    return name_;
}

EntityType InteractiveEntity::getType() const {
    return type_;
}

bool InteractiveEntity::isActive() const {
    return active_;
}

void InteractiveEntity::setActive(bool active) {
    active_ = active;
}

bool InteractiveEntity::isNear(const sf::Vector2f& point, float radius) const {
    float dx = position_.x - point.x;
    float dy = position_.y - point.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// ==========================================
// DOOR ENTITY IMPLEMENTATION
// ==========================================
DoorEntity::DoorEntity(int doorId, const std::string& name, int destRoomId, int puzzleId,
                       const sf::Vector2f& pos, DoorSide side)
    : InteractiveEntity(EntityType::Door, pos,
                        (side == DoorSide::North || side == DoorSide::South) ? sf::Vector2f(110.0f, 40.0f) : sf::Vector2f(40.0f, 110.0f),
                        name),
      doorId_(doorId),
      destinationRoomId_(destRoomId),
      requiredPuzzleId_(puzzleId),
      unlocked_(false),
      side_(side),
      openProgress_(0.0f) {}

void DoorEntity::update(float dt) {
    if (unlocked_) {
        openProgress_ = std::min(1.0f, openProgress_ + dt * 2.5f);
    } else {
        openProgress_ = std::max(0.0f, openProgress_ - dt * 2.5f);
    }
}

void DoorEntity::render(sf::RenderTarget& target, const sf::Font& font) {
    // Outer Door Frame
    sf::RectangleShape frame(size_);
    frame.setOrigin({size_.x * 0.5f, size_.y * 0.5f});
    frame.setPosition(position_);
    frame.setFillColor(sf::Color(25, 28, 36));
    frame.setOutlineColor(unlocked_ ? sf::Color(40, 200, 100) : sf::Color(220, 60, 60));
    frame.setOutlineThickness(3.0f);
    target.draw(frame);

    // Door Panels (Slide open based on openProgress_)
    if (side_ == DoorSide::North || side_ == DoorSide::South) {
        float panelW = (size_.x * 0.5f) * (1.0f - openProgress_);
        if (panelW > 2.0f) {
            // Left Panel
            sf::RectangleShape pLeft(sf::Vector2f(panelW, size_.y - 6.0f));
            pLeft.setPosition({position_.x - size_.x * 0.5f + 3.0f, position_.y - size_.y * 0.5f + 3.0f});
            pLeft.setFillColor(sf::Color(75, 50, 35));
            pLeft.setOutlineColor(sf::Color(100, 70, 50));
            pLeft.setOutlineThickness(1.0f);
            target.draw(pLeft);

            // Right Panel
            sf::RectangleShape pRight(sf::Vector2f(panelW, size_.y - 6.0f));
            pRight.setPosition({position_.x + size_.x * 0.5f - 3.0f - panelW, position_.y - size_.y * 0.5f + 3.0f});
            pRight.setFillColor(sf::Color(75, 50, 35));
            pRight.setOutlineColor(sf::Color(100, 70, 50));
            pRight.setOutlineThickness(1.0f);
            target.draw(pRight);
        }
    } else {
        float panelH = (size_.y * 0.5f) * (1.0f - openProgress_);
        if (panelH > 2.0f) {
            sf::RectangleShape pTop(sf::Vector2f(size_.x - 6.0f, panelH));
            pTop.setPosition({position_.x - size_.x * 0.5f + 3.0f, position_.y - size_.y * 0.5f + 3.0f});
            pTop.setFillColor(sf::Color(75, 50, 35));
            target.draw(pTop);

            sf::RectangleShape pBot(sf::Vector2f(size_.x - 6.0f, panelH));
            pBot.setPosition({position_.x - size_.x * 0.5f + 3.0f, position_.y + size_.y * 0.5f - 3.0f - panelH});
            pBot.setFillColor(sf::Color(75, 50, 35));
            target.draw(pBot);
        }
    }

    // Lock status beacon
    sf::CircleShape beacon(7.0f);
    beacon.setOrigin({7.0f, 7.0f});
    beacon.setPosition(position_);
    beacon.setFillColor(unlocked_ ? sf::Color(50, 255, 120) : sf::Color(255, 50, 50));
    beacon.setOutlineColor(sf::Color::White);
    beacon.setOutlineThickness(1.0f);
    target.draw(beacon);

    // Door Label
    sf::Text text(font, name_, 12);
    text.setFillColor(unlocked_ ? sf::Color(180, 255, 200) : sf::Color(255, 180, 180));
    text.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({bounds.size.x * 0.5f, bounds.size.y * 0.5f});

    if (side_ == DoorSide::North) {
        text.setPosition({position_.x, position_.y + 28.0f});
    } else if (side_ == DoorSide::South) {
        text.setPosition({position_.x, position_.y - 28.0f});
    } else if (side_ == DoorSide::East) {
        text.setPosition({position_.x - 30.0f, position_.y - 25.0f});
    } else {
        text.setPosition({position_.x + 30.0f, position_.y - 25.0f});
    }
    target.draw(text);
}

int DoorEntity::getDoorId() const { return doorId_; }
int DoorEntity::getDestinationRoomId() const { return destinationRoomId_; }
int DoorEntity::getRequiredPuzzleId() const { return requiredPuzzleId_; }
bool DoorEntity::isUnlocked() const { return unlocked_; }
void DoorEntity::setUnlocked(bool unlocked) { unlocked_ = unlocked; }
DoorSide DoorEntity::getSide() const { return side_; }

// ==========================================
// PUZZLE PEDESTAL IMPLEMENTATION
// ==========================================
PuzzlePedestalEntity::PuzzlePedestalEntity(int puzzleId, const std::string& title, const sf::Vector2f& pos)
    : InteractiveEntity(EntityType::PuzzlePedestal, pos, sf::Vector2f(44.0f, 44.0f), title),
      puzzleId_(puzzleId),
      solved_(false),
      rotationTimer_(0.0f) {}

void PuzzlePedestalEntity::update(float dt) {
    rotationTimer_ += dt;
}

void PuzzlePedestalEntity::render(sf::RenderTarget& target, const sf::Font& font) {
    // Pedestal Base
    sf::CircleShape base(22.0f);
    base.setOrigin({22.0f, 22.0f});
    base.setPosition(position_);
    base.setFillColor(sf::Color(35, 38, 50));
    base.setOutlineColor(solved_ ? sf::Color(50, 220, 120) : sf::Color(240, 180, 50));
    base.setOutlineThickness(3.0f);
    target.draw(base);

    // Inner Pillar
    sf::RectangleShape pillar(sf::Vector2f(16.0f, 16.0f));
    pillar.setOrigin({8.0f, 8.0f});
    pillar.setPosition(position_);
    pillar.setFillColor(sf::Color(55, 60, 75));
    target.draw(pillar);

    // Floating Holographic Symbol
    float floatY = std::sin(rotationTimer_ * 4.0f) * 4.0f;
    sf::Vector2f orbPos(position_.x, position_.y - 24.0f + floatY);

    sf::CircleShape orb(12.0f);
    orb.setOrigin({12.0f, 12.0f});
    orb.setPosition(orbPos);
    orb.setFillColor(solved_ ? sf::Color(40, 220, 110, 190) : sf::Color(255, 190, 40, 190));
    target.draw(orb);

    sf::Text sym(font, solved_ ? "OK" : "?", 13);
    sym.setStyle(sf::Text::Bold);
    sym.setFillColor(sf::Color::White);
    sf::FloatRect sb = sym.getLocalBounds();
    sym.setOrigin({sb.size.x * 0.5f, sb.size.y * 0.5f});
    sym.setPosition({orbPos.x, orbPos.y - 1.0f});
    target.draw(sym);

    // Label
    sf::Text label(font, solved_ ? "[SOLVED]" : name_, 11);
    label.setFillColor(solved_ ? sf::Color(140, 255, 180) : sf::Color(255, 230, 140));
    sf::FloatRect lb = label.getLocalBounds();
    label.setOrigin({lb.size.x * 0.5f, lb.size.y * 0.5f});
    label.setPosition({position_.x, position_.y + 26.0f});
    target.draw(label);
}

int PuzzlePedestalEntity::getPuzzleId() const { return puzzleId_; }
bool PuzzlePedestalEntity::isSolved() const { return solved_; }
void PuzzlePedestalEntity::setSolved(bool solved) { solved_ = solved; }

// ==========================================
// TRAP ZONE IMPLEMENTATION
// ==========================================
TrapZoneEntity::TrapZoneEntity(const std::string& name, int damage, const sf::Vector2f& pos, const sf::Vector2f& size)
    : InteractiveEntity(EntityType::TrapZone, pos, size, name),
      trapName_(name),
      damage_(damage),
      triggered_(false),
      hazardTimer_(0.0f),
      flashTimer_(0.0f) {}

void TrapZoneEntity::update(float dt) {
    hazardTimer_ += dt;
    if (flashTimer_ > 0.0f) {
        flashTimer_ -= dt;
    }
}

void TrapZoneEntity::render(sf::RenderTarget& target, const sf::Font& font) {
    sf::RectangleShape zone(size_);
    zone.setOrigin({size_.x * 0.5f, size_.y * 0.5f});
    zone.setPosition(position_);

    if (flashTimer_ > 0.0f) {
        // Red flashing when triggered
        zone.setFillColor(sf::Color(240, 50, 50, 180));
        zone.setOutlineColor(sf::Color::White);
        zone.setOutlineThickness(3.0f);
    } else if (triggered_) {
        // Spent / activated trap
        zone.setFillColor(sf::Color(45, 30, 30, 120));
        zone.setOutlineColor(sf::Color(120, 50, 50, 160));
        zone.setOutlineThickness(2.0f);
    } else {
        // Armed trap - subtle hazard pattern
        zone.setFillColor(sf::Color(40, 35, 45, 100));
        zone.setOutlineColor(sf::Color(180, 110, 40, 150));
        zone.setOutlineThickness(2.0f);
    }
    target.draw(zone);

    // Hazard grating lines
    const int count = static_cast<int>(size_.x / 20.0f);
    for (int i = 0; i < count; ++i) {
        float lx = position_.x - size_.x * 0.5f + 10.0f + i * 20.0f;
        sf::Vertex bar[] = {
            sf::Vertex{sf::Vector2f(lx, position_.y - size_.y * 0.5f + 4.0f), sf::Color(160, 120, 50, 160)},
            sf::Vertex{sf::Vector2f(lx, position_.y + size_.y * 0.5f - 4.0f), sf::Color(160, 120, 50, 160)}
        };
        target.draw(bar, 2, sf::PrimitiveType::Lines);
    }

    // Hazard skull / warning sign
    sf::Text sign(font, triggered_ ? "[TRIGGERED TRAP]" : "! HAZARD ZONE !", 10);
    sign.setStyle(sf::Text::Bold);
    sign.setFillColor(triggered_ ? sf::Color(160, 120, 120) : sf::Color(255, 180, 60));
    sf::FloatRect sb = sign.getLocalBounds();
    sign.setOrigin({sb.size.x * 0.5f, sb.size.y * 0.5f});
    sign.setPosition(position_);
    target.draw(sign);
}

bool TrapZoneEntity::isTriggered() const { return triggered_; }
void TrapZoneEntity::trigger() {
    triggered_ = true;
    flashTimer_ = 0.6f;
}
int TrapZoneEntity::getDamage() const { return damage_; }

// ==========================================
// ITEM PICKUP IMPLEMENTATION
// ==========================================
ItemPickupEntity::ItemPickupEntity(const std::string& name, const std::string& desc, int healAmount, const sf::Vector2f& pos)
    : InteractiveEntity(EntityType::ItemPickup, pos, sf::Vector2f(28.0f, 28.0f), name),
      itemName_(name),
      description_(desc),
      healAmount_(healAmount),
      floatTimer_(0.0f),
      collected_(false) {}

void ItemPickupEntity::update(float dt) {
    if (!collected_) {
        floatTimer_ += dt;
    }
}

void ItemPickupEntity::render(sf::RenderTarget& target, const sf::Font& font) {
    if (collected_) return;

    float floatY = std::sin(floatTimer_ * 5.0f) * 4.0f;
    sf::Vector2f drawPos(position_.x, position_.y + floatY);

    // Glowing ground aura
    sf::CircleShape aura(18.0f);
    aura.setOrigin({18.0f, 18.0f});
    aura.setPosition({position_.x, position_.y + 6.0f});
    aura.setFillColor(sf::Color(80, 220, 255, 60));
    target.draw(aura);

    // Item container/flask
    sf::CircleShape flask(11.0f);
    flask.setOrigin({11.0f, 11.0f});
    flask.setPosition(drawPos);
    flask.setFillColor(healAmount_ > 0 ? sf::Color(230, 40, 60) : sf::Color(240, 210, 60));
    flask.setOutlineColor(sf::Color::White);
    flask.setOutlineThickness(2.0f);
    target.draw(flask);

    // Item Name label
    sf::Text label(font, itemName_, 11);
    label.setStyle(sf::Text::Bold);
    label.setFillColor(sf::Color(220, 240, 255));
    sf::FloatRect lb = label.getLocalBounds();
    label.setOrigin({lb.size.x * 0.5f, lb.size.y * 0.5f});
    label.setPosition({drawPos.x, drawPos.y - 20.0f});
    target.draw(label);
}

bool ItemPickupEntity::isCollected() const { return collected_; }
void ItemPickupEntity::collect() {
    collected_ = true;
    active_ = false;
}
int ItemPickupEntity::getHealAmount() const { return healAmount_; }
const std::string& ItemPickupEntity::getDescription() const { return description_; }
