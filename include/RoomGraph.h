#ifndef ROOM_GRAPH_H
#define ROOM_GRAPH_H

#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class RoomGraph {
private:
unordered_map<int, vector<int>> connections;

public:
// Connect two rooms
void addConnection(int roomA, int roomB);

// Get directly connected rooms
vector<int> getNeighbors(int roomId) const;

// Check whether two rooms are directly connected
bool isConnected(int roomA, int roomB) const;

// Find the shortest path using BFS
vector<int> findPath(int start, int destination) const;

};

#endif