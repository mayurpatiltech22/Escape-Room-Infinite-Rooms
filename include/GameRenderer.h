#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include "SoundSystem.h"
#include "RoomVisuals.h"
#include "PlayerEntity.h"
#include "InteractiveEntity.h"
#include "HUD.h"
#include "PuzzleModal.h"
#include "GameDialog.h"

class GameManager;

class GameRenderer {
private:
    sf::RenderWindow window_;
    sf::Font font_;
    SoundSystem sound_;
    std::unique_ptr<RoomVisuals> visuals_;
    std::unique_ptr<PlayerEntity> player_;
    std::unique_ptr<HUD> hud_;
    std::unique_ptr<PuzzleModal> puzzleModal_;
    std::unique_ptr<GameDialog> dialog_;

    std::unordered_map<int, std::vector<std::shared_ptr<InteractiveEntity>>> roomEntities_;
    int lastRoomId_;
    bool victoryTriggered_;
    bool gameOverTriggered_;

    void setupRoomEntities(GameManager& game);
    void syncEntitiesWithGameState(GameManager& game);
    void handleInteraction(GameManager& game);
    void useInventoryItem(int slotIndex, GameManager& game);

public:
    GameRenderer();
    ~GameRenderer() = default;

    bool isOpen() const;
    void runGraphicalGame(GameManager& game);
};

#endif // GAME_RENDERER_H