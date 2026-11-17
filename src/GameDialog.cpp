#include "GameDialog.h"
#include "GameManager.h"
#include "SoundSystem.h"
#include "HUD.h"
#include <cmath>
#include <iostream>

GameDialog::GameDialog(sf::Font& font)
    : font_(font),
      state_(DialogState::None),
      feedbackColor_(sf::Color::White),
      animTimer_(0.0f) {}

void GameDialog::openPauseMenu() {
    state_ = DialogState::PauseMenu;
    feedbackMessage_ = "Select an operation:";
    feedbackColor_ = sf::Color(190, 210, 240);
}

void GameDialog::openVictory() {
    state_ = DialogState::Victory;
    feedbackMessage_ = "All puzzles resolved! Security breached!";
    feedbackColor_ = sf::Color(255, 215, 60);
}

void GameDialog::openGameOver() {
    state_ = DialogState::GameOver;
    feedbackMessage_ = "Vitals expired. Facility sealed.";
    feedbackColor_ = sf::Color(255, 60, 60);
}

void GameDialog::close() {
    state_ = DialogState::None;
}

bool GameDialog::isOpen() const {
    return state_ != DialogState::None;
}

DialogState GameDialog::getState() const {
    return state_;
}

void GameDialog::update(float dt) {
    animTimer_ += dt;
}

void GameDialog::handleKeyPressed(sf::Keyboard::Key key, GameManager& game, SoundSystem& sound, HUD& hud, sf::RenderWindow& window) {
    if (state_ == DialogState::PauseMenu) {
        if (key == sf::Keyboard::Key::Num1 || key == sf::Keyboard::Key::Escape) {
            close();
        } else if (key == sf::Keyboard::Key::Num2 || key == sf::Keyboard::Key::F5) {
            // Quick Save
            try {
                game.saveGame("quicksave.sav");
                feedbackMessage_ = "Game saved successfully to quicksave.sav!";
                feedbackColor_ = sf::Color(60, 255, 120);
                sound.play(SoundEffect::Interact);
                hud.addLog("QuickSave created: quicksave.sav");
            } catch (const std::exception& e) {
                feedbackMessage_ = std::string("Save failed: ") + e.what();
                feedbackColor_ = sf::Color(255, 70, 70);
            }
        } else if (key == sf::Keyboard::Key::Num3 || key == sf::Keyboard::Key::F9) {
            // Quick Load
            try {
                game.loadGame("quicksave.sav");
                feedbackMessage_ = "Game loaded from quicksave.sav!";
                feedbackColor_ = sf::Color(60, 255, 120);
                sound.play(SoundEffect::Interact);
                hud.addLog("Loaded game from quicksave.sav");
                close();
            } catch (const std::exception& e) {
                feedbackMessage_ = std::string("Load failed: ") + e.what();
                feedbackColor_ = sf::Color(255, 70, 70);
            }
        } else if (key == sf::Keyboard::Key::Num4) {
            // Toggle Sound
            sound.setEnabled(!sound.isEnabled());
            feedbackMessage_ = sound.isEnabled() ? "Sound Effects: ENABLED" : "Sound Effects: MUTED";
            feedbackColor_ = sound.isEnabled() ? sf::Color(100, 255, 150) : sf::Color(200, 200, 200);
        } else if (key == sf::Keyboard::Key::Num5) {
            window.close();
        }
    } else if (state_ == DialogState::Victory) {
        if (key == sf::Keyboard::Key::Escape || key == sf::Keyboard::Key::Enter) {
            window.close();
        }
    } else if (state_ == DialogState::GameOver) {
        if (key == sf::Keyboard::Key::F9) {
            try {
                game.loadGame("quicksave.sav");
                hud.addLog("Restored from quicksave.sav");
                close();
            } catch (...) {
                feedbackMessage_ = "No quicksave.sav found. Exiting.";
            }
        } else if (key == sf::Keyboard::Key::Escape) {
            window.close();
        }
    }
}

