#include "GameManager.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

using namespace std;

GameManager::GameManager() : currentRoomId(0) {
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
if (rooms.find(roomA) == rooms.end() ||
rooms.find(roomB) == rooms.end()) {
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
    cout << "5. Exit game\n";
    cout << "Choose an option: ";

    int choice;

    if (!(cin >> choice)) {
        if (cin.eof()) {
            cout << "\nInput ended. Exiting game.\n";
            break;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Invalid input. Enter a number from 1 to 5.\n";
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

        case 5:
            running = false;
            cout << "Thanks for playing!\n";
            break;

        default:
            cout << "Invalid choice. Please choose 1 to 5.\n";
    }
}

}