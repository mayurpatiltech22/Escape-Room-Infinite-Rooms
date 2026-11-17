#ifndef GAME_DIALOG_H
#define GAME_DIALOG_H

#include <SFML/Graphics.hpp>
#include <string>

class GameManager;
class SoundSystem;
class HUD;

enum class DialogState {
    None,
    PauseMenu,
    Victory,
    GameOver
};

class GameDialog {
private:
    sf::Font& font_;
    DialogState state_;
    std::string feedbackMessage_;
    sf::Color feedbackColor_;
    float animTimer_;

public:
    explicit GameDialog(sf::Font& font);
    ~GameDialog() = default;

    void openPauseMenu();
    void openVictory();
    void openGameOver();
    void close();

    bool isOpen() const;
    DialogState getState() const;

    void update(float dt);
    void handleKeyPressed(sf::Keyboard::Key key, GameManager& game, SoundSystem& sound, HUD& hud, sf::RenderWindow& window);
    void handleMouseClick(const sf::Vector2i& mousePos, GameManager& game, SoundSystem& sound, HUD& hud, sf::RenderWindow& window);
    void render(sf::RenderTarget& target, const GameManager& game);
};

#endif // GAME_DIALOG_H
