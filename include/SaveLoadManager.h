#ifndef SAVE_LOAD_MANAGER_H
#define SAVE_LOAD_MANAGER_H

#include "GameExceptions.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

class GameManager;

/**
 * @brief Represents a serializable snapshot of the entire game's state.
 */
struct GameState {
    int version = 1;
    std::string timestamp;
    int currentRoomId = 0;
    int score = 0;
    int movesCount = 0;
    std::map<int, bool> roomCompleted;
    std::map<int, bool> puzzleSolved;
    std::map<int, bool> doorUnlocked;
    std::vector<int> visitedRooms;

    bool operator==(const GameState& other) const;
    bool operator!=(const GameState& other) const { return !(*this == other); }

    /**
     * @brief Validates semantic integrity of the state values.
     * @throws InvalidGameStateDataException if any rule is violated.
     */
    void validate() const;
};

/**
 * @brief Static service responsible for serializing, deserializing,
 * saving, loading, and verifying game states.
 */
class SaveLoadManager {
public:
    static constexpr int CURRENT_SAVE_VERSION = 1;
    static constexpr const char* MAGIC_HEADER = "[ESCAPE_ROOM_SAVE_V1]";
    static constexpr const char* EOF_TAG = "[EOF]";

    // Save/Load directly with GameState
    static void saveToFile(const std::string& filepath, const GameState& state);
    static GameState loadFromFile(const std::string& filepath);

    // Save/Load directly through GameManager
    static void saveGame(const std::string& filepath, const GameManager& manager);
    static void loadGame(const std::string& filepath, GameManager& manager);

    // In-memory serialization and deserialization
    static std::string serializeToString(const GameState& state);
    static GameState deserializeFromString(const std::string& data, const std::string& origin = "memory");

    // File utilities
    static bool fileExists(const std::string& filepath);
    static bool deleteSaveFile(const std::string& filepath);

    // Integrity checksum helper
    static unsigned long calculateChecksum(const std::string& content);
};

#endif // SAVE_LOAD_MANAGER_H
