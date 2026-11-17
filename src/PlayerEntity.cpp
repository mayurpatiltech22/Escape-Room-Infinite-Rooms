#include "PlayerEntity.h"
#include "InteractiveEntity.h"
#include "SoundSystem.h"
#include <cmath>
#include <algorithm>

PlayerEntity::PlayerEntity(const sf::Vector2f& startPos, sf::Font& font)
    : position_(startPos),
      velocity_(0.0f, 0.0f),
      direction_(FacingDirection::Down),
      speed_(240.0f),
      walkTimer_(0.0f),
      stepSoundTimer_(0.0f),
      isMoving_(false),
      font_(font) {}

void PlayerEntity::handleInput(float /*dt*/) {
    velocity_ = sf::Vector2f(0.0f, 0.0f);

    bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
    bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);

    if (up) velocity_.y -= 1.0f;
    if (down) velocity_.y += 1.0f;
    if (left) velocity_.x -= 1.0f;
    if (right) velocity_.x += 1.0f;

    // Determine facing direction
    if (std::abs(velocity_.x) > 0.0f || std::abs(velocity_.y) > 0.0f) {
        isMoving_ = true;
        if (std::abs(velocity_.x) > std::abs(velocity_.y)) {
            direction_ = (velocity_.x > 0.0f) ? FacingDirection::Right : FacingDirection::Left;
        } else {
            direction_ = (velocity_.y > 0.0f) ? FacingDirection::Down : FacingDirection::Up;
        }

        // Normalize diagonal speed
        float len = std::sqrt(velocity_.x * velocity_.x + velocity_.y * velocity_.y);
        if (len > 0.001f) {
            velocity_ = (velocity_ / len) * speed_;
        }
    } else {
        isMoving_ = false;
    }
}

void PlayerEntity::update(float dt, SoundSystem& sound) {
    if (isMoving_) {
        position_ += velocity_ * dt;
        walkTimer_ += dt * 10.0f;
        stepSoundTimer_ += dt;

        // Play footstep sound periodically
        if (stepSoundTimer_ >= 0.32f) {
            sound.play(SoundEffect::Step);
            stepSoundTimer_ = 0.0f;
        }
    } else {
        stepSoundTimer_ = 0.25f;
    }

    // Clamp player within room boundaries
    position_.x = std::max(60.0f, std::min(800.0f, position_.x));
    position_.y = std::max(60.0f, std::min(740.0f, position_.y));
}

