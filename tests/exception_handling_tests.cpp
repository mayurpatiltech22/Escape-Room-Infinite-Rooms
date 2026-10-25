#include "Door.h"
#include "EntranceRoom.h"
#include "EscapeRoom.h"
#include "GameExceptions.h"
#include "GameManager.h"
#include "LabRoom.h"
#include "LibraryRoom.h"
#include "Room.h"
#include "SaveLoadManager.h"
#include "SecretRoom.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

static void safeRemove(const std::string& path) {
    SaveLoadManager::deleteSaveFile(path);
}

// 1. Loading non-existent file throws SaveFileNotFoundException
static void testSaveFileNotFoundException() {
    std::cout << "[TEST] Running testSaveFileNotFoundException...\n";
    const std::string missingFile = "missing_file_xyz_12345.sav";
    safeRemove(missingFile);

    bool caught = false;
    try {
        SaveLoadManager::loadFromFile(missingFile);
    } catch (const SaveFileNotFoundException& ex) {
        caught = true;
        assert(ex.getFilename() == missingFile);
        std::string msg = ex.what();
        assert(msg.find(missingFile) != std::string::npos);
    } catch (...) {
        assert(false && "Unexpected exception type thrown");
    }
    assert(caught);
    std::cout << "  -> PASSED\n";
}

// 2. Loading empty file (0 bytes) throws CorruptSaveException
static void testEmptyFileException() {
    std::cout << "[TEST] Running testEmptyFileException...\n";
    const std::string emptyFile = "empty_test.sav";
    safeRemove(emptyFile);

    // Create an empty file
    std::ofstream out(emptyFile);
    out.close();

    bool caught = false;
    try {
        SaveLoadManager::loadFromFile(emptyFile);
    } catch (const CorruptSaveException& ex) {
        caught = true;
        std::string msg = ex.what();
        assert(msg.find("empty") != std::string::npos);
    } catch (...) {
        assert(false && "Unexpected exception type thrown");
    }
    assert(caught);
    safeRemove(emptyFile);
    std::cout << "  -> PASSED\n";
}

// 3. File with invalid or missing header throws CorruptSaveException
static void testCorruptHeaderException() {
    std::cout << "[TEST] Running testCorruptHeaderException...\n";
    const std::string badHeaderFile = "bad_header.sav";
    safeRemove(badHeaderFile);

    std::ofstream out(badHeaderFile);
    out << "[CORRUPT_HEADER_TAG]\n";
    out << "VERSION:1\n";
    out << "[EOF]\n";
    out.close();

    bool caught = false;
    try {
        SaveLoadManager::loadFromFile(badHeaderFile);
    } catch (const CorruptSaveException& ex) {
        caught = true;
        std::string msg = ex.what();
        assert(msg.find("header") != std::string::npos);
    } catch (...) {
        assert(false && "Unexpected exception type thrown");
    }
    assert(caught);
    safeRemove(badHeaderFile);
    std::cout << "  -> PASSED\n";
}

// 4. Save file version mismatch throws SaveVersionMismatchException
static void testVersionMismatchException() {
    std::cout << "[TEST] Running testVersionMismatchException...\n";
    GameState futureState;
    futureState.version = 99; // Incompatible future version
    futureState.currentRoomId = 1;

    // Serialize valid state with future version
    std::string serialized = SaveLoadManager::serializeToString(futureState);

    bool caught = false;
    try {
        SaveLoadManager::deserializeFromString(serialized, "test_version_mismatch");
    } catch (const SaveVersionMismatchException& ex) {
        caught = true;
        assert(ex.getFileVersion() == 99);
        assert(ex.getExpectedVersion() == SaveLoadManager::CURRENT_SAVE_VERSION);
    } catch (...) {
        assert(false && "Unexpected exception type thrown");
    }
    assert(caught);
    std::cout << "  -> PASSED\n";
}

// 5. Truncated save file (missing [EOF]) throws CorruptSaveException
static void testTruncatedFileException() {
    std::cout << "[TEST] Running testTruncatedFileException...\n";
    const std::string truncatedFile = "truncated.sav";
    safeRemove(truncatedFile);

    std::ofstream out(truncatedFile);
    out << SaveLoadManager::MAGIC_HEADER << "\n";
    out << "VERSION:1\n";
    out << "CURRENT_ROOM:0\n";
    // File cuts off abruptly without [CHECKSUM] and [EOF]
    out.close();

    bool caught = false;
    try {
        SaveLoadManager::loadFromFile(truncatedFile);
    } catch (const CorruptSaveException& ex) {
        caught = true;
        std::string msg = ex.what();
        assert(msg.find("truncated") != std::string::npos);
    } catch (...) {
        assert(false && "Unexpected exception type thrown");
    }
    assert(caught);
    safeRemove(truncatedFile);
    std::cout << "  -> PASSED\n";
}

