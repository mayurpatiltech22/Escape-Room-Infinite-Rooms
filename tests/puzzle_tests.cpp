#include "Door.h"
#include "Puzzle.h"

#include <cassert>
#include <iostream>

int main() {
    MathPuzzle math(1, "Math", "What is 2 + 3?", "5", 10);
    assert(!math.isSolved());
    assert(!math.submitAnswer("4"));
    assert(!math.isSolved());
    assert(math.submitAnswer(" 5 "));
    assert(math.isSolved());
    assert(math.submitAnswer("wrong")); // solved puzzle remains solved

    WordPuzzle word(2, "Word", "Unscramble: R O O D", "door", 10);
    assert(word.submitAnswer(" DOOR "));

    Door door(7, "Library Door", 2, 1);
    MathPuzzle wrongPuzzle(99, "Other", "Not the required puzzle", "yes", 1);
    assert(!door.tryUnlock(wrongPuzzle));
    assert(!door.open());
    assert(door.tryUnlock(math));
    assert(door.open());

    std::cout << "All puzzle and door tests passed.\n";
    return 0;
}
