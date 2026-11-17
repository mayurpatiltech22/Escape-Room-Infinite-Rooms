#include "PuzzleModal.h"
#include "Puzzle.h"
#include "SoundSystem.h"
#include "GameManager.h"
#include <cmath>
#include <iostream>

PuzzleModal::PuzzleModal(sf::Font& font)
    : font_(font),
      isOpen_(false),
      currentPuzzle_(nullptr),
      statusColor_(sf::Color::White),
      cursorTimer_(0.0f),
      closeTimer_(0.0f),
      shakeTimer_(0.0f) {}

void PuzzleModal::open(Puzzle* puzzle) {
    if (!puzzle) return;
    currentPuzzle_ = puzzle;
    isOpen_ = true;
    userInput_.clear();
    statusMessage_ = "Enter solution code and submit.";
    statusColor_ = sf::Color(180, 200, 230);
    closeTimer_ = 0.0f;
    shakeTimer_ = 0.0f;
}

void PuzzleModal::close() {
    isOpen_ = false;
    currentPuzzle_ = nullptr;
    userInput_.clear();
}

bool PuzzleModal::isOpen() const {
    return isOpen_;
}

void PuzzleModal::handleTextEntered(char32_t unicode) {
    if (!isOpen_) return;

    if (unicode == 8) { // Backspace
        if (!userInput_.empty()) {
            userInput_.pop_back();
        }
    } else if (unicode >= 32 && unicode < 127 && userInput_.size() < 28) {
        userInput_ += static_cast<char>(unicode);
    }
}

void PuzzleModal::handleKeyPressed(sf::Keyboard::Key key, GameManager& game, SoundSystem& sound) {
    if (!isOpen_) return;

    if (key == sf::Keyboard::Key::Enter) {
        submitAnswer(game, sound);
    } else if (key == sf::Keyboard::Key::Escape) {
        close();
    } else if (key == sf::Keyboard::Key::H) {
        if (currentPuzzle_ && currentPuzzle_->hasMoreHints()) {
            std::string hint = currentPuzzle_->getNextHint();
            if (!hint.empty()) {
                statusMessage_ = "HINT: " + hint;
                statusColor_ = sf::Color(255, 215, 100);
            }
        } else if (currentPuzzle_) {
            statusMessage_ = "No more hints available.";
            statusColor_ = sf::Color(180, 180, 180);
        }
    }
}

void PuzzleModal::handleMouseClick(const sf::Vector2i& mousePos, GameManager& game, SoundSystem& sound) {
    if (!isOpen_) return;

    // Submit button area
    sf::FloatRect submitBtn({510.0f, 510.0f}, {160.0f, 40.0f});
    if (submitBtn.contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
        submitAnswer(game, sound);
        return;
    }

    // Cancel button area
    sf::FloatRect cancelBtn({690.0f, 510.0f}, {140.0f, 40.0f});
    if (cancelBtn.contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
        close();
        return;
    }

    // Hint button area
    sf::FloatRect hintBtn({340.0f, 510.0f}, {150.0f, 40.0f});
    if (hintBtn.contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
        if (currentPuzzle_ && currentPuzzle_->hasMoreHints()) {
            std::string hint = currentPuzzle_->getNextHint();
            if (!hint.empty()) {
                statusMessage_ = "HINT: " + hint;
                statusColor_ = sf::Color(255, 215, 100);
            }
        } else if (currentPuzzle_) {
            statusMessage_ = "No more hints available.";
            statusColor_ = sf::Color(180, 180, 180);
        }
        return;
    }
}

void PuzzleModal::submitAnswer(GameManager& game, SoundSystem& sound) {
    if (!currentPuzzle_) return;

    if (currentPuzzle_->isSolved()) {
        statusMessage_ = "Puzzle is already solved!";
        statusColor_ = sf::Color(100, 255, 150);
        return;
    }

    if (userInput_.empty()) {
        statusMessage_ = "Please enter an answer.";
        statusColor_ = sf::Color(255, 200, 80);
        return;
    }

    bool correct = currentPuzzle_->submitAnswer(userInput_);
    if (correct) {
        sound.play(SoundEffect::PuzzleSuccess);
        game.addScore(currentPuzzle_->getPoints());

        // Also unlock any doors connected to this puzzle
        for (auto& pair : game.getDoors()) {
            if (pair.second.getRequiredPuzzleId() == currentPuzzle_->getId()) {
                pair.second.tryUnlock(*currentPuzzle_);
            }
        }

        statusMessage_ = "ACCESS GRANTED! Correct answer. (+" + std::to_string(currentPuzzle_->getPoints()) + " pts)";
        statusColor_ = sf::Color(60, 255, 130);
        closeTimer_ = 1.2f;
    } else {
        sound.play(SoundEffect::PuzzleFail);
        statusMessage_ = "ACCESS DENIED! Incorrect answer. Try again.";
        statusColor_ = sf::Color(255, 70, 70);
        shakeTimer_ = 0.35f;
    }
}

