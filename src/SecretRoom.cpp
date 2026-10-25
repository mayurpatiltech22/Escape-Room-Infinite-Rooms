#include "SecretRoom.h"

SecretRoom::SecretRoom()
: Room(3, "Secret Archive",
"A hidden archive containing the building's secrets.") {
}

void SecretRoom::enterRoom() {
cout << "\nYou entered the Secret Archive.\n";
cout << "A locked passage may lead to the final chamber.\n";
}