#include "RoomVisuals.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

RoomVisuals::RoomVisuals(sf::Font& font)
    : font_(font), animationTimer_(0.0f) {
    // Pre-populate particles
    particles_.resize(40);
    for (auto& p : particles_) {
        p.position = sf::Vector2f(
            60.0f + static_cast<float>(std::rand() % 740),
            60.0f + static_cast<float>(std::rand() % 680)
        );
        p.velocity = sf::Vector2f(
            (static_cast<float>(std::rand() % 100) - 50.0f) * 0.3f,
            (static_cast<float>(std::rand() % 100) - 70.0f) * 0.4f
        );
        p.color = sf::Color(255, 255, 255, 120);
        p.size = 2.0f + static_cast<float>(std::rand() % 4);
        p.lifetime = static_cast<float>(std::rand() % 100) / 20.0f;
        p.maxLifetime = 5.0f;
    }
}

void RoomVisuals::update(float dt, int roomId) {
    animationTimer_ += dt;

    // Update ambient particles
    for (auto& p : particles_) {
        p.position += p.velocity * dt;
        p.lifetime += dt;

        if (p.lifetime >= p.maxLifetime || p.position.x < 50.0f || p.position.x > 810.0f || p.position.y < 50.0f || p.position.y > 750.0f) {
            p.position = sf::Vector2f(
                60.0f + static_cast<float>(std::rand() % 740),
                680.0f + static_cast<float>(std::rand() % 60)
            );
            p.lifetime = 0.0f;

            if (roomId == 0) { // Entrance: subtle warm dust
                p.color = sf::Color(255, 230, 180, 90);
                p.velocity = sf::Vector2f(
                    (static_cast<float>(std::rand() % 60) - 30.0f) * 0.3f,
                    -15.0f - static_cast<float>(std::rand() % 25)
                );
            } else if (roomId == 1) { // Library: gentle golden paper motes
                p.color = sf::Color(255, 220, 130, 80);
                p.velocity = sf::Vector2f(
                    (static_cast<float>(std::rand() % 40) - 20.0f) * 0.2f,
                    -10.0f - static_cast<float>(std::rand() % 15)
                );
            } else if (roomId == 2) { // Laboratory: neon toxic bubbles
                p.color = sf::Color(80, 255, 180, 140);
                p.velocity = sf::Vector2f(
                    (static_cast<float>(std::rand() % 30) - 15.0f) * 0.4f,
                    -35.0f - static_cast<float>(std::rand() % 35)
                );
            } else if (roomId == 3) { // Archive: arcane sparks
                p.color = sf::Color(200, 140, 255, 130);
                p.velocity = sf::Vector2f(
                    (static_cast<float>(std::rand() % 100) - 50.0f) * 0.4f,
                    -20.0f - static_cast<float>(std::rand() % 30)
                );
            } else { // Escape Chamber: victory sparkles
                p.color = sf::Color(255, 235, 100, 180);
                p.velocity = sf::Vector2f(
                    (static_cast<float>(std::rand() % 80) - 40.0f) * 0.5f,
                    -40.0f - static_cast<float>(std::rand() % 40)
                );
            }
        }
    }
}