void PlayerEntity::render(sf::RenderTarget& target, const InteractiveEntity* nearbyEntity) {
    // 1. Flashlight Beam casting in facing direction
    sf::Vertex beam[4];
    beam[0].position = position_;
    beam[0].color = sf::Color(255, 240, 180, 70);

    float angle = 0.0f;
    if (direction_ == FacingDirection::Down) angle = 90.0f;
    else if (direction_ == FacingDirection::Up) angle = 270.0f;
    else if (direction_ == FacingDirection::Left) angle = 180.0f;
    else angle = 0.0f;

    const float spreadDeg = 35.0f;
    const float dist = 140.0f;
    const float rad1 = (angle - spreadDeg) * 3.14159f / 180.0f;
    const float rad2 = (angle) * 3.14159f / 180.0f;
    const float rad3 = (angle + spreadDeg) * 3.14159f / 180.0f;

    beam[1].position = position_ + sf::Vector2f(std::cos(rad1) * dist, std::sin(rad1) * dist);
    beam[1].color = sf::Color(255, 235, 150, 0);

    beam[2].position = position_ + sf::Vector2f(std::cos(rad2) * dist * 1.15f, std::sin(rad2) * dist * 1.15f);
    beam[2].color = sf::Color(255, 245, 180, 0);

    beam[3].position = position_ + sf::Vector2f(std::cos(rad3) * dist, std::sin(rad3) * dist);
    beam[3].color = sf::Color(255, 235, 150, 0);

    sf::Vertex triangle1[] = { beam[0], beam[1], beam[2] };
    sf::Vertex triangle2[] = { beam[0], beam[2], beam[3] };
    target.draw(triangle1, 3, sf::PrimitiveType::Triangles);
    target.draw(triangle2, 3, sf::PrimitiveType::Triangles);

    // 2. Soft shadow underneath player
    sf::CircleShape shadow(16.0f);
    shadow.setScale({1.0f, 0.45f});
    shadow.setOrigin({16.0f, 16.0f});
    shadow.setPosition({position_.x, position_.y + 16.0f});
    shadow.setFillColor(sf::Color(0, 0, 0, 90));
    target.draw(shadow);

    // Walking animation bob
    float bob = isMoving_ ? std::abs(std::sin(walkTimer_)) * 3.0f : 0.0f;
    sf::Vector2f bodyPos(position_.x, position_.y - bob);

    // 3. Explorer Torso / Coat
    sf::RectangleShape coat(sf::Vector2f(22.0f, 24.0f));
    coat.setOrigin({11.0f, 12.0f});
    coat.setPosition(bodyPos);
    coat.setFillColor(sf::Color(70, 85, 105)); // Explorer teal-slate coat
    coat.setOutlineColor(sf::Color(45, 55, 70));
    coat.setOutlineThickness(2.0f);
    target.draw(coat);

    // Belt and buckle
    sf::RectangleShape belt(sf::Vector2f(22.0f, 4.0f));
    belt.setOrigin({11.0f, 2.0f});
    belt.setPosition({bodyPos.x, bodyPos.y + 4.0f});
    belt.setFillColor(sf::Color(50, 35, 20));
    target.draw(belt);

    sf::RectangleShape buckle(sf::Vector2f(6.0f, 6.0f));
    buckle.setOrigin({3.0f, 3.0f});
    buckle.setPosition({bodyPos.x, bodyPos.y + 4.0f});
    buckle.setFillColor(sf::Color(220, 180, 50));
    target.draw(buckle);

    // Backpack
    if (direction_ == FacingDirection::Down || direction_ == FacingDirection::Left || direction_ == FacingDirection::Right) {
        sf::RectangleShape pack(sf::Vector2f(16.0f, 12.0f));
        pack.setOrigin({8.0f, 6.0f});
        if (direction_ == FacingDirection::Down) {
            pack.setPosition({bodyPos.x, bodyPos.y - 4.0f});
        } else if (direction_ == FacingDirection::Left) {
            pack.setPosition({bodyPos.x + 8.0f, bodyPos.y - 2.0f});
        } else {
            pack.setPosition({bodyPos.x - 8.0f, bodyPos.y - 2.0f});
        }
        pack.setFillColor(sf::Color(100, 65, 40));
        target.draw(pack);
    }

    // 4. Explorer Head & Hat
    sf::CircleShape head(9.0f);
    head.setOrigin({9.0f, 9.0f});
    head.setPosition({bodyPos.x, bodyPos.y - 14.0f});
    head.setFillColor(sf::Color(235, 195, 165)); // skin tone
    target.draw(head);

    // Fedora / Explorer Hat Brim
    sf::CircleShape hatBrim(13.0f);
    hatBrim.setScale({1.2f, 0.7f});
    hatBrim.setOrigin({13.0f, 13.0f});
    hatBrim.setPosition({bodyPos.x, bodyPos.y - 17.0f});
    hatBrim.setFillColor(sf::Color(120, 85, 55));
    target.draw(hatBrim);

    // Hat Crown
    sf::CircleShape hatCrown(8.0f);
    hatCrown.setOrigin({8.0f, 8.0f});
    hatCrown.setPosition({bodyPos.x, bodyPos.y - 21.0f});
    hatCrown.setFillColor(sf::Color(140, 100, 65));
    target.draw(hatCrown);

    // 5. Interaction Bubble if near an object
    if (nearbyEntity) {
        std::string prompt = "[E] Interact: " + nearbyEntity->getName();
        if (nearbyEntity->getType() == EntityType::Door) {
            prompt = "[E] Enter Door: " + nearbyEntity->getName();
        } else if (nearbyEntity->getType() == EntityType::PuzzlePedestal) {
            prompt = "[E] Inspect Puzzle: " + nearbyEntity->getName();
        } else if (nearbyEntity->getType() == EntityType::ItemPickup) {
            prompt = "[E] Pick Up: " + nearbyEntity->getName();
        }

        sf::Text promptText(font_, prompt, 12);
        promptText.setStyle(sf::Text::Bold);
        promptText.setFillColor(sf::Color(255, 255, 255));
        sf::FloatRect tb = promptText.getLocalBounds();
        promptText.setOrigin({tb.size.x * 0.5f, tb.size.y * 0.5f});
        promptText.setPosition({position_.x, position_.y - 48.0f});

        // Pill background
        sf::RectangleShape pill(sf::Vector2f(tb.size.x + 18.0f, 22.0f));
        pill.setOrigin({(tb.size.x + 18.0f) * 0.5f, 11.0f});
        pill.setPosition({position_.x, position_.y - 48.0f});
        pill.setFillColor(sf::Color(20, 24, 35, 230));
        pill.setOutlineColor(sf::Color(255, 215, 60));
        pill.setOutlineThickness(2.0f);

        target.draw(pill);
        target.draw(promptText);
    }
}

sf::Vector2f PlayerEntity::getPosition() const { return position_; }
void PlayerEntity::setPosition(const sf::Vector2f& pos) { position_ = pos; }
sf::FloatRect PlayerEntity::getBounds() const {
    return sf::FloatRect({position_.x - 14.0f, position_.y - 14.0f}, {28.0f, 28.0f});
}
FacingDirection PlayerEntity::getDirection() const { return direction_; }
void PlayerEntity::setDirection(FacingDirection dir) { direction_ = dir; }
