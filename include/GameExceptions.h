#ifndef GAME_EXCEPTIONS_H
#define GAME_EXCEPTIONS_H

#include <stdexcept>
#include <string>

/**
 * @brief Base exception class for all Escape Room game-related exceptions.
 */
class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Base exception class for save/load and file-handling operations.
 */
class SaveLoadException : public GameException {
private:
    std::string filename_;

public:
    SaveLoadException(const std::string& message, std::string filename = "")
        : GameException(filename.empty() ? message : message + " [File: " + filename + "]"),
          filename_(std::move(filename)) {}

    const std::string& getFilename() const noexcept {
        return filename_;
    }
};

/**
 * @brief Thrown when a specified save file does not exist on disk.
 */
class SaveFileNotFoundException : public SaveLoadException {
public:
    explicit SaveFileNotFoundException(const std::string& filename)
        : SaveLoadException("Save file not found: " + filename, filename) {}
};

/**
 * @brief Thrown when writing save data to disk fails (e.g. read-only, bad path, disk error).
 */
class FileWriteException : public SaveLoadException {
public:
    FileWriteException(const std::string& message, const std::string& filename = "")
        : SaveLoadException("File write error: " + message, filename) {}
};

/**
 * @brief Thrown when reading save data from disk fails.
 */
class FileReadException : public SaveLoadException {
public:
    FileReadException(const std::string& message, const std::string& filename = "")
        : SaveLoadException("File read error: " + message, filename) {}
};

/**
 * @brief Thrown when save data is corrupted, malformed, truncated, or fails checksum verification.
 */
class CorruptSaveException : public SaveLoadException {
public:
    explicit CorruptSaveException(const std::string& reason, const std::string& filename = "")
        : SaveLoadException("Corrupt save file: " + reason, filename) {}
};

/**
 * @brief Thrown when the version in the save file is incompatible with the current engine.
 */
class SaveVersionMismatchException : public SaveLoadException {
private:
    int fileVersion_;
    int expectedVersion_;

public:
    SaveVersionMismatchException(int fileVersion, int expectedVersion, const std::string& filename = "")
        : SaveLoadException("Save file version mismatch (Found: " + std::to_string(fileVersion) +
                            ", Expected: " + std::to_string(expectedVersion) + ")", filename),
          fileVersion_(fileVersion),
          expectedVersion_(expectedVersion) {}

    int getFileVersion() const noexcept { return fileVersion_; }
    int getExpectedVersion() const noexcept { return expectedVersion_; }
};

/**
 * @brief Thrown when save file values violate logical game constraints (e.g., negative ID, unknown room).
 */
class InvalidGameStateDataException : public SaveLoadException {
public:
    explicit InvalidGameStateDataException(const std::string& detail, const std::string& filename = "")
        : SaveLoadException("Invalid game state data: " + detail, filename) {}
};

/**
 * @brief Thrown when an operation references a room ID that does not exist in the game manager.
 */
class RoomNotFoundException : public GameException {
private:
    int roomId_;

public:
    explicit RoomNotFoundException(int roomId)
        : GameException("Room with ID " + std::to_string(roomId) + " does not exist."),
          roomId_(roomId) {}

    int getRoomId() const noexcept { return roomId_; }
};

#endif // GAME_EXCEPTIONS_H
