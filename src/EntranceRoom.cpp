#include "EntranceRoom.h"

EntranceRoom::EntranceRoom()
: Room(0, "Entrance Hall",
"You have entered a mysterious building. Find a way to escape.") {
}

void EntranceRoom::enterRoom() {
cout << "\nYou are standing in the Entrance Hall.\n";
cout << "Two doors lead to different parts of the building.\n";
}