#include "EscapeRoom.h"

EscapeRoom::EscapeRoom()
: Room(4, "Escape Chamber",
"The final chamber. Find a way to unlock the exit.") {
}

void EscapeRoom::enterRoom() {
cout << "\nYou entered the Escape Chamber!\n";
cout << "You have reached the final room. Can you escape?\n";
}