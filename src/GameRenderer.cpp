#include "GameRenderer.h"
#include "GameManager.h"
#include <iostream>
#include <cstdlib>

GameRenderer::GameRenderer()
    : window_(sf::VideoMode({1200, 800}), "Escape Room: Infinite Rooms", sf::Style::Titlebar | sf::Style::Close),
      lastRoomId_(-1),
      victoryTriggered_(false),
      gameOverTriggered_(false) {
    window_.setFramerateLimit(60);

    // Load Font with robust fallback paths
    bool fontLoaded = font_.openFromFile("assets/font.ttf");
    if (!fontLoaded) {
        fontLoaded = font_.openFromFile("C:/Windows/Fonts/segoeui.ttf");
    }
    if (!fontLoaded) {
        fontLoaded = font_.openFromFile("C:/Windows/Fonts/arial.ttf");
    }
    if (!fontLoaded) {
        std::cerr << "Warning: Could not open system font. Text may not render properly.\n";
    }

    visuals_ = std::make_unique<RoomVisuals>(font_);
    player_ = std::make_unique<PlayerEntity>(sf::Vector2f(430.0f, 620.0f), font_);
    hud_ = std::make_unique<HUD>(font_);
    puzzleModal_ = std::make_unique<PuzzleModal>(font_);
    dialog_ = std::make_unique<GameDialog>(font_);
}

bool GameRenderer::isOpen() const {
    return window_.isOpen();
}

