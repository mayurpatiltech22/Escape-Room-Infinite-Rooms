#include "SaveLoadManager.h"
#include "GameManager.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <sstream>

// ==========================================
// GameState Implementation
// ==========================================

bool GameState::operator==(const GameState& other) const {
    return version == other.version &&
           currentRoomId == other.currentRoomId &&
           score == other.score &&
           movesCount == other.movesCount &&
           roomCompleted == other.roomCompleted &&
           puzzleSolved == other.puzzleSolved &&
           doorUnlocked == other.doorUnlocked &&
           visitedRooms == other.visitedRooms;
}

void GameState::validate() const {
    if (version <= 0) {
        throw InvalidGameStateDataException("Save version must be positive.");
    }
    if (currentRoomId < 0) {
        throw InvalidGameStateDataException("Current room ID cannot be negative.");
    }
    if (score < 0) {
        throw InvalidGameStateDataException("Score cannot be negative.");
    }
    if (movesCount < 0) {
        throw InvalidGameStateDataException("Moves count cannot be negative.");
    }

    for (const auto& pair : roomCompleted) {
        if (pair.first < 0) {
            throw InvalidGameStateDataException("Room ID cannot be negative: " + std::to_string(pair.first));
        }
    }
    for (const auto& pair : puzzleSolved) {
        if (pair.first < 0) {
            throw InvalidGameStateDataException("Puzzle ID cannot be negative: " + std::to_string(pair.first));
        }
    }
    for (const auto& pair : doorUnlocked) {
        if (pair.first < 0) {
            throw InvalidGameStateDataException("Door ID cannot be negative: " + std::to_string(pair.first));
        }
    }
    for (int visitedId : visitedRooms) {
        if (visitedId < 0) {
            throw InvalidGameStateDataException("Visited room ID cannot be negative: " + std::to_string(visitedId));
        }
    }
}

// ==========================================
// SaveLoadManager Implementation
// ==========================================

