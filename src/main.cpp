#include <iostream>
#include <memory>
#include <exception>

#include "GameManager.h"
#include "EntranceRoom.h"
#include "LibraryRoom.h"
#include "LabRoom.h"
#include "SecretRoom.h"
#include "EscapeRoom.h"

using namespace std;

int main() {
try {
GameManager game;

    // Register all rooms
    game.addRoom(make_unique<EntranceRoom>());
    game.addRoom(make_unique<LibraryRoom>());
    game.addRoom(make_unique<LabRoom>());
    game.addRoom(make_unique<SecretRoom>());
    game.addRoom(make_unique<EscapeRoom>());

    // Connect rooms
    game.connectRooms(0, 1);
    game.connectRooms(0, 2);
    game.connectRooms(1, 3);
    game.connectRooms(2, 3);
    game.connectRooms(3, 4);

    // Start the game
    game.run();
}
catch (const exception& error) {
    cerr << "Game error: " << error.what() << '\n';
    return 1;
}

return 0;

}