void GameRenderer::setupRoomEntities(GameManager& game) {
    roomEntities_.clear();

    // ==========================================
    // ROOM 0: ENTRANCE HALL
    // ==========================================
    // Door 1 -> Library (Room 1)
    auto doorToLib = std::make_shared<DoorEntity>(1, "To Library [Room 1]", 1, 101, sf::Vector2f(430.0f, 48.0f), DoorSide::North);
    // Door 2 -> Laboratory (Room 2)
    auto doorToLab = std::make_shared<DoorEntity>(2, "To Laboratory [Room 2]", 2, 102, sf::Vector2f(806.0f, 400.0f), DoorSide::East);
    // Trap in Entrance Hall
    auto entranceTrap = std::make_shared<TrapZoneEntity>("Spike Trap", 20, sf::Vector2f(430.0f, 380.0f), sf::Vector2f(160.0f, 70.0f));
    // Pickup Item: Health Potion
    auto healthPotion = std::make_shared<ItemPickupEntity>("Health Potion", "Restores 25 Health Points", 25, sf::Vector2f(240.0f, 520.0f));

    roomEntities_[0].push_back(doorToLib);
    roomEntities_[0].push_back(doorToLab);
    roomEntities_[0].push_back(entranceTrap);
    roomEntities_[0].push_back(healthPotion);

    // ==========================================
    // ROOM 1: LIBRARY
    // ==========================================
    // Door 3 -> Secret Archive (Room 3)
    auto doorLibToArchive = std::make_shared<DoorEntity>(3, "To Secret Archive [Room 3]", 3, 103, sf::Vector2f(430.0f, 48.0f), DoorSide::North);
    // Return Door -> Entrance Hall (Room 0)
    auto doorLibToEntrance = std::make_shared<DoorEntity>(10, "To Entrance Hall [Room 0]", 0, 0, sf::Vector2f(430.0f, 752.0f), DoorSide::South);
    doorLibToEntrance->setUnlocked(true);

    // Puzzle Pedestal 101
    auto ped101 = std::make_shared<PuzzlePedestalEntity>(101, "Library Puzzle #101", sf::Vector2f(430.0f, 320.0f));
    // Item: Ancient Clue Scroll
    auto clueScroll = std::make_shared<ItemPickupEntity>("Clue Scroll", "Ancient writings detailing facility secrets", 0, sf::Vector2f(580.0f, 360.0f));

    roomEntities_[1].push_back(doorLibToArchive);
    roomEntities_[1].push_back(doorLibToEntrance);
    roomEntities_[1].push_back(ped101);
    roomEntities_[1].push_back(clueScroll);

    // ==========================================
    // ROOM 2: LABORATORY
    // ==========================================
    // Door 3 -> Secret Archive (Room 3)
    auto doorLabToArchive = std::make_shared<DoorEntity>(3, "To Secret Archive [Room 3]", 3, 103, sf::Vector2f(430.0f, 48.0f), DoorSide::North);
    // Return Door -> Entrance Hall (Room 0)
    auto doorLabToEntrance = std::make_shared<DoorEntity>(20, "To Entrance Hall [Room 0]", 0, 0, sf::Vector2f(54.0f, 400.0f), DoorSide::West);
    doorLabToEntrance->setUnlocked(true);

    // Puzzle Pedestal 102
    auto ped102 = std::make_shared<PuzzlePedestalEntity>(102, "Laboratory Puzzle #102", sf::Vector2f(430.0f, 360.0f));
    // Trap in Laboratory: Poison Gas Vent
    auto labTrap = std::make_shared<TrapZoneEntity>("Poison Gas Vent", 15, sf::Vector2f(430.0f, 520.0f), sf::Vector2f(160.0f, 70.0f));
    // Item: Medical First-Aid Kit
    auto medKit = std::make_shared<ItemPickupEntity>("Medkit", "Standard facility first-aid kit (+35 HP)", 35, sf::Vector2f(280.0f, 560.0f));

    roomEntities_[2].push_back(doorLabToArchive);
    roomEntities_[2].push_back(doorLabToEntrance);
    roomEntities_[2].push_back(ped102);
    roomEntities_[2].push_back(labTrap);
    roomEntities_[2].push_back(medKit);

    // ==========================================
    // ROOM 3: SECRET ARCHIVE
    // ==========================================
    // Door 4 -> Escape Chamber (Room 4)
    auto doorArchiveToEscape = std::make_shared<DoorEntity>(4, "To Escape Chamber [Room 4]", 4, 104, sf::Vector2f(430.0f, 48.0f), DoorSide::North);
    // Return Door -> Library (Room 1)
    auto doorArchiveToLib = std::make_shared<DoorEntity>(30, "To Library [Room 1]", 1, 0, sf::Vector2f(280.0f, 752.0f), DoorSide::South);
    doorArchiveToLib->setUnlocked(true);
    // Return Door -> Laboratory (Room 2)
    auto doorArchiveToLab = std::make_shared<DoorEntity>(31, "To Laboratory [Room 2]", 2, 0, sf::Vector2f(580.0f, 752.0f), DoorSide::South);
    doorArchiveToLab->setUnlocked(true);

    // Puzzle Pedestal 103
    auto ped103 = std::make_shared<PuzzlePedestalEntity>(103, "Archive Cipher #103", sf::Vector2f(430.0f, 400.0f));
    // Trap in Archive: Tesla Electric Arc
    auto archiveTrap = std::make_shared<TrapZoneEntity>("Electric Arc Coil", 25, sf::Vector2f(430.0f, 260.0f), sf::Vector2f(160.0f, 70.0f));
    // Item: Archive Keycard
    auto keycard = std::make_shared<ItemPickupEntity>("Archive Keycard", "Level-4 Security access card", 0, sf::Vector2f(600.0f, 560.0f));

    roomEntities_[3].push_back(doorArchiveToEscape);
    roomEntities_[3].push_back(doorArchiveToLib);
    roomEntities_[3].push_back(doorArchiveToLab);
    roomEntities_[3].push_back(ped103);
    roomEntities_[3].push_back(archiveTrap);
    roomEntities_[3].push_back(keycard);

    // ==========================================
    // ROOM 4: ESCAPE CHAMBER
    // ==========================================
    // Return Door -> Secret Archive (Room 3)
    auto doorEscapeToArchive = std::make_shared<DoorEntity>(40, "To Secret Archive [Room 3]", 3, 0, sf::Vector2f(430.0f, 752.0f), DoorSide::South);
    doorEscapeToArchive->setUnlocked(true);

    // Puzzle Pedestal 104: Final Sequence
    auto ped104 = std::make_shared<PuzzlePedestalEntity>(104, "Final Escape Sequence #104", sf::Vector2f(430.0f, 320.0f));

    roomEntities_[4].push_back(doorEscapeToArchive);
    roomEntities_[4].push_back(ped104);

    syncEntitiesWithGameState(game);
}

