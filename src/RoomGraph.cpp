#include "RoomGraph.h"

#include <algorithm>
#include <queue>
#include <unordered_set>

void RoomGraph::addConnection(int roomA, int roomB) {
if (roomA == roomB) {
return;
}

vector<int>& neighborsA = connections[roomA];
vector<int>& neighborsB = connections[roomB];

if (find(neighborsA.begin(), neighborsA.end(), roomB) == neighborsA.end()) {
    neighborsA.push_back(roomB);
}

if (find(neighborsB.begin(), neighborsB.end(), roomA) == neighborsB.end()) {
    neighborsB.push_back(roomA);
}

}

vector<int> RoomGraph::getNeighbors(int roomId) const {
auto it = connections.find(roomId);

if (it == connections.end()) {
    return {};
}

return it->second;

}

bool RoomGraph::isConnected(int roomA, int roomB) const {
vector<int> neighbors = getNeighbors(roomA);

return find(neighbors.begin(), neighbors.end(), roomB) != neighbors.end();

}

vector<int> RoomGraph::findPath(int start, int destination) const {
if (start == destination) {
return {start};
}

queue<int> pending;
unordered_set<int> visited;
unordered_map<int, int> parent;

pending.push(start);
visited.insert(start);

while (!pending.empty()) {
    int current = pending.front();
    pending.pop();

    if (current == destination) {
        break;
    }

    for (int neighbor : getNeighbors(current)) {
        if (visited.insert(neighbor).second) {
            parent[neighbor] = current;
            pending.push(neighbor);
        }
    }
}

if (visited.find(destination) == visited.end()) {
    return {};
}

vector<int> path;

for (int current = destination; current != start; current = parent[current]) {
    path.push_back(current);
}

path.push_back(start);
reverse(path.begin(), path.end());

return path;

}