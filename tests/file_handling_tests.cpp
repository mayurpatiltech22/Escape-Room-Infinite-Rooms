#include "Door.h"
#include "EntranceRoom.h"
#include "EscapeRoom.h"
#include "GameExceptions.h"
#include "GameManager.h"
#include "LabRoom.h"
#include "LibraryRoom.h"
#include "Puzzle.h"
#include "Room.h"
#include "SaveLoadManager.h"
#include "SecretRoom.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

// Helper to remove files safely
static void safeRemove(const std::string& path) {
    SaveLoadManager::deleteSaveFile(path);
}

// 1. Test basic file creation and existence on disk
static void testSaveFileCreation() {
    std::cout << "[TEST] Running testSaveFileCreation...\n";
    const std::string filename = "test_create.sav";
    safeRemove(filename);

    GameState state;
    state.currentRoomId = 1;
    state.score = 100;
    state.movesCount = 5;
    state.timestamp = "2026-10-25 21:00";

    SaveLoadManager::saveToFile(filename, state);

    assert(SaveLoadManager::fileExists(filename));

    // Verify non-zero file size
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    assert(file.is_open());
    std::streamsize size = file.tellg();
    assert(size > 0);
    file.close();

    safeRemove(filename);
    assert(!SaveLoadManager::fileExists(filename));
    std::cout << "  -> PASSED\n";
}

// 2. Test saving and loading an exact state round-trip
static void testSaveAndLoadExactRoundTrip() {
    std::cout << "[TEST] Running testSaveAndLoadExactRoundTrip...\n";
    const std::string filename = "test_roundtrip.sav";
    safeRemove(filename);

    GameState original;
    original.version = 1;
    original.timestamp = "2026-10-25 21:30:00";
    original.currentRoomId = 3;
    original.score = 250;
    original.movesCount = 14;
    original.visitedRooms = {0, 1, 3};
    original.roomCompleted = {{0, true}, {1, true}, {2, false}, {3, true}, {4, false}};
    original.puzzleSolved = {{1, true}, {2, false}, {10, true}};
    original.doorUnlocked = {{7, true}, {8, false}};

    SaveLoadManager::saveToFile(filename, original);

    GameState loaded = SaveLoadManager::loadFromFile(filename);

    assert(loaded.version == original.version);
    assert(loaded.timestamp == original.timestamp);
    assert(loaded.currentRoomId == original.currentRoomId);
    assert(loaded.score == original.score);
    assert(loaded.movesCount == original.movesCount);
    assert(loaded.visitedRooms == original.visitedRooms);
    assert(loaded.roomCompleted == original.roomCompleted);
    assert(loaded.puzzleSolved == original.puzzleSolved);
    assert(loaded.doorUnlocked == original.doorUnlocked);
    assert(loaded == original);

    safeRemove(filename);
    std::cout << "  -> PASSED\n";
}

// 3. Test overwriting an existing save file
static void testFileOverwrite() {
    std::cout << "[TEST] Running testFileOverwrite...\n";
    const std::string filename = "test_overwrite.sav";
    safeRemove(filename);

    GameState state1;
    state1.currentRoomId = 1;
    state1.score = 50;
    state1.movesCount = 2;
    state1.timestamp = "FIRST_SAVE";
    SaveLoadManager::saveToFile(filename, state1);

    GameState loaded1 = SaveLoadManager::loadFromFile(filename);
    assert(loaded1.score == 50);
    assert(loaded1.timestamp == "FIRST_SAVE");

    // Overwrite with state2
    GameState state2;
    state2.currentRoomId = 4;
    state2.score = 999;
    state2.movesCount = 25;
    state2.timestamp = "OVERWRITTEN_SAVE";
    SaveLoadManager::saveToFile(filename, state2);

    GameState loaded2 = SaveLoadManager::loadFromFile(filename);
    assert(loaded2.currentRoomId == 4);
    assert(loaded2.score == 999);
    assert(loaded2.movesCount == 25);
    assert(loaded2.timestamp == "OVERWRITTEN_SAVE");

    safeRemove(filename);
    std::cout << "  -> PASSED\n";
}

// 4. Test multiple distinct save slots
static void testMultipleSaveSlots() {
    std::cout << "[TEST] Running testMultipleSaveSlots...\n";
    const std::string slot1 = "slot1.sav";
    const std::string slot2 = "slot2.sav";
    const std::string slot3 = "slot3.sav";
    safeRemove(slot1);
    safeRemove(slot2);
    safeRemove(slot3);

    GameState s1; s1.currentRoomId = 0; s1.score = 10;
    GameState s2; s2.currentRoomId = 1; s2.score = 20;
    GameState s3; s3.currentRoomId = 2; s3.score = 30;

    SaveLoadManager::saveToFile(slot1, s1);
    SaveLoadManager::saveToFile(slot2, s2);
    SaveLoadManager::saveToFile(slot3, s3);

    assert(SaveLoadManager::fileExists(slot1));
    assert(SaveLoadManager::fileExists(slot2));
    assert(SaveLoadManager::fileExists(slot3));

    GameState r1 = SaveLoadManager::loadFromFile(slot1);
    GameState r2 = SaveLoadManager::loadFromFile(slot2);
    GameState r3 = SaveLoadManager::loadFromFile(slot3);

    assert(r1.score == 10 && r1.currentRoomId == 0);
    assert(r2.score == 20 && r2.currentRoomId == 1);
    assert(r3.score == 30 && r3.currentRoomId == 2);

    safeRemove(slot1);
    safeRemove(slot2);
    safeRemove(slot3);
    std::cout << "  -> PASSED\n";
}