void GameRenderer::syncEntitiesWithGameState(GameManager& game) {
    const auto& puzzles = game.getPuzzles();
    const auto& doors = game.getDoors();

    for (auto& pair : roomEntities_) {
        for (auto& entity : pair.second) {
            if (entity->getType() == EntityType::Door) {
                auto doorEnt = std::dynamic_pointer_cast<DoorEntity>(entity);
                if (doorEnt && doorEnt->getRequiredPuzzleId() > 0) {
                    auto pIt = puzzles.find(doorEnt->getRequiredPuzzleId());
                    if (pIt != puzzles.end() && pIt->second->isSolved()) {
                        doorEnt->setUnlocked(true);
                    }
                }
            } else if (entity->getType() == EntityType::PuzzlePedestal) {
                auto pedEnt = std::dynamic_pointer_cast<PuzzlePedestalEntity>(entity);
                if (pedEnt) {
                    auto pIt = puzzles.find(pedEnt->getPuzzleId());
                    if (pIt != puzzles.end()) {
                        pedEnt->setSolved(pIt->second->isSolved());
                    }
                }
            }
        }
    }
}

void GameRenderer::handleInteraction(GameManager& game) {
    int curRoom = game.getCurrentRoomId();
    sf::Vector2f playerPos = player_->getPosition();

    auto it = roomEntities_.find(curRoom);
    if (it == roomEntities_.end()) return;

    for (auto& entity : it->second) {
        if (!entity->isActive()) continue;

        if (entity->isNear(playerPos, 70.0f)) {
            if (entity->getType() == EntityType::Door) {
                auto door = std::dynamic_pointer_cast<DoorEntity>(entity);
                if (door) {
                    if (door->isUnlocked()) {
                        // Move through door
                        int dest = door->getDestinationRoomId();
                        game.moveToRoom(dest);
                        sound_.play(SoundEffect::DoorUnlock);
                        hud_->addLog("Traversed door to Chamber " + std::to_string(dest));

                        // Reposition player near the entrance of destination room
                        if (door->getSide() == DoorSide::North) {
                            player_->setPosition({430.0f, 680.0f});
                            player_->setDirection(FacingDirection::Up);
                        } else if (door->getSide() == DoorSide::South) {
                            player_->setPosition({430.0f, 100.0f});
                            player_->setDirection(FacingDirection::Down);
                        } else if (door->getSide() == DoorSide::East) {
                            player_->setPosition({100.0f, 400.0f});
                            player_->setDirection(FacingDirection::Right);
                        } else {
                            player_->setPosition({740.0f, 400.0f});
                            player_->setDirection(FacingDirection::Left);
                        }
                        return;
                    } else {
                        // Door locked! Open required puzzle modal
                        sound_.play(SoundEffect::DoorLocked);
                        hud_->addLog("Door locked! Requires puzzle resolution.");
                        hud_->addFloatingText("LOCKED!", playerPos, sf::Color(255, 60, 60));

                        const auto& puzzles = game.getPuzzles();
                        auto pIt = puzzles.find(door->getRequiredPuzzleId());
                        if (pIt != puzzles.end()) {
                            puzzleModal_->open(pIt->second.get());
                        }
                        return;
                    }
                }
            } else if (entity->getType() == EntityType::PuzzlePedestal) {
                auto ped = std::dynamic_pointer_cast<PuzzlePedestalEntity>(entity);
                if (ped) {
                    sound_.play(SoundEffect::Interact);
                    const auto& puzzles = game.getPuzzles();
                    auto pIt = puzzles.find(ped->getPuzzleId());
                    if (pIt != puzzles.end()) {
                        puzzleModal_->open(pIt->second.get());
                        hud_->addLog("Inspecting " + ped->getName());
                    }
                    return;
                }
            } else if (entity->getType() == EntityType::ItemPickup) {
                auto pickup = std::dynamic_pointer_cast<ItemPickupEntity>(entity);
                if (pickup && !pickup->isCollected()) {
                    pickup->collect();
                    sound_.play(SoundEffect::Pickup);

                    // Add to Player's inventory
                    auto item = std::make_shared<Item>(pickup->getName(), pickup->getDescription(), true);
                    game.getPlayer().getInventory().addItem(item);

                    hud_->addLog("Collected: " + pickup->getName());
                    hud_->addFloatingText("+" + pickup->getName(), playerPos, sf::Color(80, 240, 255));
                    return;
                }
            }
        }
    }
}

