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

    // Test hint system
    MathPuzzle puzzleWithHints(3, "Hint Test", "Test puzzle with hints", "42", 15);
    puzzleWithHints.addHint("Think about the meaning of life");
    puzzleWithHints.addHint("It's a famous number from pop culture");
    assert(puzzleWithHints.hasMoreHints());
    std::string hint1 = puzzleWithHints.getNextHint();
    assert(!hint1.empty());
    assert(puzzleWithHints.getHintsRevealed() == 1);

    // Test attempts tracking
    assert(puzzleWithHints.getAttempts() == 0);
    puzzleWithHints.submitAnswer("wrong");
    assert(puzzleWithHints.getAttempts() == 1);
    assert(!puzzleWithHints.isSolved());

    // Test new puzzle types
    CipherPuzzle cipher(4, "Cipher", "Decrypt: KHOOR", "KHOOR", 3, "HELLO", 20);
    assert(cipher.submitAnswer("HELLO"));
    assert(cipher.isSolved());

    std::cout << "All puzzle and door tests passed.\n";
    return 0;
}
