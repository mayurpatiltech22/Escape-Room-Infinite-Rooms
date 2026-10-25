#include <iostream>
#include <memory>
#include <exception>
#include "Puzzle.h"
#include "Door.h"
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

    // Register puzzles
game.addPuzzle(
    make_unique<MathPuzzle>(
        101,
        "Library Puzzle",
        "What is 12 + 8?",
        "20",
        10
    )
);

game.addPuzzle(
    make_unique<LogicPuzzle>(
        102,
        "Laboratory Puzzle",
        "What comes next: 2, 4, 8, 16, ?",
        "32",
        15
    )
);

game.addPuzzle(
    make_unique<WordPuzzle>(
        103,
        "Secret Archive Puzzle",
        "What is the opposite of the word 'open'?",
        "closed",
        10
    )
);

game.addPuzzle(
    make_unique<SequencePuzzle>(
        104,
        "Final Escape Puzzle",
        "What comes next: 1, 1, 2, 3, 5, ?",
        "8",
        20
    )
);

// Register doors
game.addDoor(Door(1, "Library Door", 1, 101));
game.addDoor(Door(2, "Laboratory Door", 2, 102));
game.addDoor(Door(3, "Secret Archive Door", 3, 103));
game.addDoor(Door(4, "Escape Chamber Door", 4, 104));

game.run();
    
}
catch (const exception& error) {
    cerr << "Game error: " << error.what() << '\n';
    return 1;
}

return 0;

}