void RoomVisuals::renderFloor(sf::RenderTarget& target, int roomId) {
    const float tileSize = 60.0f;
    const int cols = 13; // 13 * 60 = 780
    const int rows = 12; // 12 * 60 = 720
    const float startX = 40.0f;
    const float startY = 40.0f;

    sf::RectangleShape tile(sf::Vector2f(tileSize - 2.0f, tileSize - 2.0f));

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            tile.setPosition({startX + c * tileSize, startY + r * tileSize});

            bool alt = ((r + c) % 2 == 0);
            if (roomId == 0) { // Entrance Hall: elegant marble
                tile.setFillColor(alt ? sf::Color(55, 58, 70) : sf::Color(45, 48, 58));
                tile.setOutlineColor(sf::Color(35, 38, 48));
            } else if (roomId == 1) { // Library: polished mahogany wood
                tile.setFillColor(alt ? sf::Color(65, 43, 30) : sf::Color(55, 35, 24));
                tile.setOutlineColor(sf::Color(40, 26, 18));
            } else if (roomId == 2) { // Laboratory: slate tiles with high-tech tint
                tile.setFillColor(alt ? sf::Color(32, 44, 56) : sf::Color(24, 34, 46));
                tile.setOutlineColor(sf::Color(18, 60, 75));
            } else if (roomId == 3) { // Secret Archive: ancient obsidian flagstones
                tile.setFillColor(alt ? sf::Color(32, 26, 42) : sf::Color(24, 20, 34));
                tile.setOutlineColor(sf::Color(45, 32, 60));
            } else { // Escape Chamber: gold-trimmed obsidian
                tile.setFillColor(alt ? sf::Color(48, 42, 30) : sf::Color(38, 32, 22));
                tile.setOutlineColor(sf::Color(180, 150, 60));
            }
            tile.setOutlineThickness(1.0f);
            target.draw(tile);
        }
    }

    // Special floor accents per room:
    if (roomId == 0) {
        // Red carpet runner from entrance to center
        sf::RectangleShape carpet(sf::Vector2f(120.0f, 680.0f));
        carpet.setPosition({370.0f, 50.0f});
        carpet.setFillColor(sf::Color(140, 25, 35, 200));
        carpet.setOutlineColor(sf::Color(200, 160, 60, 220));
        carpet.setOutlineThickness(3.0f);
        target.draw(carpet);
    } else if (roomId == 1) {
        // Ornate Persian rug in center
        sf::RectangleShape rug(sf::Vector2f(320.0f, 260.0f));
        rug.setPosition({270.0f, 270.0f});
        rug.setFillColor(sf::Color(24, 70, 50, 210));
        rug.setOutlineColor(sf::Color(190, 145, 50, 220));
        rug.setOutlineThickness(4.0f);
        target.draw(rug);

        // Inner pattern border
        sf::RectangleShape rugInner(sf::Vector2f(280.0f, 220.0f));
        rugInner.setPosition({290.0f, 290.0f});
        rugInner.setFillColor(sf::Color(80, 30, 35, 180));
        rugInner.setOutlineColor(sf::Color(210, 180, 90, 160));
        rugInner.setOutlineThickness(2.0f);
        target.draw(rugInner);
    } else if (roomId == 2) {
        // High-tech floor circuit lines
        sf::Vertex lines[] = {
            sf::Vertex{sf::Vector2f(100.0f, 400.0f), sf::Color(0, 220, 200, 120)},
            sf::Vertex{sf::Vector2f(760.0f, 400.0f), sf::Color(0, 220, 200, 120)},
            sf::Vertex{sf::Vector2f(430.0f, 100.0f), sf::Color(0, 220, 200, 120)},
            sf::Vertex{sf::Vector2f(430.0f, 700.0f), sf::Color(0, 220, 200, 120)}
        };
        target.draw(lines, 4, sf::PrimitiveType::Lines);

        // Center circular tech pad
        sf::CircleShape pad(120.0f);
        pad.setOrigin({120.0f, 120.0f});
        pad.setPosition({430.0f, 400.0f});
        pad.setFillColor(sf::Color(20, 50, 65, 140));
        pad.setOutlineColor(sf::Color(0, 230, 210, 180));
        pad.setOutlineThickness(3.0f);
        target.draw(pad);
    } else if (roomId == 3) {
        // Arcane magical rune circle
        sf::CircleShape runeCircle(150.0f);
        runeCircle.setOrigin({150.0f, 150.0f});
        runeCircle.setPosition({430.0f, 400.0f});
        runeCircle.setFillColor(sf::Color(35, 20, 55, 120));
        runeCircle.setOutlineColor(sf::Color(170, 90, 250, 160));
        runeCircle.setOutlineThickness(3.0f);
        target.draw(runeCircle);

        // Inner glowing star/polygon
        sf::CircleShape innerRune(90.0f, 6);
        innerRune.setOrigin({90.0f, 90.0f});
        innerRune.setPosition({430.0f, 400.0f});
        innerRune.setRotation(sf::degrees(animationTimer_ * 15.0f));
        innerRune.setFillColor(sf::Color(0, 0, 0, 0));
        innerRune.setOutlineColor(sf::Color(210, 140, 255, 200));
        innerRune.setOutlineThickness(2.0f);
        target.draw(innerRune);
    } else if (roomId == 4) {
        // Escape Chamber radiant floor seal
        sf::CircleShape seal(170.0f);
        seal.setOrigin({170.0f, 170.0f});
        seal.setPosition({430.0f, 400.0f});
        float pulse = (std::sin(animationTimer_ * 3.0f) + 1.0f) * 0.5f;
        seal.setFillColor(sf::Color(60, 50, 20, static_cast<std::uint8_t>(100 + pulse * 60)));
        seal.setOutlineColor(sf::Color(255, 215, 0, static_cast<std::uint8_t>(180 + pulse * 75)));
        seal.setOutlineThickness(4.0f);
        target.draw(seal);
    }
}

