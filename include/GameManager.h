#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Room.h"
#include "RoomGraph.h"
#include "Puzzle.h"
#include "Door.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

class GameManager {
private:
    unordered_map<int, unique_ptr<Room>> rooms;
    RoomGraph roomGraph;

    // Member 2: Puzzle system
    unordered_map<int, unique_ptr<Puzzle>> puzzles;

    // Doors indexed by their destination room ID.
    // This assumes one controlled entrance per destination room.
    unordered_map<int, Door> doors;
    int currentRoomId;
    int score;
    bool gameWon;
public:
    GameManager();

    // Existing room-management functions
    void addRoom(unique_ptr<Room> room);
    void connectRooms(int roomA, int roomB);

    bool moveToRoom(int destinationRoomId);

    void displayCurrentRoom() const;
    void displayAvailableRooms() const;
    void displayPathToRoom(int destinationRoomId) const;

    // Member 2: Puzzle and door functions
    void addPuzzle(unique_ptr<Puzzle> puzzle);
    void addDoor(const Door& door);

    void displayPuzzles() const;
    void solvePuzzle(int puzzleId);

    // Main game loop
    void run();
};

#endif