// 6. Non-numeric or malformed integer values throw CorruptSaveException
static void testMalformedNumericDataException() {
    std::cout << "[TEST] Running testMalformedNumericDataException...\n";
    std::string malformedData =
        std::string(SaveLoadManager::MAGIC_HEADER) + "\n"
        "VERSION:1\n"
        "TIMESTAMP:2026-10-25\n"
        "CURRENT_ROOM:not_a_number\n"
        "SCORE:50\n"
        "MOVES_COUNT:5\n"
        "[VISITED_ROOMS]\n"
        "0\n"
        "[ROOM_COMPLETED]\n"
        "[PUZZLES_SOLVED]\n"
        "[DOORS_UNLOCKED]\n"
        "[CHECKSUM]\n"
        "1234567\n"
        "[EOF]\n";

    bool caught = false;
    try {
        SaveLoadManager::deserializeFromString(malformedData);
    } catch (const CorruptSaveException&) {
        caught = true;
    } catch (...) {
        assert(false && "Expected CorruptSaveException");
    }
    assert(caught);
    std::cout << "  -> PASSED\n";
}

// 7. Negative room ID or score throws InvalidGameStateDataException
static void testNegativeValuesException() {
    std::cout << "[TEST] Running testNegativeValuesException...\n";
    GameState badRoomState;
    badRoomState.currentRoomId = -10;

    bool caughtBadRoom = false;
    try {
        badRoomState.validate();
    } catch (const InvalidGameStateDataException&) {
        caughtBadRoom = true;
    }
    assert(caughtBadRoom);

    GameState badScoreState;
    badScoreState.score = -50;

    bool caughtBadScore = false;
    try {
        badScoreState.validate();
    } catch (const InvalidGameStateDataException&) {
        caughtBadScore = true;
    }
    assert(caughtBadScore);

    GameState badMovesState;
    badMovesState.movesCount = -1;

    bool caughtBadMoves = false;
    try {
        badMovesState.validate();
    } catch (const InvalidGameStateDataException&) {
        caughtBadMoves = true;
    }
    assert(caughtBadMoves);

    std::cout << "  -> PASSED\n";
}

// 8. Tampered save file with invalid checksum throws CorruptSaveException
static void testTamperedChecksumException() {
    std::cout << "[TEST] Running testTamperedChecksumException...\n";
    GameState original;
    original.currentRoomId = 1;
    original.score = 100;

    std::string validSaved = SaveLoadManager::serializeToString(original);

    // Tamper with data (change score 100 to 999) without updating checksum
    size_t scorePos = validSaved.find("SCORE:100");
    assert(scorePos != std::string::npos);
    validSaved.replace(scorePos, 9, "SCORE:999");

    bool caught = false;
    try {
        SaveLoadManager::deserializeFromString(validSaved, "tampered_test");
    } catch (const CorruptSaveException& ex) {
        caught = true;
        std::string msg = ex.what();
        assert(msg.find("checksum") != std::string::npos || msg.find("Checksum") != std::string::npos);
    } catch (...) {
        assert(false && "Expected CorruptSaveException on checksum mismatch");
    }
    assert(caught);
    std::cout << "  -> PASSED\n";
}

// 9. Restoring a state with an unknown room ID throws InvalidGameStateDataException
static void testInvalidRoomInGameManagerRestore() {
    std::cout << "[TEST] Running testInvalidRoomInGameManagerRestore...\n";
    GameManager game;
    game.addRoom(std::make_unique<EntranceRoom>()); // Only Room ID 0 registered

    GameState invalidState;
    invalidState.currentRoomId = 999; // Room 999 not registered

    bool caught = false;
    try {
        game.restoreState(invalidState);
    } catch (const InvalidGameStateDataException& ex) {
        caught = true;
        std::string msg = ex.what();
        assert(msg.find("999") != std::string::npos);
    } catch (...) {
        assert(false && "Expected InvalidGameStateDataException");
    }
    assert(caught);
    std::cout << "  -> PASSED\n";
}