void GameRenderer::useInventoryItem(int slotIndex, GameManager& game) {
    auto& inv = game.getPlayer().getInventory();
    const auto& items = inv.getItems();
    if (slotIndex >= 0 && slotIndex < static_cast<int>(items.size())) {
        std::string iname = items[slotIndex]->getName();
        int heal = 0;
        if (iname.find("Potion") != std::string::npos) heal = 25;
        else if (iname.find("Medkit") != std::string::npos) heal = 35;

        if (heal > 0) {
            game.getPlayer().heal(heal);
            sound_.play(SoundEffect::Heal);
            hud_->addLog("Used " + iname + " (+ " + std::to_string(heal) + " HP)");
            hud_->addFloatingText("+" + std::to_string(heal) + " HP", player_->getPosition(), sf::Color(60, 255, 120));
            inv.removeItem(iname);
        } else {
            hud_->addLog("Examined " + iname + ": " + items[slotIndex]->getDescription());
            sound_.play(SoundEffect::Interact);
        }
    }
}

void GameRenderer::runGraphicalGame(GameManager& game) {
    setupRoomEntities(game);

    sf::Clock clock;

    while (window_.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; // Clamp delta time

        int curRoom = game.getCurrentRoomId();

        // 1. Process Window Events
        while (const std::optional event = window_.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window_.close();
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (dialog_->isOpen()) {
                    dialog_->handleKeyPressed(keyPressed->code, game, sound_, *hud_, window_);
                    syncEntitiesWithGameState(game);
                } else if (puzzleModal_->isOpen()) {
                    puzzleModal_->handleKeyPressed(keyPressed->code, game, sound_);
                    syncEntitiesWithGameState(game);
                } else {
                    // Exploration key handlers
                    if (keyPressed->code == sf::Keyboard::Key::Escape || keyPressed->code == sf::Keyboard::Key::P) {
                        dialog_->openPauseMenu();
                    } else if (keyPressed->code == sf::Keyboard::Key::E) {
                        handleInteraction(game);
                    } else if (keyPressed->code == sf::Keyboard::Key::M) {
                        hud_->toggleBFSPath();
                        sound_.play(SoundEffect::Interact);
                    } else if (keyPressed->code == sf::Keyboard::Key::Num1) {
                        useInventoryItem(0, game);
                    } else if (keyPressed->code == sf::Keyboard::Key::Num2) {
                        useInventoryItem(1, game);
                    } else if (keyPressed->code == sf::Keyboard::Key::Num3) {
                        useInventoryItem(2, game);
                    } else if (keyPressed->code == sf::Keyboard::Key::Num4) {
                        useInventoryItem(3, game);
                    } else if (keyPressed->code == sf::Keyboard::Key::F5) {
                        try {
                            game.saveGame("quicksave.sav");
                            hud_->addLog("QuickSave saved: quicksave.sav");
                            hud_->addFloatingText("GAME SAVED!", player_->getPosition(), sf::Color(80, 255, 140));
                            sound_.play(SoundEffect::Interact);
                        } catch (const std::exception& e) {
                            hud_->addLog(std::string("Save error: ") + e.what());
                        }
                    } else if (keyPressed->code == sf::Keyboard::Key::F9) {
                        try {
                            game.loadGame("quicksave.sav");
                            syncEntitiesWithGameState(game);
                            hud_->addLog("QuickSave restored from quicksave.sav");
                            hud_->addFloatingText("GAME LOADED!", player_->getPosition(), sf::Color(80, 255, 140));
                            sound_.play(SoundEffect::Interact);
                        } catch (const std::exception& e) {
                            hud_->addLog(std::string("Load error: ") + e.what());
                        }
                    }
                }
            } else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                if (puzzleModal_->isOpen()) {
                    puzzleModal_->handleTextEntered(textEntered->unicode);
                }
            } else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (dialog_->isOpen()) {
                    dialog_->handleMouseClick(mousePressed->position, game, sound_, *hud_, window_);
                    syncEntitiesWithGameState(game);
                } else if (puzzleModal_->isOpen()) {
                    puzzleModal_->handleMouseClick(mousePressed->position, game, sound_);
                    syncEntitiesWithGameState(game);
                }
            }
        }

        // 2. Update Game State
        if (dialog_->isOpen()) {
            dialog_->update(dt);
        } else if (puzzleModal_->isOpen()) {
            puzzleModal_->update(dt);
        } else {
            // Player movement
            player_->handleInput(dt);
            player_->update(dt, sound_);

            // Check Trap Collisions
            sf::FloatRect playerBounds = player_->getBounds();
            auto it = roomEntities_.find(curRoom);
            if (it != roomEntities_.end()) {
                for (auto& entity : it->second) {
                    entity->update(dt);

                    if (entity->getType() == EntityType::TrapZone && entity->isActive()) {
                        auto trap = std::dynamic_pointer_cast<TrapZoneEntity>(entity);
                        if (trap && !trap->isTriggered() && trap->getBounds().findIntersection(playerBounds).has_value()) {
                            trap->trigger();

                            // Trap evasion check
                            if (std::rand() % 2 == 0) {
                                // Evasion success
                                game.getPlayer().addScore(2);
                                hud_->addLog("EVALUATION: Safe from " + trap->getName() + "! (+2 pts)");
                                hud_->addFloatingText("TRAP EVADED! +2 pts", player_->getPosition(), sf::Color(255, 215, 60));
                                sound_.play(SoundEffect::Interact);
                            } else {
                                // Damage taken
                                int dmg = trap->getDamage();
                                game.getPlayer().takeDamage(dmg);
                                sound_.play(SoundEffect::TrapTrigger);
                                hud_->addLog("WARNING: " + trap->getName() + " struck! (-" + std::to_string(dmg) + " HP)");
                                hud_->addFloatingText("-" + std::to_string(dmg) + " HP!", player_->getPosition(), sf::Color(255, 50, 50));
                            }
                        }
                    }
                }
            }

            // Check Player Death
            if (!game.getPlayer().isAlive() && !gameOverTriggered_) {
                gameOverTriggered_ = true;
                sound_.play(SoundEffect::GameOver);
                dialog_->openGameOver();
            }

            // Check Win State: In room 4 and all registered puzzles solved
            if (curRoom == 4 && !victoryTriggered_) {
                bool allSolved = true;
                for (const auto& p : game.getPuzzles()) {
                    if (!p.second->isSolved()) {
                        allSolved = false;
                        break;
                    }
                }
                if (allSolved) {
                    victoryTriggered_ = true;
                    sound_.play(SoundEffect::Victory);
                    dialog_->openVictory();
                }
            }
        }

        visuals_->update(dt, curRoom);
        hud_->update(dt, game.getPlayer().getHealth());

        // Find nearest interactable entity for prompt
        InteractiveEntity* nearestEntity = nullptr;
        auto curEntIt = roomEntities_.find(curRoom);
        if (curEntIt != roomEntities_.end()) {
            for (auto& entity : curEntIt->second) {
                if (entity->isActive() && entity->isNear(player_->getPosition(), 65.0f)) {
                    nearestEntity = entity.get();
                    break;
                }
            }
        }

        // 3. Render Pipeline
        window_.clear(sf::Color(14, 16, 22));

        // Draw Room Visuals (Floor, Walls, Thematic props, Particles, Torch Lighting)
        visuals_->render(window_, curRoom, player_->getPosition());

        // Draw Interactive Room Entities (Doors, Pedestals, Traps, Item Pickups)
        if (curEntIt != roomEntities_.end()) {
            for (auto& entity : curEntIt->second) {
                if (entity->isActive()) {
                    entity->render(window_, font_);
                }
            }
        }

        // Draw Player with Flashlight beam & Proximity prompt
        player_->render(window_, nearestEntity);

        // Draw HUD Side Panel (Health bar, Mini-map BFS Route, Inventory, Activity log, Controls)
        hud_->render(window_, game);

        // Draw Puzzle Modal if open
        if (puzzleModal_->isOpen()) {
            puzzleModal_->render(window_);
        }

        // Draw Dialogs (Pause/Save/Load, Victory, Game Over)
        if (dialog_->isOpen()) {
            dialog_->render(window_, game);
        }

        window_.display();
    }
}