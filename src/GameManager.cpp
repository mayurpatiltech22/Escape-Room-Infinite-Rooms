#include "GameManager.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

using namespace std;

GameManager::GameManager()
    : currentRoomId(0), score(0), gameWon(false) {
}

// ======================================================
// ROOM MANAGEMENT
// ======================================================

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
    if (rooms.find(roomA) == rooms.end() ||
        rooms.find(roomB) == rooms.end()) {
        throw invalid_argument(
            "Both rooms must exist before connecting them."
        );
    }

    roomGraph.addConnection(roomA, roomB);
}

// ======================================================
// PUZZLE MANAGEMENT
// ======================================================

void GameManager::addPuzzle(unique_ptr<Puzzle> puzzle) {
    if (!puzzle) {
        throw invalid_argument("Cannot add a null puzzle.");
    }

    int puzzleId = puzzle->getId();

    if (puzzles.find(puzzleId) != puzzles.end()) {
        throw invalid_argument("A puzzle with this ID already exists.");
    }

    puzzles.emplace(puzzleId, move(puzzle));
}

void GameManager::addDoor(const Door& door) {
    int destination = door.getDestinationRoomId();

    if (rooms.find(destination) == rooms.end()) {
        throw invalid_argument(
            "The door destination room does not exist."
        );
    }

    if (doors.find(destination) != doors.end()) {
        throw invalid_argument(
            "A door for this destination room already exists."
        );
    }

    doors.emplace(destination, door);
}

void GameManager::displayPuzzles() const {
    cout << "\n========== PUZZLES ==========\n";

    if (puzzles.empty()) {
        cout << "No puzzles have been added yet.\n";
        return;
    }

    for (const auto& entry : puzzles) {
        entry.second->display();
    }
}

void GameManager::solvePuzzle(int puzzleId) {
    auto puzzleIt = puzzles.find(puzzleId);

    if (puzzleIt == puzzles.end()) {
        cout << "Error: Puzzle not found.\n";
        return;
    }

    Puzzle& puzzle = *puzzleIt->second;

    if (puzzle.isSolved()) {
        cout << "This puzzle has already been solved.\n";
        return;
    }

    cout << "\n========== SOLVE PUZZLE ==========\n";

    puzzle.display();

    cout << "Enter your answer: ";

    string answer;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!getline(cin, answer)) {
        if (cin.eof()) {
            cout << "\nInput ended.\n";
        } else {
            cin.clear();
            cout << "Could not read the answer.\n";
        }

        return;
    }

    if (!puzzle.submitAnswer(answer)) {
        cout << "\nIncorrect answer! Try again later.\n";
        return;
    }

    score += puzzle.getPoints();

    cout << "\nCorrect answer!\n";
    cout << "You earned " << puzzle.getPoints() << " points.\n";
    cout << "Total score: " << score << '\n';

    bool unlockedDoor = false;

    for (auto& entry : doors) {
        Door& door = entry.second;

        if (door.getRequiredPuzzleId() == puzzleId) {
            if (door.tryUnlock(puzzle)) {
                cout << "Door unlocked: "
                     << door.getName() << '\n';

                unlockedDoor = true;
            }
        }
    }

    if (!unlockedDoor) {
        cout << "Puzzle solved, but no registered door requires it.\n";
    }

    // Check whether the player has completed the game.
    // This also handles the case where the player is already
    // standing in the Escape Chamber when solving the last puzzle.
    if (currentRoomId == 4 && !puzzles.empty()) {
        bool allPuzzlesSolved = true;

        for (const auto& entry : puzzles) {
            if (!entry.second->isSolved()) {
                allPuzzlesSolved = false;
                break;
            }
        }

        if (allPuzzlesSolved && !gameWon) {
            gameWon = true;

            cout << "\n";
            cout << "============================================\n";
            cout << "              YOU ESCAPED!\n";
            cout << "============================================\n";
            cout << "Congratulations! You solved every puzzle.\n";
            cout << "You successfully escaped the mysterious rooms!\n";
            cout << "Final Score: " << score << '\n';
            cout << "============================================\n";
            cout << "       THANK YOU FOR PLAYING!\n";
            cout << "============================================\n";
        }
    }
}

