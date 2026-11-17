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
#include "GameRenderer.h"

using namespace std;

int main(int argc, char* argv[]) {
try {
    bool cliMode = false;
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "--cli") {
            cliMode = true;
            break;
        }
    }

    GameManager game;

    // Register all rooms
    game.addRoom(make_unique<EntranceRoom>());
    game.addRoom(make_unique<LibraryRoom>());
    game.addRoom(make_unique<LabRoom>());
    game.addRoom(make_unique<SecretRoom>());
    game.addRoom(make_unique<EscapeRoom>());
    // Bonus rooms for extra puzzles (commented out due to ID conflict)
    // game.addRoom(make_unique<LibraryRoom>(5));  // Hidden Chamber (reusing LibraryRoom for simplicity)
    // game.addRoom(make_unique<LabRoom>(6));      // Ancient Passage (reusing LabRoom for simplicity)

    // Connect rooms
    game.connectRooms(0, 1);
    game.connectRooms(0, 2);
    game.connectRooms(1, 3);
    game.connectRooms(2, 3);
    game.connectRooms(3, 4);
    // Bonus connections for extra puzzles (commented out with bonus rooms)
    // game.connectRooms(1, 5);  // Library to Hidden Chamber
    // game.connectRooms(2, 6);  // Lab to Ancient Passage

    // Register puzzles
// Library Puzzle
auto libraryPuzzle = make_unique<MathPuzzle>(
    101,
    "Library Calculation",
    "Solve: 5 + 3 = ?",
    "8",
    10,
    Difficulty::EASY,
    0
);
game.addPuzzle(std::move(libraryPuzzle));

// Laboratory Puzzle
auto labPuzzle = make_unique<LogicPuzzle>(
    102,
    "Laboratory Code",
    "What comes next: 2, 4, 6, ?",
    "8",
    10,
    Difficulty::EASY,
    0
);
game.addPuzzle(std::move(labPuzzle));

// Secret Archive Puzzle
auto archivePuzzle = make_unique<CipherPuzzle>(
    103,
    "Archive Entry",
    "Decrypt: HELLO (shift 0)",
    "HELLO",
    0,
    "HELLO",
    10,
    Difficulty::EASY,
    0
);
game.addPuzzle(std::move(archivePuzzle));

// Final Escape Puzzle
auto escapePuzzle = make_unique<SequencePuzzle>(
    104,
    "Escape Sequence",
    "Complete: 1, 2, 3, ?",
    "4",
    10,
    Difficulty::EASY,
    0
);
game.addPuzzle(std::move(escapePuzzle));

// Bonus Puzzle - Riddle with hints (commented out - requires bonus rooms)
// auto riddlePuzzle = make_unique<RiddlePuzzle>(
//     105,
//     "The Guardian's Riddle",
//     "I have cities, but no houses. I have mountains, but no trees. I have water, but no fish. What am I?",
//     "map",
//     25,
//     Difficulty::HARD,
//     3
// );
// riddlePuzzle->addHint("Think about something you use for navigation");
// riddlePuzzle->addHint("It shows locations but not the actual buildings");
// riddlePuzzle->addHint("Check your phone or car dashboard");
// game.addPuzzle(std::move(riddlePuzzle));

// Bonus Puzzle - Pattern with hints (commented out - requires bonus rooms)
// auto patternPuzzle = make_unique<PatternPuzzle>(
//     106,
//     "Wall Pattern Analysis",
//     "Observe the pattern on the wall: A, C, E, G, ? What letter completes it?",
//     "A, C, E, G, ?",
//     "I",
//     18,
//     Difficulty::MEDIUM,
//     3
// );
// patternPuzzle->addHint("Look at the alphabet");
// patternPuzzle->addHint("Skip one letter each time");
// game.addPuzzle(std::move(patternPuzzle));

// Register doors
game.addDoor(Door(1, "Library Door", 1, 101));
game.addDoor(Door(2, "Laboratory Door", 2, 102));
game.addDoor(Door(3, "Secret Archive Door", 3, 103));
game.addDoor(Door(4, "Escape Chamber Door", 4, 104));
// Bonus doors for extra puzzles (commented out with bonus rooms)
// game.addDoor(Door(5, "Hidden Chamber Door", 1, 105));
// game.addDoor(Door(6, "Ancient Passage Door", 2, 106));
    if (cliMode) {
        game.run();
    } else {
        GameRenderer renderer;
        renderer.runGraphicalGame(game);
    }
}
catch (const exception& error) {
    cerr << "Game error: " << error.what() << '\n';
    return 1;
}

return 0;

}