// 10. Strong Exception Safety: Failed load leaves original state completely unmodified
static void testStrongExceptionSafetyOnLoadFailure() {
    std::cout << "[TEST] Running testStrongExceptionSafetyOnLoadFailure...\n";
    GameManager game;
    game.addRoom(std::make_unique<EntranceRoom>());
    game.addRoom(std::make_unique<LibraryRoom>());
    game.connectRooms(0, 1);

    // Initial state: move to room 1, score 80
    game.moveToRoom(1);
    game.addScore(80);
    assert(game.getCurrentRoomId() == 1);
    assert(game.getScore() == 80);
    assert(game.getMovesCount() == 1);

    // Attempt to load corrupt state (invalid room 888)
    GameState corruptState;
    corruptState.currentRoomId = 888;
    corruptState.score = 5000;
    corruptState.movesCount = 99;

    bool caught = false;
    try {
        game.restoreState(corruptState);
    } catch (const InvalidGameStateDataException&) {
        caught = true;
    }
    assert(caught);

    // Verify GameManager state was NOT modified or corrupted
    assert(game.getCurrentRoomId() == 1);
    assert(game.getScore() == 80);
    assert(game.getMovesCount() == 1);
    std::cout << "  -> PASSED\n";
}

// 11. GameManager input validation and precondition exceptions
static void testGameManagerPreconditionExceptions() {
    std::cout << "[TEST] Running testGameManagerPreconditionExceptions...\n";
    GameManager game;

    // Null room check
    bool caughtNull = false;
    try {
        game.addRoom(nullptr);
    } catch (const std::invalid_argument&) {
        caughtNull = true;
    }
    assert(caughtNull);

    // Duplicate room check
    game.addRoom(std::make_unique<EntranceRoom>());
    bool caughtDuplicate = false;
    try {
        game.addRoom(std::make_unique<EntranceRoom>());
    } catch (const std::invalid_argument&) {
        caughtDuplicate = true;
    }
    assert(caughtDuplicate);

    // Connect non-existent rooms check
    bool caughtBadConnect = false;
    try {
        game.connectRooms(0, 999);
    } catch (const std::invalid_argument&) {
        caughtBadConnect = true;
    }
    assert(caughtBadConnect);

    // Query non-existent room status check
    bool caughtRoomNotFound = false;
    try {
        game.isRoomCompleted(999);
    } catch (const RoomNotFoundException& ex) {
        caughtRoomNotFound = true;
        assert(ex.getRoomId() == 999);
    }
    assert(caughtRoomNotFound);

    std::cout << "  -> PASSED\n";
}

// 12. Polymorphic exception handling verification
static void testPolymorphicCatching() {
    std::cout << "[TEST] Running testPolymorphicCatching...\n";

    // 12a. Catch SaveFileNotFoundException through SaveLoadException reference
    bool caughtAsSaveLoad = false;
    try {
        throw SaveFileNotFoundException("missing.sav");
    } catch (const SaveLoadException& ex) {
        caughtAsSaveLoad = true;
        assert(ex.getFilename() == "missing.sav");
    }
    assert(caughtAsSaveLoad);

    // 12b. Catch CorruptSaveException through GameException reference
    bool caughtAsGameException = false;
    try {
        throw CorruptSaveException("checksum failed", "corrupt.sav");
    } catch (const GameException& ex) {
        caughtAsGameException = true;
        std::string msg = ex.what();
        assert(msg.find("checksum") != std::string::npos);
    }
    assert(caughtAsGameException);

    // 12c. Catch FileWriteException through standard std::exception reference
    bool caughtAsStdException = false;
    try {
        throw FileWriteException("disk full", "save.sav");
    } catch (const std::exception& ex) {
        caughtAsStdException = true;
        std::string msg = ex.what();
        assert(msg.find("disk full") != std::string::npos);
    }
    assert(caughtAsStdException);

    std::cout << "  -> PASSED\n";
}

// 13. Empty path validation
static void testEmptyPathExceptions() {
    std::cout << "[TEST] Running testEmptyPathExceptions...\n";
    GameState state;

    bool caughtSave = false;
    try {
        SaveLoadManager::saveToFile("", state);
    } catch (const FileWriteException&) {
        caughtSave = true;
    }
    assert(caughtSave);

    bool caughtLoad = false;
    try {
        SaveLoadManager::loadFromFile("");
    } catch (const SaveFileNotFoundException&) {
        caughtLoad = true;
    }
    assert(caughtLoad);

    std::cout << "  -> PASSED\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "  RUNNING EXCEPTION-HANDLING TESTS\n";
    std::cout << "========================================\n";

    testSaveFileNotFoundException();
    testEmptyFileException();
    testCorruptHeaderException();
    testVersionMismatchException();
    testTruncatedFileException();
    testMalformedNumericDataException();
    testNegativeValuesException();
    testTamperedChecksumException();
    testInvalidRoomInGameManagerRestore();
    testStrongExceptionSafetyOnLoadFailure();
    testGameManagerPreconditionExceptions();
    testPolymorphicCatching();
    testEmptyPathExceptions();

    std::cout << "========================================\n";
    std::cout << "  ALL EXCEPTION-HANDLING TESTS PASSED!\n";
    std::cout << "========================================\n";
    return 0;
}
