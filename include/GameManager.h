#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Room.h"
#include "RoomGraph.h"
#include "Player.h"
#include "Trap.h"
#include <vector>



#include <memory>
#include <unordered_map>

using namespace std;

class GameManager {
private:
unordered_map<int, unique_ptr<Room>> rooms;
RoomGraph roomGraph;
int currentRoomId;
Player player;
unordered_map<int, vector<Trap>> roomTraps;

public:
GameManager();

void addRoom(unique_ptr<Room> room);
void connectRooms(int roomA, int roomB);

bool moveToRoom(int destinationRoomId);

void displayCurrentRoom() const;
void displayAvailableRooms() const;
void displayPathToRoom(int destinationRoomId) const;

void run();

};

#endif