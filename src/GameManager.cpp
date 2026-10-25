#include "GameManager.h"
#include "GameExceptions.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

using namespace std;

GameManager::GameManager()
    : currentRoomId(0),
      score(0),
      movesCount(0) {
    visitedRooms.push_back(0);
}

void GameManager::addRoom(unique_ptr<Room> room) {
    if (!room) {
        throw invalid_argument("Cannot add a null room.");
    }

    int id = room->getId();

    if (rooms.find(id) != rooms.end()) {
        throw invalid_argument("A room with this ID already exists.");
    }

    rooms.emplace(id, move(room));
}

void GameManager::connectRooms(int roomA, int roomB) {
    if (rooms.find(roomA) == rooms.end() || rooms.find(roomB) == rooms.end()) {
        throw invalid_argument("Both rooms must exist before connecting them.");
    }

    roomGraph.addConnection(roomA, roomB);
}

bool GameManager::moveToRoom(int destinationRoomId) {
    if (rooms.find(destinationRoomId) == rooms.end()) {
        cout << "Error: This room does not exist.\n";
        return false;
    }

    if (!roomGraph.isConnected(currentRoomId, destinationRoomId)) {
        cout << "You cannot move directly to that room.\n";
        cout << "Choose one of the connected rooms.\n";
        return false;
    }

    currentRoomId = destinationRoomId;
    movesCount++;
    visitedRooms.push_back(destinationRoomId);

    cout << "\nMoving to " << rooms.at(currentRoomId)->getName() << "...\n";
    displayCurrentRoom();

    return true;
}

void GameManager::displayCurrentRoom() const {
    auto it = rooms.find(currentRoomId);

    if (it == rooms.end()) {
        cout << "Current room is unavailable.\n";
        return;
    }

    it->second->displayInfo();
    it->second->enterRoom();
}

void GameManager::displayAvailableRooms() const {
    cout << "\nAvailable connected rooms:\n";

    vector<int> neighbors = roomGraph.getNeighbors(currentRoomId);

    if (neighbors.empty()) {
        cout << "No connected rooms found.\n";
        return;
    }

    for (int id : neighbors) {
        cout << "ID: " << id
             << " | Name: " << rooms.at(id)->getName() << '\n';
    }
}

void GameManager::displayPathToRoom(int destinationRoomId) const {
    if (rooms.find(destinationRoomId) == rooms.end()) {
        cout << "Error: Destination room does not exist.\n";
        return;
    }

    vector<int> path = roomGraph.findPath(currentRoomId, destinationRoomId);

    if (path.empty()) {
        cout << "No path exists to the selected room.\n";
        return;
    }

    cout << "\nShortest path: ";

    for (size_t i = 0; i < path.size(); i++) {
        cout << rooms.at(path[i])->getName();

        if (i + 1 < path.size()) {
            cout << " -> ";
        }
    }

    cout << "\n";
}

int GameManager::getCurrentRoomId() const {
    return currentRoomId;
}

int GameManager::getScore() const {
    return score;
}

int GameManager::getMovesCount() const {
    return movesCount;
}

const vector<int>& GameManager::getVisitedRooms() const {
    return visitedRooms;
}

const RoomGraph& GameManager::getRoomGraph() const {
    return roomGraph;
}

Room* GameManager::getRoom(int id) const {
    auto it = rooms.find(id);
    if (it == rooms.end()) {
        return nullptr;
    }
    return it->second.get();
}

bool GameManager::isRoomCompleted(int id) const {
    auto it = rooms.find(id);
    if (it == rooms.end()) {
        throw RoomNotFoundException(id);
    }
    return it->second->isCompleted();
}

void GameManager::setRoomCompleted(int id, bool completed) {
    auto it = rooms.find(id);
    if (it == rooms.end()) {
        throw RoomNotFoundException(id);
    }
    it->second->setCompleted(completed);
}

void GameManager::addScore(int points) {
    if (points > 0) {
        score += points;
    }
}

