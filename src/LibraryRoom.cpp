#include "LibraryRoom.h"

LibraryRoom::LibraryRoom()
: Room(1, "Library",
"An old library filled with dusty books and hidden clues.") {
}

void LibraryRoom::enterRoom() {
cout << "\nYou entered the Library.\n";
cout << "Ancient books may contain clues to your escape.\n";
}