unsigned long SaveLoadManager::calculateChecksum(const std::string& content) {
    // DJB2 hash algorithm, ignoring '\r' to be platform and line-ending agnostic
    unsigned long hash = 5381;
    for (char c : content) {
        if (c == '\r') {
            continue;
        }
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    return hash;
}

bool SaveLoadManager::fileExists(const std::string& filepath) {
    std::ifstream stream(filepath);
    return stream.good();
}

bool SaveLoadManager::deleteSaveFile(const std::string& filepath) {
    return std::remove(filepath.c_str()) == 0;
}

std::string SaveLoadManager::serializeToString(const GameState& state) {
    state.validate();

    std::ostringstream payload;
    payload << MAGIC_HEADER << "\n";
    payload << "VERSION:" << state.version << "\n";
    payload << "TIMESTAMP:" << (state.timestamp.empty() ? "UNSET" : state.timestamp) << "\n";
    payload << "CURRENT_ROOM:" << state.currentRoomId << "\n";
    payload << "SCORE:" << state.score << "\n";
    payload << "MOVES_COUNT:" << state.movesCount << "\n";

    payload << "[VISITED_ROOMS]\n";
    for (size_t i = 0; i < state.visitedRooms.size(); ++i) {
        payload << state.visitedRooms[i];
        if (i + 1 < state.visitedRooms.size()) {
            payload << ",";
        }
    }
    payload << "\n";

    payload << "[ROOM_COMPLETED]\n";
    for (const auto& pair : state.roomCompleted) {
        payload << pair.first << "=" << (pair.second ? 1 : 0) << "\n";
    }

    payload << "[PUZZLES_SOLVED]\n";
    for (const auto& pair : state.puzzleSolved) {
        payload << pair.first << "=" << (pair.second ? 1 : 0) << "\n";
    }

    payload << "[DOORS_UNLOCKED]\n";
    for (const auto& pair : state.doorUnlocked) {
        payload << pair.first << "=" << (pair.second ? 1 : 0) << "\n";
    }

    std::string body = payload.str();
    unsigned long checksum = calculateChecksum(body);

    std::ostringstream result;
    result << body;
    result << "[CHECKSUM]\n" << checksum << "\n";
    result << EOF_TAG << "\n";

    return result.str();
}

GameState SaveLoadManager::deserializeFromString(const std::string& data, const std::string& origin) {
    if (data.empty()) {
        throw CorruptSaveException("Save data is completely empty.", origin);
    }

    // Split into individual lines (stripping '\r')
    std::vector<std::string> lines;
    std::istringstream stream(data);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }

    if (lines.empty()) {
        throw CorruptSaveException("Save data contains no lines.", origin);
    }

    // Filter out trailing blank lines to check EOF marker
    while (!lines.empty() && lines.back().empty()) {
        lines.pop_back();
    }

    if (lines.empty() || lines.back() != EOF_TAG) {
        throw CorruptSaveException("File is truncated; missing [EOF] marker.", origin);
    }

    // Find magic header
    if (lines.front() != MAGIC_HEADER) {
        throw CorruptSaveException("Invalid or missing save file header: " + lines.front(), origin);
    }

    // Find [CHECKSUM] block
    int checksumLineIndex = -1;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i] == "[CHECKSUM]") {
            checksumLineIndex = static_cast<int>(i);
            break;
        }
    }

    if (checksumLineIndex == -1 || checksumLineIndex + 1 >= static_cast<int>(lines.size())) {
        throw CorruptSaveException("Missing [CHECKSUM] block or value.", origin);
    }

    unsigned long expectedChecksum = 0;
    try {
        expectedChecksum = std::stoul(lines[checksumLineIndex + 1]);
    } catch (...) {
        throw CorruptSaveException("Malformed checksum numerical value.", origin);
    }

    // Reconstruct body (from line 0 to checksumLineIndex - 1)
    std::ostringstream bodyStream;
    for (int i = 0; i < checksumLineIndex; ++i) {
        bodyStream << lines[i] << "\n";
    }
    std::string body = bodyStream.str();
    unsigned long computedChecksum = calculateChecksum(body);

    if (computedChecksum != expectedChecksum) {
        throw CorruptSaveException("Checksum verification failed (file contents have been tampered with or corrupted).", origin);
    }

    // Parse payload body
    GameState state;
    enum class Section { HEADER, VISITED_ROOMS, ROOMS, PUZZLES, DOORS };
    Section currentSection = Section::HEADER;

    for (int i = 1; i < checksumLineIndex; ++i) {
        const std::string& cur = lines[i];
        if (cur.empty()) {
            continue;
        }

        if (cur == "[VISITED_ROOMS]") {
            currentSection = Section::VISITED_ROOMS;
            continue;
        } else if (cur == "[ROOM_COMPLETED]") {
            currentSection = Section::ROOMS;
            continue;
        } else if (cur == "[PUZZLES_SOLVED]") {
            currentSection = Section::PUZZLES;
            continue;
        } else if (cur == "[DOORS_UNLOCKED]") {
            currentSection = Section::DOORS;
            continue;
        }

        if (currentSection == Section::HEADER) {
            auto colonPos = cur.find(':');
            if (colonPos == std::string::npos) {
                throw CorruptSaveException("Malformed header entry (missing ':'): " + cur, origin);
            }
            std::string key = cur.substr(0, colonPos);
            std::string val = cur.substr(colonPos + 1);

            try {
                if (key == "VERSION") {
                    int ver = std::stoi(val);
                    if (ver <= 0) {
                        throw CorruptSaveException("Invalid version number in save file.", origin);
                    }
                    if (ver != CURRENT_SAVE_VERSION) {
                        throw SaveVersionMismatchException(ver, CURRENT_SAVE_VERSION, origin);
                    }
                    state.version = ver;
                } else if (key == "TIMESTAMP") {
                    state.timestamp = val;
                } else if (key == "CURRENT_ROOM") {
                    state.currentRoomId = std::stoi(val);
                    if (state.currentRoomId < 0) {
                        throw InvalidGameStateDataException("Current room ID cannot be negative.", origin);
                    }
                } else if (key == "SCORE") {
                    state.score = std::stoi(val);
                    if (state.score < 0) {
                        throw InvalidGameStateDataException("Score cannot be negative.", origin);
                    }
                } else if (key == "MOVES_COUNT") {
                    state.movesCount = std::stoi(val);
                    if (state.movesCount < 0) {
                        throw InvalidGameStateDataException("Moves count cannot be negative.", origin);
                    }
                }
            } catch (const SaveLoadException&) {
                throw;
            } catch (const std::exception&) {
                throw CorruptSaveException("Failed to parse numeric header field: " + cur, origin);
            }
        } else if (currentSection == Section::VISITED_ROOMS) {
            std::istringstream visitedStream(cur);
            std::string token;
            while (std::getline(visitedStream, token, ',')) {
                if (token.empty()) continue;
                try {
                    int roomId = std::stoi(token);
                    if (roomId < 0) {
                        throw InvalidGameStateDataException("Visited room ID cannot be negative.", origin);
                    }
                    state.visitedRooms.push_back(roomId);
                } catch (const SaveLoadException&) {
                    throw;
                } catch (...) {
                    throw CorruptSaveException("Invalid visited room ID format: " + token, origin);
                }
            }
        } else {
            // Key=Value parsing for rooms, puzzles, and doors
            auto eqPos = cur.find('=');
            if (eqPos == std::string::npos) {
                throw CorruptSaveException("Malformed section entry (missing '='): " + cur, origin);
            }
            try {
                int id = std::stoi(cur.substr(0, eqPos));
                int status = std::stoi(cur.substr(eqPos + 1));
                if (id < 0) {
                    throw InvalidGameStateDataException("ID cannot be negative: " + std::to_string(id), origin);
                }
                if (status != 0 && status != 1) {
                    throw CorruptSaveException("Boolean state value must be 0 or 1: " + cur, origin);
                }

                if (currentSection == Section::ROOMS) {
                    state.roomCompleted[id] = (status == 1);
                } else if (currentSection == Section::PUZZLES) {
                    state.puzzleSolved[id] = (status == 1);
                } else if (currentSection == Section::DOORS) {
                    state.doorUnlocked[id] = (status == 1);
                }
            } catch (const SaveLoadException&) {
                throw;
            } catch (...) {
                throw CorruptSaveException("Failed to parse ID/value mapping: " + cur, origin);
            }
        }
    }

    state.validate();
    return state;
}

