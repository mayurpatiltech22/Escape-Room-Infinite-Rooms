#include "HUD.h"
#include "GameManager.h"
#include <cmath>
#include <algorithm>
#include <sstream>

HUD::HUD(sf::Font& font)
    : font_(font),
      animTimer_(0.0f),
      displayedHealth_(100.0f),
      showBFSPath_(true) {
    logMessages_.push_back("System online. Facility navigation initialized.");
    logMessages_.push_back("Goal: Solve puzzles and reach the Escape Chamber.");
}

void HUD::update(float dt, int targetHealth) {
    animTimer_ += dt;

    // Smooth health bar interpolation
    float target = static_cast<float>(targetHealth);
    displayedHealth_ += (target - displayedHealth_) * dt * 6.0f;

    // Update floating texts
    for (auto it = floatingTexts_.begin(); it != floatingTexts_.end();) {
        it->position.y -= dt * 35.0f;
        it->lifetime += dt;

        if (it->lifetime >= it->maxLifetime) {
            it = floatingTexts_.erase(it);
        } else {
            ++it;
        }
    }
}

void HUD::addLog(const std::string& message) {
    logMessages_.push_back(message);
    if (logMessages_.size() > 5) {
        logMessages_.pop_front();
    }
}

void HUD::addFloatingText(const std::string& text, const sf::Vector2f& pos, const sf::Color& color) {
    FloatingCombatText ft;
    ft.text = text;
    ft.position = pos;
    ft.color = color;
    ft.lifetime = 0.0f;
    ft.maxLifetime = 1.4f;
    floatingTexts_.push_back(ft);
}

void HUD::toggleBFSPath() {
    showBFSPath_ = !showBFSPath_;
}

bool HUD::isBFSPathEnabled() const {
    return showBFSPath_;
}