// ======================================================
// ROOM NAVIGATION
// ======================================================

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

    // Check whether the destination room has a controlled door.
    auto doorIt = doors.find(destinationRoomId);

    if (doorIt != doors.end() && !doorIt->second.open()) {
        cout << "\nThe door is locked: "
             << doorIt->second.getName() << '\n';

        cout << "Solve the required puzzle before entering this room.\n";

        return false;
    }

    currentRoomId = destinationRoomId;

    cout << "\nMoving to "
         << rooms.at(currentRoomId)->getName()
         << "...\n";

    displayCurrentRoom();

    // Check victory after entering the Escape Chamber (Room ID 4).
    if (currentRoomId == 4 && !puzzles.empty()) {
        bool allPuzzlesSolved = true;

        for (const auto& entry : puzzles) {
            if (!entry.second->isSolved()) {
                allPuzzlesSolved = false;
                break;
            }
        }

        if (allPuzzlesSolved && !gameWon) {
            gameWon = true;

            cout << "\n";
            cout << "============================================\n";
            cout << "              YOU ESCAPED!\n";
            cout << "============================================\n";
            cout << "Congratulations! You solved every puzzle.\n";
            cout << "You successfully escaped the mysterious rooms!\n";
            cout << "Final Score: " << score << '\n';
            cout << "============================================\n";
            cout << "       THANK YOU FOR PLAYING!\n";
            cout << "============================================\n";
        }
    }

    return true;
}

// ======================================================
// ROOM INFORMATION
// ======================================================

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
             << " | Name: " << rooms.at(id)->getName();

        auto doorIt = doors.find(id);

        if (doorIt != doors.end()) {
            cout << " | Door: "
                 << (doorIt->second.open() ? "Unlocked" : "Locked");
        }

        cout << '\n';
    }
}

// ======================================================
// SHORTEST PATH
// ======================================================

void GameManager::displayPathToRoom(int destinationRoomId) const {
    if (rooms.find(destinationRoomId) == rooms.end()) {
        cout << "Error: Destination room does not exist.\n";
        return;
    }

    vector<int> path =
        roomGraph.findPath(currentRoomId, destinationRoomId);

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

    cout << '\n';
}

// ======================================================
// MAIN GAME LOOP
// ======================================================

void GameManager::run() {
    if (rooms.find(currentRoomId) == rooms.end()) {
        throw runtime_error(
            "Starting room with ID 0 has not been registered."
        );
    }

    cout << "\nWelcome to Escape Room: Infinite Rooms!\n";

    displayCurrentRoom();

    bool running = true;

    while (running && !gameWon) {
        cout << "\n========== GAME MENU ==========\n";
        cout << "1. View current room\n";
        cout << "2. View connected rooms\n";
        cout << "3. Move to a room\n";
        cout << "4. Find shortest path\n";
        cout << "5. View puzzles\n";
        cout << "6. Solve a puzzle\n";
        cout << "7. View score\n";
        cout << "8. Exit game\n";
        cout << "Choose an option: ";

        int choice;

        if (!(cin >> choice)) {
            if (cin.eof()) {
                cout << "\nInput ended. Exiting game.\n";
                break;
            }

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Invalid input. Enter a number from 1 to 8.\n";
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
                        running = false;
                        break;
                    }

                    cin.clear();
                    cin.ignore(
                        numeric_limits<streamsize>::max(), '\n'
                    );

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
                        running = false;
                        break;
                    }

                    cin.clear();
                    cin.ignore(
                        numeric_limits<streamsize>::max(), '\n'
                    );

                    cout << "Invalid room ID.\n";
                    break;
                }

                displayPathToRoom(destination);
                break;
            }

            case 5:
                displayPuzzles();
                break;

            case 6: {
                int puzzleId;

                cout << "Enter puzzle ID: ";

                if (!(cin >> puzzleId)) {
                    if (cin.eof()) {
                        running = false;
                        break;
                    }

                    cin.clear();
                    cin.ignore(
                        numeric_limits<streamsize>::max(), '\n'
                    );

                    cout << "Invalid puzzle ID.\n";
                    break;
                }

                solvePuzzle(puzzleId);
                break;
            }

            case 7:
                cout << "Current score: " << score << '\n';
                break;

            case 8:
                running = false;
                cout << "Thanks for playing!\n";
                break;

            default:
                cout << "Invalid choice. Please choose 1 to 8.\n";
        }
    }
}