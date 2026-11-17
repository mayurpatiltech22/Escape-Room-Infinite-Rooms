#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class InteractiveEntity;
class SoundSystem;

enum class FacingDirection {
    Up,
    Down,
    Left,
    Right
};

class PlayerEntity {
private:
    sf::Vector2f position_;
    sf::Vector2f velocity_;
    FacingDirection direction_;
    float speed_;
    float walkTimer_;
    float stepSoundTimer_;
    bool isMoving_;
    sf::Font& font_;

public:
    PlayerEntity(const sf::Vector2f& startPos, sf::Font& font);
    ~PlayerEntity() = default;

    void handleInput(float dt);
    void update(float dt, SoundSystem& sound);
    void render(sf::RenderTarget& target, const InteractiveEntity* nearbyEntity);

    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);
    sf::FloatRect getBounds() const;
    FacingDirection getDirection() const;
    void setDirection(FacingDirection dir);
};

#endif // PLAYER_ENTITY_H
