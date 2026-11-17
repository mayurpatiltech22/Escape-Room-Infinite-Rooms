#ifndef HUD_H
#define HUD_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <deque>

class GameManager;

struct FloatingCombatText {
    std::string text;
    sf::Vector2f position;
    sf::Color color;
    float lifetime;
    float maxLifetime;
};

class HUD {
private:
    sf::Font& font_;
    float animTimer_;
    float displayedHealth_;
    bool showBFSPath_;
    std::deque<std::string> logMessages_;
    std::vector<FloatingCombatText> floatingTexts_;

public:
    explicit HUD(sf::Font& font);
    ~HUD() = default;

    void update(float dt, int targetHealth);
    void render(sf::RenderTarget& target, const GameManager& game);

    void addLog(const std::string& message);
    void addFloatingText(const std::string& text, const sf::Vector2f& pos, const sf::Color& color = sf::Color(255, 230, 80));

    void toggleBFSPath();
    bool isBFSPathEnabled() const;
};

#endif // HUD_H