void RoomVisuals::renderWalls(sf::RenderTarget& target) {
    const float wallThickness = 45.0f;
    const sf::Color wallBase(28, 30, 38);
    const sf::Color wallTrim(18, 20, 26);
    const sf::Color wallHighlight(55, 60, 75);

    // Top Wall
    sf::RectangleShape topWall(sf::Vector2f(860.0f, wallThickness));
    topWall.setPosition({0.0f, 0.0f});
    topWall.setFillColor(wallBase);
    topWall.setOutlineColor(wallHighlight);
    topWall.setOutlineThickness(2.0f);
    target.draw(topWall);

    // Bottom Wall
    sf::RectangleShape bottomWall(sf::Vector2f(860.0f, wallThickness));
    bottomWall.setPosition({0.0f, 800.0f - wallThickness});
    bottomWall.setFillColor(wallBase);
    bottomWall.setOutlineColor(wallTrim);
    bottomWall.setOutlineThickness(2.0f);
    target.draw(bottomWall);

    // Left Wall
    sf::RectangleShape leftWall(sf::Vector2f(wallThickness, 800.0f));
    leftWall.setPosition({0.0f, 0.0f});
    leftWall.setFillColor(wallBase);
    leftWall.setOutlineColor(wallTrim);
    leftWall.setOutlineThickness(2.0f);
    target.draw(leftWall);

    // Right Wall (separating play arena from HUD)
    sf::RectangleShape rightWall(sf::Vector2f(wallThickness, 800.0f));
    rightWall.setPosition({860.0f - wallThickness, 0.0f});
    rightWall.setFillColor(wallBase);
    rightWall.setOutlineColor(sf::Color(80, 95, 130));
    rightWall.setOutlineThickness(2.0f);
    target.draw(rightWall);

    // Corner reinforcement columns
    auto drawCorner = [&](float x, float y) {
        sf::RectangleShape col(sf::Vector2f(wallThickness + 10.0f, wallThickness + 10.0f));
        col.setPosition({x, y});
        col.setFillColor(sf::Color(40, 44, 56));
        col.setOutlineColor(sf::Color(65, 72, 90));
        col.setOutlineThickness(2.0f);
        target.draw(col);
    };
    drawCorner(0.0f, 0.0f);
    drawCorner(860.0f - wallThickness - 10.0f, 0.0f);
    drawCorner(0.0f, 800.0f - wallThickness - 10.0f);
    drawCorner(860.0f - wallThickness - 10.0f, 800.0f - wallThickness - 10.0f);

    // Wall Sconces / Torches with flickering flame
    auto drawSconce = [&](float x, float y) {
        sf::RectangleShape bracket(sf::Vector2f(10.0f, 18.0f));
        bracket.setOrigin({5.0f, 9.0f});
        bracket.setPosition({x, y});
        bracket.setFillColor(sf::Color(120, 100, 60));
        target.draw(bracket);

        float flameOffset = std::sin(animationTimer_ * 8.0f + x * 0.1f) * 2.0f;
        sf::CircleShape flame(6.0f + flameOffset * 0.5f);
        flame.setOrigin({6.0f, 6.0f});
        flame.setPosition({x, y - 6.0f});
        flame.setFillColor(sf::Color(255, 170, 40, 220));
        target.draw(flame);

        // Torch glow aura
        sf::CircleShape glow(35.0f);
        glow.setOrigin({35.0f, 35.0f});
        glow.setPosition({x, y - 6.0f});
        glow.setFillColor(sf::Color(255, 160, 30, 45));
        target.draw(glow);
    };

    drawSconce(150.0f, 45.0f);
    drawSconce(710.0f, 45.0f);
    drawSconce(150.0f, 755.0f);
    drawSconce(710.0f, 755.0f);
    drawSconce(45.0f, 250.0f);
    drawSconce(45.0f, 550.0f);
}