void HUD::render(sf::RenderTarget& target, const GameManager& game) {
    const float panelX = 860.0f;
    const float panelW = 340.0f;
    const float panelH = 800.0f;

    // 1. HUD Background Panel
    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition({panelX, 0.0f});
    panel.setFillColor(sf::Color(16, 18, 26, 252));
    target.draw(panel);

    // Glowing Left Divider Line
    sf::RectangleShape divider(sf::Vector2f(3.0f, panelH));
    divider.setPosition({panelX, 0.0f});
    divider.setFillColor(sf::Color(0, 190, 220, 180));
    target.draw(divider);

    // 2. Title & Header
    sf::Text title(font_, "ESCAPE ROOM", 18);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color(0, 230, 255));
    title.setPosition({panelX + 20.0f, 16.0f});
    target.draw(title);

    sf::Text subtitle(font_, "INFINITE ROOMS // FACILITY HUD", 10);
    subtitle.setFillColor(sf::Color(140, 160, 190));
    subtitle.setPosition({panelX + 20.0f, 40.0f});
    target.draw(subtitle);

    sf::RectangleShape sep1(sf::Vector2f(panelW - 40.0f, 1.0f));
    sep1.setPosition({panelX + 20.0f, 58.0f});
    sep1.setFillColor(sf::Color(45, 55, 75));
    target.draw(sep1);

    // 3. Player Status Card
    const auto& player = game.getPlayer();
    int currentHealth = player.getHealth();

    sf::Text statusHeader(font_, "EXPLORER DIAGNOSTICS", 11);
    statusHeader.setStyle(sf::Text::Bold);
    statusHeader.setFillColor(sf::Color(255, 215, 0));
    statusHeader.setPosition({panelX + 20.0f, 68.0f});
    target.draw(statusHeader);

    // Health Bar Frame
    const float barW = 290.0f;
    const float barH = 18.0f;
    sf::RectangleShape hpBg(sf::Vector2f(barW, barH));
    hpBg.setPosition({panelX + 25.0f, 90.0f});
    hpBg.setFillColor(sf::Color(35, 30, 35));
    hpBg.setOutlineColor(sf::Color(70, 60, 75));
    hpBg.setOutlineThickness(1.5f);
    target.draw(hpBg);

    // Health Fill Bar
    float hpPercent = std::max(0.0f, std::min(1.0f, displayedHealth_ / 100.0f));
    if (hpPercent > 0.001f) {
        sf::RectangleShape hpFill(sf::Vector2f(barW * hpPercent, barH));
        hpFill.setPosition({panelX + 25.0f, 90.0f});

        if (currentHealth > 65) {
            hpFill.setFillColor(sf::Color(45, 215, 110)); // Green
        } else if (currentHealth > 35) {
            hpFill.setFillColor(sf::Color(245, 175, 40)); // Amber
        } else {
            hpFill.setFillColor(sf::Color(235, 45, 55)); // Red
        }
        target.draw(hpFill);
    }

    // Health Label Overlay
    std::string hpStr = "HP: " + std::to_string(currentHealth) + " / 100";
    if (currentHealth > 65) hpStr += "  [EXCELLENT]";
    else if (currentHealth > 35) hpStr += "  [STABLE]";
    else if (currentHealth > 0) hpStr += "  [CRITICAL]";
    else hpStr += "  [UNCONSCIOUS]";

    sf::Text hpText(font_, hpStr, 10);
    hpText.setStyle(sf::Text::Bold);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition({panelX + 32.0f, 92.0f});
    target.draw(hpText);

    // Score & Moves Counters
    sf::Text scoreText(font_, "Score: " + std::to_string(game.getScore()) + " pts", 13);
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setFillColor(sf::Color(255, 220, 80));
    scoreText.setPosition({panelX + 25.0f, 116.0f});
    target.draw(scoreText);

    sf::Text movesText(font_, "Moves: " + std::to_string(game.getMovesCount()), 13);
    movesText.setStyle(sf::Text::Bold);
    movesText.setFillColor(sf::Color(140, 200, 255));
    movesText.setPosition({panelX + 180.0f, 116.0f});
    target.draw(movesText);

    sf::RectangleShape sep2(sf::Vector2f(panelW - 40.0f, 1.0f));
    sep2.setPosition({panelX + 20.0f, 140.0f});
    sep2.setFillColor(sf::Color(45, 55, 75));
    target.draw(sep2);

    // 4. Current Chamber Info
    int curRoomId = game.getCurrentRoomId();
    Room* curRoom = game.getRoom(curRoomId);

    std::string roomTitle = "CHAMBER " + std::to_string(curRoomId);
    if (curRoom) {
        roomTitle += ": " + curRoom->getName();
    }
    sf::Text chamberHeader(font_, roomTitle, 13);
    chamberHeader.setStyle(sf::Text::Bold);
    chamberHeader.setFillColor(sf::Color(255, 230, 160));
    chamberHeader.setPosition({panelX + 20.0f, 150.0f});
    target.draw(chamberHeader);

    bool completed = game.isRoomCompleted(curRoomId);
    sf::Text compBadge(font_, completed ? "[STATUS: CHAMBER CLEARED]" : "[STATUS: UNRESOLVED PUZZLES]", 10);
    compBadge.setFillColor(completed ? sf::Color(80, 255, 140) : sf::Color(255, 170, 70));
    compBadge.setStyle(sf::Text::Bold);
    compBadge.setPosition({panelX + 20.0f, 172.0f});
    target.draw(compBadge);

    // 5. RoomGraph Mini-Map & BFS Shortest Route
    sf::RectangleShape mapBox(sf::Vector2f(panelW - 40.0f, 220.0f));
    mapBox.setPosition({panelX + 20.0f, 200.0f});
    mapBox.setFillColor(sf::Color(22, 26, 38));
    mapBox.setOutlineColor(sf::Color(45, 60, 85));
    mapBox.setOutlineThickness(1.5f);
    target.draw(mapBox);

    sf::Text mapTitle(font_, "FACILITY GRAPH // BFS SHORTEST ROUTE", 10);
    mapTitle.setStyle(sf::Text::Bold);
    mapTitle.setFillColor(sf::Color(0, 210, 240));
    mapTitle.setPosition({panelX + 30.0f, 208.0f});
    target.draw(mapTitle);

    // Node layout coordinates inside mini-map
    // Room 0: Entrance (bottom center)
    // Room 1: Library (mid left)
    // Room 2: Lab (mid right)
    // Room 3: Secret Archive (upper center)
    // Room 4: Escape Chamber (top center)
    sf::Vector2f nodePos[5] = {
        sf::Vector2f(panelX + 170.0f, 385.0f), // Room 0
        sf::Vector2f(panelX + 90.0f,  325.0f), // Room 1
        sf::Vector2f(panelX + 250.0f, 325.0f), // Room 2
        sf::Vector2f(panelX + 170.0f, 290.0f), // Room 3
        sf::Vector2f(panelX + 170.0f, 240.0f)  // Room 4
    };

    // Graph Connections
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}
    };

    // Calculate BFS Path if enabled
    std::vector<int> bfsPath;
    if (showBFSPath_) {
        bfsPath = game.getRoomGraph().findPath(curRoomId, 4);
    }

    auto isEdgeInPath = [&](int u, int v) {
        if (bfsPath.size() < 2) return false;
        for (std::size_t i = 0; i < bfsPath.size() - 1; ++i) {
            if ((bfsPath[i] == u && bfsPath[i+1] == v) || (bfsPath[i] == v && bfsPath[i+1] == u)) {
                return true;
            }
        }
        return false;
    };

    // Draw Graph Edges
    for (const auto& e : edges) {
        bool onPath = showBFSPath_ && isEdgeInPath(e.first, e.second);
        sf::Color edgeColor = onPath ? sf::Color(255, 215, 0, 240) : sf::Color(65, 80, 110, 150);

        sf::Vertex line[] = {
            sf::Vertex{nodePos[e.first], edgeColor},
            sf::Vertex{nodePos[e.second], edgeColor}
        };
        target.draw(line, 2, sf::PrimitiveType::Lines);

        // If on path, draw thicker pulsing overlay
        if (onPath) {
            float pulse = (std::sin(animTimer_ * 6.0f) + 1.0f) * 0.5f;
            sf::CircleShape glowPoint(4.0f + pulse * 2.0f);
            glowPoint.setOrigin({4.0f + pulse * 2.0f, 4.0f + pulse * 2.0f});
            sf::Vector2f mid = (nodePos[e.first] + nodePos[e.second]) * 0.5f;
            glowPoint.setPosition(mid);
            glowPoint.setFillColor(sf::Color(0, 255, 240, 220));
            target.draw(glowPoint);
        }
    }

    // Draw Nodes
    const auto& visited = game.getVisitedRooms();
    auto isVisited = [&](int id) {
        return std::find(visited.begin(), visited.end(), id) != visited.end();
    };

    for (int i = 0; i < 5; ++i) {
        bool isCurrent = (i == curRoomId);
        bool visitedNode = isVisited(i);

        // Pulsing halo for current room
        if (isCurrent) {
            float pulse = (std::sin(animTimer_ * 5.0f) + 1.0f) * 0.5f;
            sf::CircleShape halo(18.0f + pulse * 5.0f);
            halo.setOrigin({18.0f + pulse * 5.0f, 18.0f + pulse * 5.0f});
            halo.setPosition(nodePos[i]);
            halo.setFillColor(sf::Color(0, 240, 255, static_cast<std::uint8_t>(60 + pulse * 60)));
            target.draw(halo);
        }

        sf::CircleShape circle(13.0f);
        circle.setOrigin({13.0f, 13.0f});
        circle.setPosition(nodePos[i]);

        if (isCurrent) {
            circle.setFillColor(sf::Color(0, 210, 255));
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(2.5f);
        } else if (i == 4) { // Escape room
            circle.setFillColor(sf::Color(240, 190, 40));
            circle.setOutlineColor(sf::Color(255, 235, 120));
            circle.setOutlineThickness(2.0f);
        } else if (visitedNode) {
            circle.setFillColor(sf::Color(45, 75, 115));
            circle.setOutlineColor(sf::Color(90, 130, 180));
            circle.setOutlineThickness(1.5f);
        } else {
            circle.setFillColor(sf::Color(30, 35, 48));
            circle.setOutlineColor(sf::Color(55, 65, 85));
            circle.setOutlineThickness(1.5f);
        }
        target.draw(circle);

        // Room ID Number
        sf::Text numText(font_, std::to_string(i), 11);
        numText.setStyle(sf::Text::Bold);
        numText.setFillColor(isCurrent ? sf::Color(10, 20, 30) : sf::Color::White);
        sf::FloatRect nb = numText.getLocalBounds();
        numText.setOrigin({nb.size.x * 0.5f, nb.size.y * 0.5f});
        numText.setPosition({nodePos[i].x, nodePos[i].y - 2.0f});
        target.draw(numText);
    }

    // Toggle Path Key Notice
    sf::Text pathNotice(font_, showBFSPath_ ? "[M] BFS Route: ACTIVE" : "[M] BFS Route: HIDDEN", 10);
    pathNotice.setFillColor(showBFSPath_ ? sf::Color(255, 215, 60) : sf::Color(120, 130, 150));
    pathNotice.setPosition({panelX + 30.0f, 402.0f});
    target.draw(pathNotice);

    // 6. Inventory Slots
    sf::Text invTitle(font_, "EXPLORER SATCHEL // INVENTORY", 11);
    invTitle.setStyle(sf::Text::Bold);
    invTitle.setFillColor(sf::Color(255, 215, 0));
    invTitle.setPosition({panelX + 20.0f, 435.0f});
    target.draw(invTitle);

    const auto& items = player.getInventory().getItems();
    const float slotW = 68.0f;
    const float slotH = 46.0f;

    for (int i = 0; i < 4; ++i) {
        float sx = panelX + 20.0f + i * 74.0f;
        float sy = 454.0f;

        sf::RectangleShape slot(sf::Vector2f(slotW, slotH));
        slot.setPosition({sx, sy});

        if (i < static_cast<int>(items.size())) {
            slot.setFillColor(sf::Color(35, 45, 65));
            slot.setOutlineColor(sf::Color(70, 160, 240));
            slot.setOutlineThickness(1.5f);
            target.draw(slot);

            // Item Name
            std::string iname = items[i]->getName();
            if (iname.length() > 9) iname = iname.substr(0, 8) + "..";
            sf::Text itext(font_, iname, 9);
            itext.setFillColor(sf::Color::White);
            itext.setPosition({sx + 4.0f, sy + 6.0f});
            target.draw(itext);

            sf::Text useText(font_, "[" + std::to_string(i + 1) + "] USE", 9);
            useText.setStyle(sf::Text::Bold);
            useText.setFillColor(sf::Color(255, 200, 60));
            useText.setPosition({sx + 4.0f, sy + 26.0f});
            target.draw(useText);
        } else {
            slot.setFillColor(sf::Color(22, 26, 35));
            slot.setOutlineColor(sf::Color(45, 52, 68));
            slot.setOutlineThickness(1.0f);
            target.draw(slot);

            sf::Text emptyText(font_, "[" + std::to_string(i + 1) + "]", 10);
            emptyText.setFillColor(sf::Color(75, 85, 105));
            emptyText.setPosition({sx + 24.0f, sy + 14.0f});
            target.draw(emptyText);
        }
    }

    // 7. Activity Log Feed
    sf::Text logTitle(font_, "MISSION ACTIVITY FEED", 11);
    logTitle.setStyle(sf::Text::Bold);
    logTitle.setFillColor(sf::Color(0, 220, 240));
    logTitle.setPosition({panelX + 20.0f, 515.0f});
    target.draw(logTitle);

    sf::RectangleShape logBox(sf::Vector2f(panelW - 40.0f, 135.0f));
    logBox.setPosition({panelX + 20.0f, 532.0f});
    logBox.setFillColor(sf::Color(18, 22, 32));
    logBox.setOutlineColor(sf::Color(40, 50, 70));
    logBox.setOutlineThickness(1.0f);
    target.draw(logBox);

    float logY = 540.0f;
    for (const auto& msg : logMessages_) {
        sf::Text line(font_, "> " + msg, 10);
        line.setFillColor(sf::Color(200, 215, 235));
        line.setPosition({panelX + 26.0f, logY});
        target.draw(line);
        logY += 24.0f;
    }

    // 8. Bottom Control Cheatsheet
    sf::RectangleShape guideBox(sf::Vector2f(panelW - 40.0f, 100.0f));
    guideBox.setPosition({panelX + 20.0f, 680.0f});
    guideBox.setFillColor(sf::Color(25, 28, 40));
    guideBox.setOutlineColor(sf::Color(55, 65, 88));
    guideBox.setOutlineThickness(1.0f);
    target.draw(guideBox);

    sf::Text guideTitle(font_, "OPERATIONAL CONTROLS", 10);
    guideTitle.setStyle(sf::Text::Bold);
    guideTitle.setFillColor(sf::Color(255, 215, 60));
    guideTitle.setPosition({panelX + 30.0f, 688.0f});
    target.draw(guideTitle);

    sf::Text g1(font_, "[W/A/S/D] Move    [E] Interact / Enter", 10);
    g1.setFillColor(sf::Color(190, 205, 225));
    g1.setPosition({panelX + 30.0f, 708.0f});
    target.draw(g1);

    sf::Text g2(font_, "[1-4] Use Item      [M] BFS Route", 10);
    g2.setFillColor(sf::Color(190, 205, 225));
    g2.setPosition({panelX + 30.0f, 726.0f});
    target.draw(g2);

    sf::Text g3(font_, "[F5] QuickSave    [F9] QuickLoad    [ESC] Menu", 10);
    g3.setFillColor(sf::Color(170, 185, 205));
    g3.setPosition({panelX + 30.0f, 744.0f});
    target.draw(g3);

    // 9. Render Floating Texts in play area
    for (const auto& ft : floatingTexts_) {
        sf::Text ftext(font_, ft.text, 14);
        ftext.setStyle(sf::Text::Bold);
        float alpha = 1.0f - (ft.lifetime / ft.maxLifetime);
        sf::Color c = ft.color;
        c.a = static_cast<std::uint8_t>(255 * std::max(0.0f, alpha));
        ftext.setFillColor(c);
        ftext.setOutlineColor(sf::Color(0, 0, 0, c.a));
        ftext.setOutlineThickness(2.0f);

        sf::FloatRect bounds = ftext.getLocalBounds();
        ftext.setOrigin({bounds.size.x * 0.5f, bounds.size.y * 0.5f});
        ftext.setPosition(ft.position);
        target.draw(ftext);
    }
}