void GameDialog::handleMouseClick(const sf::Vector2i& mousePos, GameManager& game, SoundSystem& sound, HUD& hud, sf::RenderWindow& window) {
    if (state_ == DialogState::PauseMenu) {
        float mx = static_cast<float>(mousePos.x);
        float my = static_cast<float>(mousePos.y);

        auto checkBtn = [&](float y) {
            return (mx >= 440.0f && mx <= 760.0f && my >= y && my <= y + 42.0f);
        };

        if (checkBtn(240.0f)) { // 1. Resume
            close();
        } else if (checkBtn(295.0f)) { // 2. Quick Save
            try {
                game.saveGame("quicksave.sav");
                feedbackMessage_ = "Saved to quicksave.sav!";
                feedbackColor_ = sf::Color(60, 255, 120);
                sound.play(SoundEffect::Interact);
                hud.addLog("QuickSave created");
            } catch (const std::exception& e) {
                feedbackMessage_ = e.what();
                feedbackColor_ = sf::Color(255, 70, 70);
            }
        } else if (checkBtn(350.0f)) { // 3. Quick Load
            try {
                game.loadGame("quicksave.sav");
                feedbackMessage_ = "Loaded from quicksave.sav!";
                feedbackColor_ = sf::Color(60, 255, 120);
                sound.play(SoundEffect::Interact);
                hud.addLog("Loaded game state");
                close();
            } catch (const std::exception& e) {
                feedbackMessage_ = e.what();
                feedbackColor_ = sf::Color(255, 70, 70);
            }
        } else if (checkBtn(405.0f)) { // 4. Toggle Audio
            sound.setEnabled(!sound.isEnabled());
            feedbackMessage_ = sound.isEnabled() ? "Sound Effects: ENABLED" : "Sound Effects: MUTED";
            feedbackColor_ = sound.isEnabled() ? sf::Color(100, 255, 150) : sf::Color(200, 200, 200);
        } else if (checkBtn(460.0f)) { // 5. Exit
            window.close();
        }
    } else if (state_ == DialogState::Victory || state_ == DialogState::GameOver) {
        window.close();
    }
}