// 5. Test file delete functionality
static void testDeleteSaveFile() {
    std::cout << "[TEST] Running testDeleteSaveFile...\n";
    const std::string filename = "test_del.sav";
    safeRemove(filename);

    GameState state;
    SaveLoadManager::saveToFile(filename, state);
    assert(SaveLoadManager::fileExists(filename));

    bool deleted = SaveLoadManager::deleteSaveFile(filename);
    assert(deleted);
    assert(!SaveLoadManager::fileExists(filename));

    // Deleting non-existent file returns false
    bool secondDelete = SaveLoadManager::deleteSaveFile("non_existent_file.sav");
    assert(!secondDelete);

    std::cout << "  -> PASSED\n";
}

// 6. Test file stream closure and RAII
static void testFileStreamClosedProperly() {
    std::cout << "[TEST] Running testFileStreamClosedProperly...\n";
    const std::string filename = "test_stream_closed.sav";
    safeRemove(filename);

    GameState state;
    state.currentRoomId = 2;
    SaveLoadManager::saveToFile(filename, state);

    // Reading immediately with an external stream should succeed with no file lock
    std::ifstream file(filename);
    assert(file.is_open());
    std::string line;
    std::getline(file, line);
    assert(line == SaveLoadManager::MAGIC_HEADER);
    file.close();

    // Deleting immediately should succeed without file lock errors
    assert(SaveLoadManager::deleteSaveFile(filename));
    std::cout << "  -> PASSED\n";
}

// 7. Test GameManager end-to-end integration
static void testGameManagerEndToEndPersistence() {
    std::cout << "[TEST] Running testGameManagerEndToEndPersistence...\n";
    const std::string filename = "test_gm_persistence.sav";
    safeRemove(filename);

    // Setup active game
    GameManager originalGame;
    originalGame.addRoom(std::make_unique<EntranceRoom>());
    originalGame.addRoom(std::make_unique<LibraryRoom>());
    originalGame.addRoom(std::make_unique<LabRoom>());
    originalGame.addRoom(std::make_unique<SecretRoom>());
    originalGame.addRoom(std::make_unique<EscapeRoom>());

    originalGame.connectRooms(0, 1);
    originalGame.connectRooms(0, 2);
    originalGame.connectRooms(1, 3);
    originalGame.connectRooms(2, 3);
    originalGame.connectRooms(3, 4);

    // Perform game actions
    bool moved = originalGame.moveToRoom(1);
    assert(moved);
    assert(originalGame.getCurrentRoomId() == 1);
    assert(originalGame.getMovesCount() == 1);

    originalGame.setRoomCompleted(0, true);
    originalGame.addScore(75);

    // Save to disk
    originalGame.saveGame(filename);
    assert(SaveLoadManager::fileExists(filename));

    // Create a fresh game instance
    GameManager freshGame;
    freshGame.addRoom(std::make_unique<EntranceRoom>());
    freshGame.addRoom(std::make_unique<LibraryRoom>());
    freshGame.addRoom(std::make_unique<LabRoom>());
    freshGame.addRoom(std::make_unique<SecretRoom>());
    freshGame.addRoom(std::make_unique<EscapeRoom>());

    freshGame.connectRooms(0, 1);
    freshGame.connectRooms(0, 2);
    freshGame.connectRooms(1, 3);
    freshGame.connectRooms(2, 3);
    freshGame.connectRooms(3, 4);

    // Fresh game should start at room 0, score 0, moves 0
    assert(freshGame.getCurrentRoomId() == 0);
    assert(freshGame.getScore() == 0);
    assert(freshGame.getMovesCount() == 0);
    assert(!freshGame.isRoomCompleted(0));

    // Load saved game
    freshGame.loadGame(filename);

    // Verify restored state matches original game exactly
    assert(freshGame.getCurrentRoomId() == 1);
    assert(freshGame.getScore() == 75);
    assert(freshGame.getMovesCount() == 1);
    assert(freshGame.isRoomCompleted(0));
    assert(!freshGame.isRoomCompleted(1));

    safeRemove(filename);
    std::cout << "  -> PASSED\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "  RUNNING FILE-HANDLING TESTS\n";
    std::cout << "========================================\n";

    testSaveFileCreation();
    testSaveAndLoadExactRoundTrip();
    testFileOverwrite();
    testMultipleSaveSlots();
    testDeleteSaveFile();
    testFileStreamClosedProperly();
    testGameManagerEndToEndPersistence();

    std::cout << "========================================\n";
    std::cout << "  ALL FILE-HANDLING TESTS PASSED!\n";
    std::cout << "========================================\n";
    return 0;
}
