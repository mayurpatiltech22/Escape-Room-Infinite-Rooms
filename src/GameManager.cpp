#include "GameManager.h"
#include "Player.h"
#include "Trap.h"
#include "Item.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <cstdlib>
#include <ctime>

using namespace std;

GameManager::GameManager()
    : currentRoomId(0),
      player("Explorer") {

        std::srand(static_cast<unsigned int>(std::time(nullptr)));


    roomTraps[0].emplace_back(
        "Spike Trap",
        "You stepped on a hidden spike trap!",
        20
    );

    roomTraps[2].emplace_back(
        "Poison Trap",
        "You stepped into poisonous gas!",
        15
    );

    roomTraps[3].emplace_back(
        "Electric Trap",
        "You touched a live electric wire!",
        25
    );
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

    cout << "\nMoving to "
         << rooms.at(currentRoomId)->getName() << "...\n";

    // Automatically trigger traps in the entered room
    auto trapIt = roomTraps.find(currentRoomId);

    if (trapIt != roomTraps.end()) {
        for (auto& trap : trapIt->second) {
            trap.trigger(player);
        }
    }
if (!player.isAlive() || player.getScore() <= 0) {
    return true;
}
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
    if (!player.isAlive() || player.getScore() <= 0) {
    cout << "\n========== GAME OVER ==========\n";

    if (!player.isAlive()) {
        cout << "Your health reached zero!\n";
    }

    if (player.getScore() <= 0) {
        cout << "Your score reached zero!\n";
    }

    player.displayStatus();
    break;
}
    
cout << "\n========== ESCAPE ROOM MENU ==========\n";

cout << "\n--- ROOM NAVIGATION ---\n";
cout << "1. View Current Room\n";
cout << "2. View Connected Rooms\n";
cout << "3. Move to a Room\n";
cout << "4. Find Shortest Path\n";

cout << "\n--- PLAYER INFORMATION ---\n";
cout << "5. View Player Status\n";
cout << "6. View Inventory\n";

cout << "\n--- ITEMS & HEALTH ---\n";
cout << "7. Collect Item\n";
cout << "8. Collect Health Potion\n";
cout << "9. Use Health Potion\n";

cout << "\n--- TRAPS ---\n";
cout << "10. View Trap Information\n";

cout << "\n11. Exit Game\n";
cout << "======================================\n";
cout << "\nEnter your choice: ";

    int choice;

    if (!(cin >> choice)) {
        if (cin.eof()) {
            cout << "\nInput ended. Exiting game.\n";
            break;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Invalid input. Enter a number from 1 to 11.\n";
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
            player.displayStatus();
            break;

        case 6:
            player.getInventory().displayInventory();
            break;

             
            
        case 7: {
    if (player.getInventory().hasItem("Golden Key")) {
        cout << "You already have a Golden Key!\n";
    } else {
        auto item = make_shared<Item>(
            "Golden Key",
            "A golden key that may unlock a secret door.",
            true
        );

        player.getInventory().addItem(item);
        player.addScore(10);

        cout << "Golden Key collected! +10 points.\n";
    }
    break;
}


            
        case 8: {
            if (player.getInventory().hasItem("Health Potion")) {
                cout << "You already have a Health Potion!\n";
            } else {
                auto potion = make_shared<Item>(
                    "Health Potion",
                    "Restores 30 health points.",
                    true
                );

                player.getInventory().addItem(potion);
player.addScore(5);
cout << "Health Potion collected! +5 points.\n";
            }
            break;
        }

        case 9: {
            if (player.getInventory().hasItem("Health Potion")) {
    if (player.getHealth() >= 100) {
        cout << "Your health is already full!\n";
    } else {
        int oldHealth = player.getHealth();

        player.heal(30);

        int healthRestored = player.getHealth() - oldHealth;

        player.getInventory().removeItem("Health Potion");

        cout << "\nHealth Potion used successfully!\n";
        cout << "Health Restored: +" << healthRestored << " HP\n";
        cout << "Current Health: " << player.getHealth()
             << "/100 HP\n";
    }
} else {
    cout << "You don't have a Health Potion!\n";
}
break;
        }

        
case 10: {
    cout << "\n========================================\n";
    cout << "       TRAP INFORMATION CENTER         \n";
    cout << "========================================\n";

    if (roomTraps.empty()) {
        cout << "No traps are configured in the game.\n";
        break;
    }

    for (const auto& roomEntry : roomTraps) {
        int roomId = roomEntry.first;

        auto roomIt = rooms.find(roomId);

        if (roomIt == rooms.end()) {
            continue;
        }

        cout << "\nRoom: "
             << roomIt->second->getName()
             << " (ID: " << roomId << ")\n";

        for (const auto& trap : roomEntry.second) {
            cout << "\nTrap Name: " << trap.getName() << '\n';
            cout << "Description: "
                 << trap.getDescription() << '\n';
            cout << "Potential Damage: "
                 << trap.getDamage() << " HP\n";

            cout << "Safety Tip: Keep your health high "
                 << "and carry a Health Potion.\n";
        }

        cout << "----------------------------------------\n";
    }

    cout << "\nRemember: A trap may cause damage "
         << "or you may escape unharmed.\n";
    cout << "========================================\n";
    break;
}



         case 11:
    running = false;
    cout << "Thanks for playing!\n";
    break;


        default:
            cout << "Invalid choice. Please choose 1 to 11.\n";
    }

    // Check if the game should end
    if (!player.isAlive() || player.getScore() <= 0) {
        cout << "\n========== GAME OVER ==========\n";

        if (!player.isAlive()) {
            cout << "Your health reached zero!\n";
        }

        if (player.getScore() <= 0) {
            cout << "Your score reached zero!\n";
        }

        player.displayStatus();
        running = false;
    }
}


}