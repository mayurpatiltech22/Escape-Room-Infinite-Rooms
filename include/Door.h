#ifndef ESCAPE_ROOM_DOOR_H
#define ESCAPE_ROOM_DOOR_H

#include <string>

class Puzzle;

class Door {
private:
    int id_;
    std::string name_;
    int destinationRoomId_;
    int requiredPuzzleId_;
    bool unlocked_;

public:
    Door(int id, std::string name, int destinationRoomId, int requiredPuzzleId);

    int getId() const;
    const std::string& getName() const;
    int getDestinationRoomId() const;
    int getRequiredPuzzleId() const;
    bool isUnlocked() const;

    bool tryUnlock(const Puzzle& puzzle);
    bool open() const;
};

#endif