void PuzzleModal::update(float dt) {
    if (!isOpen_) return;

    cursorTimer_ += dt;

    if (shakeTimer_ > 0.0f) {
        shakeTimer_ -= dt;
    }

    if (closeTimer_ > 0.0f) {
        closeTimer_ -= dt;
        if (closeTimer_ <= 0.0f) {
            close();
        }
    }
}

void PuzzleModal::render(sf::RenderTarget& target) {
    if (!isOpen_ || !currentPuzzle_) return;

    // 1. Dark overlay over whole window
    sf::RectangleShape overlay(sf::Vector2f(1200.0f, 800.0f));
    overlay.setFillColor(sf::Color(0, 0, 0, 195));
    target.draw(overlay);

    // Shake offset
    float shakeX = (shakeTimer_ > 0.0f) ? std::sin(shakeTimer_ * 40.0f) * 8.0f : 0.0f;

    // 2. Modal Box
    const float modalW = 560.0f;
    const float modalH = 380.0f;
    const float modalX = (1200.0f - modalW) * 0.5f + shakeX;
    const float modalY = (800.0f - modalH) * 0.5f;

    sf::RectangleShape modal(sf::Vector2f(modalW, modalH));
    modal.setPosition({modalX, modalY});
    modal.setFillColor(sf::Color(20, 24, 36, 250));
    modal.setOutlineColor(currentPuzzle_->isSolved() ? sf::Color(50, 220, 120) : sf::Color(0, 210, 255));
    modal.setOutlineThickness(3.0f);
    target.draw(modal);

    // Modal Header Banner
    sf::RectangleShape headerBar(sf::Vector2f(modalW, 46.0f));
    headerBar.setPosition({modalX, modalY});
    headerBar.setFillColor(sf::Color(30, 36, 54));
    target.draw(headerBar);

    std::string headerStr = "SECURITY TERMINAL // " + currentPuzzle_->getTitle();
    sf::Text headerText(font_, headerStr, 14);
    headerText.setStyle(sf::Text::Bold);
    headerText.setFillColor(sf::Color(0, 235, 255));
    headerText.setPosition({modalX + 20.0f, modalY + 12.0f});
    target.draw(headerText);

    // Reward points badge
    std::string rewardStr = "Reward: +" + std::to_string(currentPuzzle_->getPoints()) + " pts";
    sf::Text rewardText(font_, rewardStr, 12);
    rewardText.setStyle(sf::Text::Bold);
    rewardText.setFillColor(sf::Color(255, 215, 60));
    rewardText.setPosition({modalX + modalW - 160.0f, modalY + 14.0f});
    target.draw(rewardText);

    // Difficulty badge
    std::string diffStr = "Difficulty: ";
    switch (currentPuzzle_->getDifficulty()) {
        case Difficulty::EASY: diffStr += "Easy"; break;
        case Difficulty::MEDIUM: diffStr += "Medium"; break;
        case Difficulty::HARD: diffStr += "Hard"; break;
        case Difficulty::EXPERT: diffStr += "Expert"; break;
    }
    sf::Text diffText(font_, diffStr, 11);
    diffText.setFillColor(sf::Color(180, 200, 230));
    diffText.setPosition({modalX + 20.0f, modalY + 34.0f});
    target.draw(diffText);

    // Attempts counter
    if (!currentPuzzle_->isSolved() && currentPuzzle_->getMaxAttempts() > 0) {
        std::string attemptsStr = "Attempts: " + std::to_string(currentPuzzle_->getAttempts()) +
                                   " / " + std::to_string(currentPuzzle_->getMaxAttempts());
        sf::Text attemptsText(font_, attemptsStr, 11);
        attemptsText.setFillColor(currentPuzzle_->getAttempts() >= currentPuzzle_->getMaxAttempts() ?
                                   sf::Color(255, 100, 100) : sf::Color(180, 200, 230));
        attemptsText.setPosition({modalX + modalW - 160.0f, modalY + 34.0f});
        target.draw(attemptsText);
    }

    // Question Box Container
    sf::RectangleShape qBox(sf::Vector2f(modalW - 40.0f, 110.0f));
    qBox.setPosition({modalX + 20.0f, modalY + 65.0f});
    qBox.setFillColor(sf::Color(14, 16, 24));
    qBox.setOutlineColor(sf::Color(45, 55, 75));
    qBox.setOutlineThickness(1.5f);
    target.draw(qBox);

    sf::Text qPrompt(font_, "PUZZLE QUERY:", 11);
    qPrompt.setStyle(sf::Text::Bold);
    qPrompt.setFillColor(sf::Color(160, 180, 210));
    qPrompt.setPosition({modalX + 32.0f, modalY + 75.0f});
    target.draw(qPrompt);

    sf::Text qText(font_, currentPuzzle_->getDescription(), 14);
    qText.setFillColor(sf::Color(245, 245, 255));
    qText.setPosition({modalX + 32.0f, modalY + 105.0f});
    target.draw(qText);

    // Text Input Container
    sf::RectangleShape inputField(sf::Vector2f(modalW - 40.0f, 44.0f));
    inputField.setPosition({modalX + 20.0f, modalY + 195.0f});
    inputField.setFillColor(sf::Color(12, 14, 22));
    inputField.setOutlineColor(sf::Color(0, 180, 240));
    inputField.setOutlineThickness(2.0f);
    target.draw(inputField);

    // Text inside input field
    std::string displayInput = userInput_;
    if (std::fmod(cursorTimer_, 0.8f) < 0.4f) {
        displayInput += "|";
    }

    sf::Text inputText(font_, displayInput.empty() ? "Type your answer..." : displayInput, 15);
    inputText.setFillColor(displayInput.empty() ? sf::Color(100, 120, 145) : sf::Color::White);
    inputText.setPosition({modalX + 32.0f, modalY + 206.0f});
    target.draw(inputText);

    // Status / Feedback message
    sf::Text status(font_, statusMessage_, 12);
    status.setStyle(sf::Text::Bold);
    status.setFillColor(statusColor_);
    status.setPosition({modalX + 24.0f, modalY + 255.0f});
    target.draw(status);

    // 3. Action Buttons
    // Hint Button
    if (currentPuzzle_->hasMoreHints()) {
        sf::RectangleShape hintBtn(sf::Vector2f(150.0f, 40.0f));
        hintBtn.setPosition({modalX + modalW - 490.0f, modalY + 300.0f});
        hintBtn.setFillColor(sf::Color(200, 150, 50));
        hintBtn.setOutlineColor(sf::Color(255, 200, 100));
        hintBtn.setOutlineThickness(2.0f);
        target.draw(hintBtn);

        sf::Text hintTxt(font_, "HINT [H]", 13);
        hintTxt.setStyle(sf::Text::Bold);
        hintTxt.setFillColor(sf::Color::White);
        hintTxt.setPosition({modalX + modalW - 475.0f, modalY + 310.0f});
        target.draw(hintTxt);
    }

    // Submit Button
    sf::RectangleShape submitBtn(sf::Vector2f(160.0f, 40.0f));
    submitBtn.setPosition({modalX + modalW - 330.0f, modalY + 300.0f});
    submitBtn.setFillColor(sf::Color(0, 170, 100));
    submitBtn.setOutlineColor(sf::Color(60, 255, 160));
    submitBtn.setOutlineThickness(2.0f);
    target.draw(submitBtn);

    sf::Text submitTxt(font_, "SUBMIT [Enter]", 13);
    submitTxt.setStyle(sf::Text::Bold);
    submitTxt.setFillColor(sf::Color::White);
    submitTxt.setPosition({modalX + modalW - 315.0f, modalY + 310.0f});
    target.draw(submitTxt);

    // Close Button
    sf::RectangleShape closeBtn(sf::Vector2f(130.0f, 40.0f));
    closeBtn.setPosition({modalX + modalW - 150.0f, modalY + 300.0f});
    closeBtn.setFillColor(sf::Color(45, 50, 65));
    closeBtn.setOutlineColor(sf::Color(90, 100, 130));
    closeBtn.setOutlineThickness(1.5f);
    target.draw(closeBtn);

    sf::Text closeTxt(font_, "CLOSE [Esc]", 13);
    closeTxt.setFillColor(sf::Color(200, 210, 230));
    closeTxt.setPosition({modalX + modalW - 138.0f, modalY + 310.0f});
    target.draw(closeTxt);
}