GameState GameManager::captureState() const {
    GameState state;
    state.version = SaveLoadManager::CURRENT_SAVE_VERSION;
    state.timestamp = "AUTO_SAVE";
    state.currentRoomId = currentRoomId;
    state.score = score;
    state.movesCount = movesCount;
    state.visitedRooms = visitedRooms;

    for (const auto& pair : rooms) {
        state.roomCompleted[pair.first] = pair.second->isCompleted();
    }

    return state;
}

void GameManager::restoreState(const GameState& state) {
    // Validate semantic integrity first
    state.validate();

    // Verify current room exists in registered rooms
    if (rooms.find(state.currentRoomId) == rooms.end()) {
        throw InvalidGameStateDataException(
            "Target current room ID " + to_string(state.currentRoomId) + " does not exist in registered rooms.");
    }

    // Verify all saved room IDs exist in registered rooms
    for (const auto& pair : state.roomCompleted) {
        if (rooms.find(pair.first) == rooms.end()) {
            throw InvalidGameStateDataException(
                "Saved room ID " + to_string(pair.first) + " does not exist in registered rooms.");
        }
    }

    // Strong Exception Safety: Commit state changes only after all validation passes
    currentRoomId = state.currentRoomId;
    score = state.score;
    movesCount = state.movesCount;
    visitedRooms = state.visitedRooms;

    for (const auto& pair : state.roomCompleted) {
        rooms[pair.first]->setCompleted(pair.second);
    }
}

void GameManager::saveGame(const string& filename) const {
    SaveLoadManager::saveGame(filename, *this);
}

void GameManager::loadGame(const string& filename) {
    SaveLoadManager::loadGame(filename, *this);
}

void GameManager::run() {
    if (rooms.find(currentRoomId) == rooms.end()) {
        throw runtime_error("Starting room with ID 0 has not been registered.");
    }

    cout << "\nWelcome to Escape Room: Infinite Rooms!\n";

    displayCurrentRoom();

    bool running = true;

    while (running) {
        cout << "\n========== GAME MENU ==========\n";
        cout << "1. View current room\n";
        cout << "2. View connected rooms\n";
        cout << "3. Move to a room\n";
        cout << "4. Find shortest path\n";
        cout << "5. Save game\n";
        cout << "6. Load game\n";
        cout << "7. Exit game\n";
        cout << "Choose an option: ";

        int choice;

        if (!(cin >> choice)) {
            if (cin.eof()) {
                cout << "\nInput ended. Exiting game.\n";
                break;
            }

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Invalid input. Enter a number from 1 to 7.\n";
            continue;
        }

        switch (choice) {
            case 1:
                displayCurrentRoom();
                break;

            case 2:
                displayAvailableRooms();
                break;

            case 3: {
                int destination;

                cout << "Enter destination room ID: ";

                if (!(cin >> destination)) {
                    if (cin.eof()) {
                        cout << "\nInput ended. Exiting game.\n";
                        running = false;
                        break;
                    }

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    cout << "Invalid room ID.\n";
                    break;
                }

                moveToRoom(destination);
                break;
            }

            case 4: {
                int destination;

                cout << "Enter destination room ID: ";

                if (!(cin >> destination)) {
                    if (cin.eof()) {
                        cout << "\nInput ended. Exiting game.\n";
                        running = false;
                        break;
                    }

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    cout << "Invalid room ID.\n";
                    break;
                }

                displayPathToRoom(destination);
                break;
            }

            case 5: {
                string filename;
                cout << "Enter save filename (e.g. quicksave.sav): ";
                cin >> filename;
                try {
                    saveGame(filename);
                    cout << "Game saved successfully to " << filename << "!\n";
                } catch (const SaveLoadException& err) {
                    cerr << "Save error: " << err.what() << "\n";
                }
                break;
            }

            case 6: {
                string filename;
                cout << "Enter filename to load (e.g. quicksave.sav): ";
                cin >> filename;
                try {
                    loadGame(filename);
                    cout << "Game loaded successfully from " << filename << "!\n";
                    displayCurrentRoom();
                } catch (const SaveLoadException& err) {
                    cerr << "Load error: " << err.what() << "\n";
                }
                break;
            }

            case 7:
                running = false;
                cout << "Thanks for playing!\n";
                break;

            default:
                cout << "Invalid choice. Please choose 1 to 7.\n";
        }
    }
}