void SaveLoadManager::saveToFile(const std::string& filepath, const GameState& state) {
    if (filepath.empty()) {
        throw FileWriteException("File path cannot be empty.", filepath);
    }

    state.validate();

    std::ofstream outFile(filepath, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        throw FileWriteException("Failed to open file for writing (bad path or insufficient permissions).", filepath);
    }

    std::string serialized = serializeToString(state);
    outFile << serialized;

    if (!outFile) {
        throw FileWriteException("I/O error occurred while writing save data.", filepath);
    }

    outFile.close();
}

GameState SaveLoadManager::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        throw SaveFileNotFoundException("File path cannot be empty.");
    }

    std::ifstream inFile(filepath, std::ios::in | std::ios::binary);
    if (!inFile.is_open()) {
        throw SaveFileNotFoundException(filepath);
    }

    // Check file size for 0 bytes
    inFile.seekg(0, std::ios::end);
    std::streamsize size = inFile.tellg();
    if (size <= 0) {
        inFile.close();
        throw CorruptSaveException("Save file is empty (0 bytes).", filepath);
    }
    inFile.seekg(0, std::ios::beg);

    std::string content((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
    inFile.close();

    return deserializeFromString(content, filepath);
}

void SaveLoadManager::saveGame(const std::string& filepath, const GameManager& manager) {
    saveToFile(filepath, manager.captureState());
}

void SaveLoadManager::loadGame(const std::string& filepath, GameManager& manager) {
    GameState loaded = loadFromFile(filepath);
    manager.restoreState(loaded);
}
