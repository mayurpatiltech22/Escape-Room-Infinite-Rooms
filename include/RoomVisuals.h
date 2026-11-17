#ifndef ROOM_VISUALS_H
#define ROOM_VISUALS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float size;
    float lifetime;
    float maxLifetime;
};

class RoomVisuals {
private:
    sf::Font& font_;
    std::vector<Particle> particles_;
    float animationTimer_;

    void renderFloor(sf::RenderTarget& target, int roomId);
    void renderWalls(sf::RenderTarget& target);
    void renderProps(sf::RenderTarget& target, int roomId);
    void renderLighting(sf::RenderTarget& target, const sf::Vector2f& playerPos);

public:
    explicit RoomVisuals(sf::Font& font);
    ~RoomVisuals() = default;

    void update(float dt, int roomId);
    void render(sf::RenderTarget& target, int roomId, const sf::Vector2f& playerPos);

    // Collision check against props/walls for player
    bool checkWallCollision(const sf::FloatRect& bounds) const;
};

#endif // ROOM_VISUALS_H