void GameDialog::render(sf::RenderTarget& target, const GameManager& game) {
    if (state_ == DialogState::None) return;

    // Dark backdrop overlay
    sf::RectangleShape backdrop(sf::Vector2f(1200.0f, 800.0f));
    backdrop.setFillColor(sf::Color(0, 0, 0, 210));
    target.draw(backdrop);

    if (state_ == DialogState::PauseMenu) {
        const float boxW = 440.0f;
        const float boxH = 430.0f;
        const float boxX = (1200.0f - boxW) * 0.5f;
        const float boxY = (800.0f - boxH) * 0.5f;

        sf::RectangleShape box(sf::Vector2f(boxW, boxH));
        box.setPosition({boxX, boxY});
        box.setFillColor(sf::Color(20, 24, 36, 250));
        box.setOutlineColor(sf::Color(0, 200, 240));
        box.setOutlineThickness(3.0f);
        target.draw(box);

        // Header
        sf::Text header(font_, "SYSTEM PAUSED // CONTROL MENU", 14);
        header.setStyle(sf::Text::Bold);
        header.setFillColor(sf::Color(0, 230, 255));
        header.setPosition({boxX + 24.0f, boxY + 18.0f});
        target.draw(header);

        // Feedback label
        sf::Text feedback(font_, feedbackMessage_, 11);
        feedback.setStyle(sf::Text::Bold);
        feedback.setFillColor(feedbackColor_);
        feedback.setPosition({boxX + 24.0f, boxY + 44.0f});
        target.draw(feedback);

        // Menu Buttons
        struct Btn {
            std::string label;
            float y;
        };
        Btn buttons[] = {
            {"[1] Resume Exploration", 240.0f},
            {"[2] Quick Save Game (F5)", 295.0f},
            {"[3] Quick Load Game (F9)", 350.0f},
            {"[4] Toggle Sound Effects", 405.0f},
            {"[5] Exit Application", 460.0f}
        };

        for (const auto& b : buttons) {
            sf::RectangleShape btn(sf::Vector2f(340.0f, 40.0f));
            btn.setPosition({boxX + 50.0f, b.y});
            btn.setFillColor(sf::Color(32, 38, 54));
            btn.setOutlineColor(sf::Color(65, 80, 110));
            btn.setOutlineThickness(1.5f);
            target.draw(btn);

            sf::Text btext(font_, b.label, 13);
            btext.setStyle(sf::Text::Bold);
            btext.setFillColor(sf::Color(240, 245, 255));
            btext.setPosition({boxX + 70.0f, b.y + 10.0f});
            target.draw(btext);
        }

    } else if (state_ == DialogState::Victory) {
        // Victory Panel
        const float boxW = 580.0f;
        const float boxH = 400.0f;
        const float boxX = (1200.0f - boxW) * 0.5f;
        const float boxY = (800.0f - boxH) * 0.5f;

        sf::RectangleShape box(sf::Vector2f(boxW, boxH));
        box.setPosition({boxX, boxY});
        box.setFillColor(sf::Color(25, 22, 16, 252));
        box.setOutlineColor(sf::Color(255, 215, 0));
        box.setOutlineThickness(4.0f);
        target.draw(box);

        // Banner
        sf::Text vTitle(font_, "MISSION ACCOMPLISHED!", 24);
        vTitle.setStyle(sf::Text::Bold);
        vTitle.setFillColor(sf::Color(255, 225, 60));
        sf::FloatRect tb = vTitle.getLocalBounds();
        vTitle.setOrigin({tb.size.x * 0.5f, tb.size.y * 0.5f});
        vTitle.setPosition({600.0f, boxY + 45.0f});
        target.draw(vTitle);

        sf::Text vSub(font_, "YOU SUCCESSFULLY ESCAPED THE INFINITE FACILITY", 12);
        vSub.setStyle(sf::Text::Bold);
        vSub.setFillColor(sf::Color(240, 240, 200));
        sf::FloatRect sb = vSub.getLocalBounds();
        vSub.setOrigin({sb.size.x * 0.5f, sb.size.y * 0.5f});
        vSub.setPosition({600.0f, boxY + 80.0f});
        target.draw(vSub);

        // Stats Card
        sf::RectangleShape statsBox(sf::Vector2f(boxW - 60.0f, 160.0f));
        statsBox.setPosition({boxX + 30.0f, boxY + 110.0f});
        statsBox.setFillColor(sf::Color(18, 16, 12));
        statsBox.setOutlineColor(sf::Color(180, 150, 60));
        statsBox.setOutlineThickness(1.5f);
        target.draw(statsBox);

        auto drawStat = [&](const std::string& label, const std::string& val, float y) {
            sf::Text l(font_, label, 13);
            l.setFillColor(sf::Color(200, 190, 160));
            l.setPosition({boxX + 50.0f, y});
            target.draw(l);

            sf::Text v(font_, val, 14);
            v.setStyle(sf::Text::Bold);
            v.setFillColor(sf::Color(255, 230, 80));
            v.setPosition({boxX + 380.0f, y});
            target.draw(v);
        };

        drawStat("Final Mission Score:", std::to_string(game.getScore()) + " Points", boxY + 125.0f);
        drawStat("Total Chamber Moves:", std::to_string(game.getMovesCount()), boxY + 155.0f);
        drawStat("Remaining Explorer Health:", std::to_string(game.getPlayer().getHealth()) + " HP", boxY + 185.0f);
        drawStat("Facility Escape Status:", "CLEARED / 100% SECURE", boxY + 215.0f);

        // Exit button
        sf::RectangleShape okBtn(sf::Vector2f(220.0f, 44.0f));
        okBtn.setPosition({boxX + (boxW - 220.0f) * 0.5f, boxY + 310.0f});
        okBtn.setFillColor(sf::Color(180, 130, 20));
        okBtn.setOutlineColor(sf::Color(255, 230, 80));
        okBtn.setOutlineThickness(2.0f);
        target.draw(okBtn);

        sf::Text okTxt(font_, "CONCLUDE [Enter/Esc]", 13);
        okTxt.setStyle(sf::Text::Bold);
        okTxt.setFillColor(sf::Color::White);
        sf::FloatRect ob = okTxt.getLocalBounds();
        okTxt.setOrigin({ob.size.x * 0.5f, ob.size.y * 0.5f});
        okTxt.setPosition({600.0f, boxY + 330.0f});
        target.draw(okTxt);

    } else if (state_ == DialogState::GameOver) {
        // Game Over Panel
        const float boxW = 500.0f;
        const float boxH = 320.0f;
        const float boxX = (1200.0f - boxW) * 0.5f;
        const float boxY = (800.0f - boxH) * 0.5f;

        sf::RectangleShape box(sf::Vector2f(boxW, boxH));
        box.setPosition({boxX, boxY});
        box.setFillColor(sf::Color(30, 16, 18, 252));
        box.setOutlineColor(sf::Color(230, 45, 55));
        box.setOutlineThickness(4.0f);
        target.draw(box);

        sf::Text goTitle(font_, "CRITICAL VITALS ZERO", 22);
        goTitle.setStyle(sf::Text::Bold);
        goTitle.setFillColor(sf::Color(255, 60, 70));
        sf::FloatRect tb = goTitle.getLocalBounds();
        goTitle.setOrigin({tb.size.x * 0.5f, tb.size.y * 0.5f});
        goTitle.setPosition({600.0f, boxY + 45.0f});
        target.draw(goTitle);

        sf::Text goSub(font_, "THE EXPLORER SUCCUMBED TO FACILITY TRAPS", 12);
        goSub.setStyle(sf::Text::Bold);
        goSub.setFillColor(sf::Color(220, 160, 160));
        sf::FloatRect sb = goSub.getLocalBounds();
        goSub.setOrigin({sb.size.x * 0.5f, sb.size.y * 0.5f});
        goSub.setPosition({600.0f, boxY + 80.0f});
        target.draw(goSub);

        sf::Text goPrompt(font_, "[F9] Load Last QuickSave    |    [ESC] Quit to Desktop", 13);
        goPrompt.setStyle(sf::Text::Bold);
        goPrompt.setFillColor(sf::Color(255, 230, 230));
        sf::FloatRect pb = goPrompt.getLocalBounds();
        goPrompt.setOrigin({pb.size.x * 0.5f, pb.size.y * 0.5f});
        goPrompt.setPosition({600.0f, boxY + 160.0f});
        target.draw(goPrompt);
    }
}
