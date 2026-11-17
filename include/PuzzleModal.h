#ifndef PUZZLE_MODAL_H
#define PUZZLE_MODAL_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class Puzzle;
class SoundSystem;
class GameManager;

class PuzzleModal {
private:
    sf::Font& font_;
    bool isOpen_;
    Puzzle* currentPuzzle_;
    std::string userInput_;
    std::string statusMessage_;
    sf::Color statusColor_;
    float cursorTimer_;
    float closeTimer_;
    float shakeTimer_;

public:
    explicit PuzzleModal(sf::Font& font);
    ~PuzzleModal() = default;

    void open(Puzzle* puzzle);
    void close();
    bool isOpen() const;

    void handleTextEntered(char32_t unicode);
    void handleKeyPressed(sf::Keyboard::Key key, GameManager& game, SoundSystem& sound);
    void handleMouseClick(const sf::Vector2i& mousePos, GameManager& game, SoundSystem& sound);

    void update(float dt);
    void render(sf::RenderTarget& target);

private:
    void submitAnswer(GameManager& game, SoundSystem& sound);
};

#endif // PUZZLE_MODAL_H
