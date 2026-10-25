#include "Door.h"
#include "Puzzle.h"

#include <utility>

Door::Door(int id, std::string name, int destinationRoomId, int requiredPuzzleId)
    : id_(id),
      name_(std::move(name)),
      destinationRoomId_(destinationRoomId),
      requiredPuzzleId_(requiredPuzzleId),
      unlocked_(false) {}

int Door::getId() const { return id_; }
const std::string& Door::getName() const { return name_; }
int Door::getDestinationRoomId() const { return destinationRoomId_; }
int Door::getRequiredPuzzleId() const { return requiredPuzzleId_; }
bool Door::isUnlocked() const { return unlocked_; }
void Door::setUnlocked(bool unlocked) { unlocked_ = unlocked; }

bool Door::tryUnlock(const Puzzle& puzzle) {
    if (puzzle.getId() != requiredPuzzleId_ || !puzzle.isSolved()) {
        return false;
    }
    unlocked_ = true;
    return true;
}

bool Door::open() const {
    return unlocked_;
}