void RoomVisuals::renderProps(sf::RenderTarget& target, int roomId) {
    if (roomId == 0) {
        // Entrance Hall: Ornate Stone Pillars
        auto drawPillar = [&](float px, float py) {
            sf::CircleShape base(24.0f);
            base.setOrigin({24.0f, 24.0f});
            base.setPosition({px, py});
            base.setFillColor(sf::Color(65, 70, 85));
            base.setOutlineColor(sf::Color(180, 150, 70));
            base.setOutlineThickness(3.0f);
            target.draw(base);

            sf::CircleShape shaft(16.0f);
            shaft.setOrigin({16.0f, 16.0f});
            shaft.setPosition({px, py});
            shaft.setFillColor(sf::Color(85, 90, 105));
            target.draw(shaft);
        };
        drawPillar(200.0f, 220.0f);
        drawPillar(660.0f, 220.0f);
        drawPillar(200.0f, 580.0f);
        drawPillar(660.0f, 580.0f);

        // Entrance banner
        sf::RectangleShape banner(sf::Vector2f(200.0f, 32.0f));
        banner.setOrigin({100.0f, 16.0f});
        banner.setPosition({430.0f, 720.0f});
        banner.setFillColor(sf::Color(100, 20, 30));
        banner.setOutlineColor(sf::Color(220, 180, 60));
        banner.setOutlineThickness(2.0f);
        target.draw(banner);

        sf::Text label(font_, "ENTRANCE FOYER", 14);
        label.setFillColor(sf::Color(255, 230, 150));
        label.setStyle(sf::Text::Bold);
        label.setPosition({365.0f, 712.0f});
        target.draw(label);

    } else if (roomId == 1) {
        // Library: Large Bookshelves along north and west walls
        auto drawBookshelf = [&](float bx, float by, float bw, float bh) {
            sf::RectangleShape shelf(sf::Vector2f(bw, bh));
            shelf.setPosition({bx, by});
            shelf.setFillColor(sf::Color(48, 30, 20));
            shelf.setOutlineColor(sf::Color(75, 45, 30));
            shelf.setOutlineThickness(2.0f);
            target.draw(shelf);

            // Draw book spines
            const int bookCount = static_cast<int>(bw / 12.0f);
            for (int i = 0; i < bookCount; ++i) {
                sf::RectangleShape book(sf::Vector2f(9.0f, bh - 8.0f));
                book.setPosition({bx + 4.0f + i * 11.5f, by + 4.0f});
                sf::Color colors[] = {
                    sf::Color(160, 40, 40), sf::Color(40, 110, 60),
                    sf::Color(30, 70, 140), sf::Color(180, 130, 30),
                    sf::Color(110, 45, 120)
                };
                book.setFillColor(colors[(i * 3 + static_cast<int>(bx)) % 5]);
                target.draw(book);
            }
        };

        drawBookshelf(100.0f, 60.0f, 240.0f, 36.0f);
        drawBookshelf(520.0f, 60.0f, 240.0f, 36.0f);
        drawBookshelf(60.0f, 260.0f, 36.0f, 280.0f);

        // Center Study Desk
        sf::RectangleShape desk(sf::Vector2f(160.0f, 80.0f));
        desk.setOrigin({80.0f, 40.0f});
        desk.setPosition({430.0f, 400.0f});
        desk.setFillColor(sf::Color(58, 36, 24));
        desk.setOutlineColor(sf::Color(90, 56, 38));
        desk.setOutlineThickness(3.0f);
        target.draw(desk);

        // Open Tome on desk
        sf::RectangleShape tomeLeft(sf::Vector2f(22.0f, 30.0f));
        tomeLeft.setPosition({408.0f, 385.0f});
        tomeLeft.setFillColor(sf::Color(240, 235, 210));
        target.draw(tomeLeft);

        sf::RectangleShape tomeRight(sf::Vector2f(22.0f, 30.0f));
        tomeRight.setPosition({432.0f, 385.0f});
        tomeRight.setFillColor(sf::Color(230, 225, 200));
        target.draw(tomeRight);

    } else if (roomId == 2) {
        // Laboratory: High-tech workstations, server racks, and chemical tube
        auto drawServer = [&](float sx, float sy) {
            sf::RectangleShape server(sf::Vector2f(60.0f, 110.0f));
            server.setPosition({sx, sy});
            server.setFillColor(sf::Color(25, 32, 42));
            server.setOutlineColor(sf::Color(45, 60, 80));
            server.setOutlineThickness(2.0f);
            target.draw(server);

            // Blinking LEDs
            for (int i = 0; i < 5; ++i) {
                sf::CircleShape led(3.0f);
                led.setPosition({sx + 10.0f, sy + 15.0f + i * 18.0f});
                bool blink = (std::sin(animationTimer_ * 5.0f + i + sx) > 0.0);
                led.setFillColor(blink ? sf::Color(0, 255, 180) : sf::Color(0, 100, 70));
                target.draw(led);

                sf::CircleShape led2(3.0f);
                led2.setPosition({sx + 25.0f, sy + 15.0f + i * 18.0f});
                bool blink2 = (std::sin(animationTimer_ * 4.0f + i * 2 + sy) > 0.0);
                led2.setFillColor(blink2 ? sf::Color(255, 200, 40) : sf::Color(120, 90, 20));
                target.draw(led2);
            }
        };

        drawServer(80.0f, 100.0f);
        drawServer(80.0f, 230.0f);
        drawServer(720.0f, 100.0f);
        drawServer(720.0f, 230.0f);

        // Cryogenic Containment Capsule in center top
        sf::RectangleShape capsule(sf::Vector2f(90.0f, 130.0f));
        capsule.setOrigin({45.0f, 65.0f});
        capsule.setPosition({430.0f, 200.0f});
        capsule.setFillColor(sf::Color(15, 30, 45, 200));
        capsule.setOutlineColor(sf::Color(0, 220, 255));
        capsule.setOutlineThickness(3.0f);
        target.draw(capsule);

        // Glowing liquid inside capsule
        sf::RectangleShape liquid(sf::Vector2f(74.0f, 100.0f));
        liquid.setOrigin({37.0f, 50.0f});
        liquid.setPosition({430.0f, 205.0f});
        float pulse = (std::sin(animationTimer_ * 4.0f) + 1.0f) * 0.5f;
        liquid.setFillColor(sf::Color(0, 240, 200, static_cast<std::uint8_t>(120 + pulse * 60)));
        target.draw(liquid);

    } else if (roomId == 3) {
        // Secret Archive: Arcane Tesla Coils & Stone Archive Pedestals
        auto drawTeslaCoil = [&](float tx, float ty) {
            sf::CircleShape base(30.0f);
            base.setOrigin({30.0f, 30.0f});
            base.setPosition({tx, ty});
            base.setFillColor(sf::Color(35, 28, 48));
            base.setOutlineColor(sf::Color(160, 90, 240));
            base.setOutlineThickness(3.0f);
            target.draw(base);

            sf::CircleShape orb(16.0f);
            orb.setOrigin({16.0f, 16.0f});
            orb.setPosition({tx, ty});
            float pulse = (std::sin(animationTimer_ * 6.0f + tx) + 1.0f) * 0.5f;
            orb.setFillColor(sf::Color(220, 150, 255, static_cast<std::uint8_t>(180 + pulse * 75)));
            target.draw(orb);

            // Crackling electric spark
            sf::Vertex spark[] = {
                sf::Vertex{sf::Vector2f(tx, ty), sf::Color(255, 200, 255)},
                sf::Vertex{sf::Vector2f(tx + std::sin(animationTimer_ * 12.0f) * 25.0f, ty + std::cos(animationTimer_ * 12.0f) * 25.0f), sf::Color(180, 100, 255, 0)}
            };
            target.draw(spark, 2, sf::PrimitiveType::Lines);
        };

        drawTeslaCoil(180.0f, 200.0f);
        drawTeslaCoil(680.0f, 200.0f);
        drawTeslaCoil(180.0f, 600.0f);
        drawTeslaCoil(680.0f, 600.0f);

        // Ancient locked Archive Safe
        sf::RectangleShape safe(sf::Vector2f(100.0f, 70.0f));
        safe.setOrigin({50.0f, 35.0f});
        safe.setPosition({430.0f, 620.0f});
        safe.setFillColor(sf::Color(30, 24, 40));
        safe.setOutlineColor(sf::Color(140, 90, 200));
        safe.setOutlineThickness(3.0f);
        target.draw(safe);

        // Safe dial
        sf::CircleShape dial(14.0f);
        dial.setOrigin({14.0f, 14.0f});
        dial.setPosition({430.0f, 620.0f});
        dial.setFillColor(sf::Color(60, 48, 80));
        dial.setOutlineColor(sf::Color(200, 160, 255));
        dial.setOutlineThickness(2.0f);
        target.draw(dial);

    } else if (roomId == 4) {
        // Escape Chamber: The Golden Exit Vault Gate
        sf::RectangleShape portalFrame(sf::Vector2f(220.0f, 140.0f));
        portalFrame.setOrigin({110.0f, 70.0f});
        portalFrame.setPosition({430.0f, 120.0f});
        portalFrame.setFillColor(sf::Color(40, 32, 20));
        portalFrame.setOutlineColor(sf::Color(255, 215, 0));
        portalFrame.setOutlineThickness(4.0f);
        target.draw(portalFrame);

        // Inner glowing escape vortex
        sf::CircleShape portalAura(60.0f);
        portalAura.setOrigin({60.0f, 60.0f});
        portalAura.setPosition({430.0f, 120.0f});
        float pulse = (std::sin(animationTimer_ * 5.0f) + 1.0f) * 0.5f;
        portalAura.setFillColor(sf::Color(255, 230, 120, static_cast<std::uint8_t>(160 + pulse * 90)));
        target.draw(portalAura);

        // Exit Sign above portal
        sf::RectangleShape exitSign(sf::Vector2f(100.0f, 26.0f));
        exitSign.setOrigin({50.0f, 13.0f});
        exitSign.setPosition({430.0f, 36.0f});
        exitSign.setFillColor(sf::Color(0, 180, 80));
        exitSign.setOutlineColor(sf::Color(255, 255, 255));
        exitSign.setOutlineThickness(2.0f);
        target.draw(exitSign);

        sf::Text exitText(font_, "EXIT PORTAL", 13);
        exitText.setFillColor(sf::Color::White);
        exitText.setStyle(sf::Text::Bold);
        exitText.setPosition({385.0f, 28.0f});
        target.draw(exitText);
    }
}

