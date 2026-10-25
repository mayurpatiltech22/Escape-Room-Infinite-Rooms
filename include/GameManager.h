#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Room.h"
#include "RoomGraph.h"
#include "Player.h"
#include "Trap.h"
#include "SaveLoadManager.h"
#include "Puzzle.h"
#include "Door.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class GameManager {
private:
// Room management
unordered_map<int, unique_ptr<Room>> rooms;
RoomGraph roomGraph;

```
// Game state
int currentRoomId;
int score;
int movesCount;
bool gameWon;

// Player and traps (Member 3)
Player player;
unordered_map<int, vector<Trap>> roomTraps;

// Visited rooms
vector<int> visitedRooms;

// Room completion tracking
unordered_map<int, bool> completedRooms;

// Puzzle system
unordered_map<int, unique_ptr<Puzzle>> puzzles;

// Doors indexed by destination room ID
unordered_map<int, Door> doors;
```

public:
GameManager();

```
// Room management
void addRoom(unique_ptr<Room> room);
void connectRooms(int roomA, int roomB);
bool moveToRoom(int destinationRoomId);

void displayCurrentRoom() const;
void displayAvailableRooms() const;
void displayPathToRoom(int destinationRoomId) const;

// State query methods
int getCurrentRoomId() const;
int getScore() const;
int getMovesCount() const;
const vector<int>& getVisitedRooms() const;
const RoomGraph& getRoomGraph() const;
Room* getRoom(int id) const;

bool isRoomCompleted(int id) const;
void setRoomCompleted(int id, bool completed);
void addScore(int points);

// Save and load management
GameState captureState() const;
void restoreState(const GameState& state);
void saveGame(const string& filename) const;
void loadGame(const string& filename);

// Puzzle and door functions (Member 2)
void addPuzzle(unique_ptr<Puzzle> puzzle);
void addDoor(const Door& door);

void displayPuzzles() const;
void solvePuzzle(int puzzleId);

// Player and trap functions (Member 3)
// Add declarations here if Member 3's implementation defines
// additional public Player or Trap management methods.

// Main game loop
void run();
```

};

#endif