void RoomVisuals::renderLighting(sf::RenderTarget& target, const sf::Vector2f& playerPos) {
    // Dynamic soft light halo around player
    sf::CircleShape playerLight(160.0f);
    playerLight.setOrigin({160.0f, 160.0f});
    playerLight.setPosition(playerPos);
    playerLight.setFillColor(sf::Color(255, 240, 200, 35));
    target.draw(playerLight);

    sf::CircleShape playerCoreLight(70.0f);
    playerCoreLight.setOrigin({70.0f, 70.0f});
    playerCoreLight.setPosition(playerPos);
    playerCoreLight.setFillColor(sf::Color(255, 250, 220, 50));
    target.draw(playerCoreLight);
}

void RoomVisuals::render(sf::RenderTarget& target, int roomId, const sf::Vector2f& playerPos) {
    renderFloor(target, roomId);
    renderProps(target, roomId);
    renderWalls(target);
    renderLighting(target, playerPos);

    // Draw ambient floating particles
    for (const auto& p : particles_) {
        sf::CircleShape part(p.size);
        part.setOrigin({p.size, p.size});
        part.setPosition(p.position);
        float alpha = 1.0f - (p.lifetime / p.maxLifetime);
        sf::Color c = p.color;
        c.a = static_cast<std::uint8_t>(c.a * std::max(0.0f, alpha));
        part.setFillColor(c);
        target.draw(part);
    }
}

bool RoomVisuals::checkWallCollision(const sf::FloatRect& bounds) const {
    // Check perimeter walls
    if (bounds.position.x < 52.0f) return true;
    if (bounds.position.x + bounds.size.x > 808.0f) return true;
    if (bounds.position.y < 52.0f) return true;
    if (bounds.position.y + bounds.size.y > 748.0f) return true;

    return false;
}
