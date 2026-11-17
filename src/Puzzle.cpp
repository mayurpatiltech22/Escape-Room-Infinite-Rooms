#include "Puzzle.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace {
std::string normalizeAnswer(std::string answer) {
    auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };

    answer.erase(answer.begin(),
                 std::find_if(answer.begin(), answer.end(), notSpace));
    answer.erase(std::find_if(answer.rbegin(), answer.rend(), notSpace).base(),
                 answer.end());

    std::transform(answer.begin(), answer.end(), answer.begin(),
                   [](unsigned char ch) {
                       return static_cast<char>(std::tolower(ch));
                   });
    return answer;
}
}

Puzzle::Puzzle(int id, std::string title, std::string description, int points,
               Difficulty difficulty, int maxAttempts)
    : id_(id),
      title_(std::move(title)),
      description_(std::move(description)),
      points_(points < 0 ? 0 : points),
      solved_(false),
      difficulty_(difficulty),
      attempts_(0),
      maxAttempts_(maxAttempts > 0 ? maxAttempts : 3),
      hintsRevealed_(0) {}

int Puzzle::getId() const { return id_; }
const std::string& Puzzle::getTitle() const { return title_; }
const std::string& Puzzle::getDescription() const { return description_; }
int Puzzle::getPoints() const { return points_; }
bool Puzzle::isSolved() const { return solved_; }
void Puzzle::setSolved(bool solved) { solved_ = solved; }
Difficulty Puzzle::getDifficulty() const { return difficulty_; }
int Puzzle::getAttempts() const { return attempts_; }
int Puzzle::getMaxAttempts() const { return maxAttempts_; }
int Puzzle::getHintsRevealed() const { return hintsRevealed_; }
int Puzzle::getTotalHints() const { return static_cast<int>(hints_.size()); }

void Puzzle::addHint(const std::string& hint) {
    hints_.push_back(hint);
}

std::string Puzzle::getNextHint() {
    if (hintsRevealed_ < static_cast<int>(hints_.size())) {
        return hints_[hintsRevealed_++];
    }
    return "";
}

bool Puzzle::hasMoreHints() const {
    return hintsRevealed_ < static_cast<int>(hints_.size());
}

bool Puzzle::submitAnswer(const std::string& answer) {
    if (solved_) {
        return true;
    }

    attempts_++;

    if (isCorrectAnswer(answer)) {
        solved_ = true;
        // Reduce points based on attempts (more attempts = fewer points)
        if (attempts_ > 1) {
            int penalty = (attempts_ - 1) * (points_ / maxAttempts_);
            points_ = std::max(points_ - penalty, points_ / 2);
        }
        return true;
    }
    return false;
}

void Puzzle::display() const {
    std::cout << "\n[" << id_ << "] " << title_ << '\n'
              << description_ << '\n'
              << "Points: " << points_ << '\n'
              << "Difficulty: ";

    switch (difficulty_) {
        case Difficulty::EASY: std::cout << "Easy"; break;
        case Difficulty::MEDIUM: std::cout << "Medium"; break;
        case Difficulty::HARD: std::cout << "Hard"; break;
        case Difficulty::EXPERT: std::cout << "Expert"; break;
    }

    std::cout << '\n'
              << "Status: " << (solved_ ? "Solved" : "Unsolved") << '\n';

    if (!solved_ && maxAttempts_ > 0) {
        std::cout << "Attempts: " << attempts_ << " / " << maxAttempts_ << '\n';
    }

    if (hintsRevealed_ > 0) {
        std::cout << "Hints used: " << hintsRevealed_ << " / " << hints_.size() << '\n';
    }
}

MathPuzzle::MathPuzzle(int id, const std::string& title,
                       const std::string& description,
                       const std::string& correctAnswer, int points,
                       Difficulty difficulty, int maxAttempts)
    : Puzzle(id, title, description, points, difficulty, maxAttempts),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool MathPuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

LogicPuzzle::LogicPuzzle(int id, const std::string& title,
                         const std::string& description,
                         const std::string& correctAnswer, int points,
                         Difficulty difficulty, int maxAttempts)
    : Puzzle(id, title, description, points, difficulty, maxAttempts),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool LogicPuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

WordPuzzle::WordPuzzle(int id, const std::string& title,
                       const std::string& description,
                       const std::string& correctAnswer, int points,
                       Difficulty difficulty, int maxAttempts)
    : Puzzle(id, title, description, points, difficulty, maxAttempts),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool WordPuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

SequencePuzzle::SequencePuzzle(int id, const std::string& title,
                               const std::string& description,
                               const std::string& correctAnswer, int points,
                               Difficulty difficulty, int maxAttempts)
    : Puzzle(id, title, description, points, difficulty, maxAttempts),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool SequencePuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

CipherPuzzle::CipherPuzzle(int id, const std::string& title,
                           const std::string& description,
                           const std::string& cipherText, int shift,
                           const std::string& correctAnswer, int points,
                           Difficulty difficulty, int maxAttempts)
    : Puzzle(id, title, description, points, difficulty, maxAttempts),
      cipherText_(cipherText),
      shift_(shift),
      correctAnswer_(normalizeAnswer(correctAnswer)) {
}

bool CipherPuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == normalizeAnswer(correctAnswer_);
}

PatternPuzzle::PatternPuzzle(int id, const std::string& title,
                             const std::string& description,
                             const std::string& pattern, const std::string& correctAnswer,
                             int points, Difficulty difficulty, int maxAttempts)
    : Puzzle(id, title, description, points, difficulty, maxAttempts),
      pattern_(pattern),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool PatternPuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

RiddlePuzzle::RiddlePuzzle(int id, const std::string& title,
                           const std::string& description,
                           const std::string& correctAnswer,
                           int points, Difficulty difficulty, int maxAttempts)
    : Puzzle(id, title, description, points, difficulty, maxAttempts),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool RiddlePuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

// ======================================================
// PUZZLE GENERATOR IMPLEMENTATION
// ======================================================

int PuzzleGenerator::generateRandomNumber(int min, int max) {
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
    return min + (std::rand() % (max - min + 1));
}

std::string PuzzleGenerator::generateCaesarCipher(const std::string& text, int shift) {
    std::string result;
    for (char ch : text) {
        if (std::isalpha(ch)) {
            char base = std::isupper(ch) ? 'A' : 'a';
            ch = base + ((ch - base + shift) % 26 + 26) % 26;
        }
        result += ch;
    }
    return result;
}

std::unique_ptr<MathPuzzle> PuzzleGenerator::generateDynamicMathPuzzle(int id, Difficulty difficulty) {
    int a, b, operation, answer;
    std::string title = "Dynamic Math Challenge";
    std::string description;
    int points = 10;
    int maxAttempts = 3;

    switch (difficulty) {
        case Difficulty::EASY:
            a = generateRandomNumber(1, 20);
            b = generateRandomNumber(1, 20);
            operation = generateRandomNumber(0, 1);
            if (operation == 0) {
                answer = a + b;
                description = "Solve: " + std::to_string(a) + " + " + std::to_string(b) + " = ?";
            } else {
                a = generateRandomNumber(10, 50);
                b = generateRandomNumber(1, a - 1);
                answer = a - b;
                description = "Solve: " + std::to_string(a) + " - " + std::to_string(b) + " = ?";
            }
            points = 10;
            maxAttempts = 5;
            break;

        case Difficulty::MEDIUM:
            a = generateRandomNumber(2, 12);
            b = generateRandomNumber(2, 12);
            answer = a * b;
            description = "Solve: " + std::to_string(a) + " × " + std::to_string(b) + " = ?";
            points = 15;
            maxAttempts = 4;
            break;

        case Difficulty::HARD: {
            a = generateRandomNumber(10, 100);
            b = generateRandomNumber(2, 10);
            int multiplier = b;
            while (a % multiplier != 0) {
                a = generateRandomNumber(10, 100);
            }
            answer = a / multiplier;
            description = "Solve: " + std::to_string(a) + " ÷ " + std::to_string(multiplier) + " = ?";
            points = 20;
            maxAttempts = 3;
            break;
        }

        case Difficulty::EXPERT: {
            a = generateRandomNumber(1, 50);
            b = generateRandomNumber(1, 50);
            int c = generateRandomNumber(1, 20);
            int op1 = generateRandomNumber(0, 1);
            if (op1 == 0) {
                answer = a + b - c;
                description = "Solve: " + std::to_string(a) + " + " + std::to_string(b) + " - " + std::to_string(c) + " = ?";
            } else {
                answer = a * b + c;
                description = "Solve: " + std::to_string(a) + " × " + std::to_string(b) + " + " + std::to_string(c) + " = ?";
            }
            points = 25;
            maxAttempts = 3;
            break;
        }
    }

    return std::make_unique<MathPuzzle>(id, title, description, std::to_string(answer),
                                         points, difficulty, maxAttempts);
}

std::unique_ptr<SequencePuzzle> PuzzleGenerator::generateDynamicSequencePuzzle(int id, Difficulty difficulty) {
    std::string title = "Dynamic Sequence Challenge";
    std::string description;
    std::string answer;
    int points = 12;
    int maxAttempts = 3;

    std::vector<int> sequence;
    int nextNum;

    switch (difficulty) {
        case Difficulty::EASY:
            {
                int start = generateRandomNumber(1, 10);
                int step = generateRandomNumber(1, 5);
                for (int i = 0; i < 4; i++) {
                    sequence.push_back(start + i * step);
                }
                nextNum = start + 4 * step;
                description = "What comes next in this arithmetic sequence: ";
                for (size_t i = 0; i < sequence.size(); i++) {
                    description += std::to_string(sequence[i]);
                    if (i < sequence.size() - 1) description += ", ";
                }
                description += ", ?";
                points = 12;
                maxAttempts = 4;
            }
            break;

        case Difficulty::MEDIUM:
            {
                int start = generateRandomNumber(1, 5);
                sequence.push_back(start);
                for (int i = 1; i < 4; i++) {
                    sequence.push_back(sequence.back() * 2);
                }
                nextNum = sequence.back() * 2;
                description = "What comes next in this geometric sequence: ";
                for (size_t i = 0; i < sequence.size(); i++) {
                    description += std::to_string(sequence[i]);
                    if (i < sequence.size() - 1) description += ", ";
                }
                description += ", ?";
                points = 15;
                maxAttempts = 3;
            }
            break;

        case Difficulty::HARD:
            {
                sequence = {1, 1};
                for (int i = 2; i < 5; i++) {
                    sequence.push_back(sequence[i-1] + sequence[i-2]);
                }
                nextNum = sequence.back() + sequence[sequence.size() - 2];
                description = "What comes next in this Fibonacci sequence: ";
                for (size_t i = 0; i < sequence.size(); i++) {
                    description += std::to_string(sequence[i]);
                    if (i < sequence.size() - 1) description += ", ";
                }
                description += ", ?";
                points = 20;
                maxAttempts = 3;
            }
            break;

        case Difficulty::EXPERT:
            {
                int start = generateRandomNumber(1, 3);
                sequence.push_back(start);
                for (int i = 1; i < 4; i++) {
                    sequence.push_back(sequence.back() + i);
                }
                nextNum = sequence.back() + 4;
                description = "What comes next: ";
                for (size_t i = 0; i < sequence.size(); i++) {
                    description += std::to_string(sequence[i]);
                    if (i < sequence.size() - 1) description += ", ";
                }
                description += ", ?";
                points = 25;
                maxAttempts = 3;
            }
            break;
    }

    return std::make_unique<SequencePuzzle>(id, title, description, std::to_string(nextNum),
                                             points, difficulty, maxAttempts);
}

std::unique_ptr<CipherPuzzle> PuzzleGenerator::generateDynamicCipherPuzzle(int id, Difficulty difficulty) {
    std::vector<std::string> words = {"escape", "freedom", "unlock", "secret", "cipher",
                                      "decode", "puzzle", "mystery", "hidden", "key"};
    std::string plaintext = words[generateRandomNumber(0, words.size() - 1)];

    int shift;
    switch (difficulty) {
        case Difficulty::EASY:
            shift = generateRandomNumber(1, 3);
            break;
        case Difficulty::MEDIUM:
            shift = generateRandomNumber(4, 7);
            break;
        case Difficulty::HARD:
            shift = generateRandomNumber(8, 12);
            break;
        case Difficulty::EXPERT:
            shift = generateRandomNumber(13, 25);
            break;
    }

    std::string cipherText = generateCaesarCipher(plaintext, shift);
    std::string title = "Caesar Cipher Challenge";
    std::string description = "Decrypt this Caesar cipher (shift of " + std::to_string(shift) + "): " + cipherText;
    int points = 20 + static_cast<int>(difficulty) * 5;
    int maxAttempts = 3;

    return std::make_unique<CipherPuzzle>(id, title, description, cipherText, shift, plaintext,
                                           points, difficulty, maxAttempts);
}

std::unique_ptr<PatternPuzzle> PuzzleGenerator::generateDynamicPatternPuzzle(int id, Difficulty difficulty) {
    std::string title = "Pattern Recognition Challenge";
    std::string description;
    std::string answer;
    int points = 15;
    int maxAttempts = 3;

    switch (difficulty) {
        case Difficulty::EASY:
            {
                char letter = 'A' + generateRandomNumber(0, 5);
                description = "What comes next: " + std::string(1, letter) + ", " +
                              std::string(1, letter + 1) + ", " +
                              std::string(1, letter + 2) + ", ?";
                answer = std::string(1, letter + 3);
                points = 12;
                maxAttempts = 4;
            }
            break;

        case Difficulty::MEDIUM: {
            std::string pattern = "○ ● ○ ● ○";
            description = "What symbol comes next: " + pattern + " ?";
            answer = "●";
            points = 15;
            maxAttempts = 3;
            break;
        }

        case Difficulty::HARD: {
            int num = generateRandomNumber(1, 3);
            description = "What comes next: " + std::to_string(num) + ", " +
                          std::to_string(num * 2) + ", " +
                          std::to_string(num * 4) + ", " +
                          std::to_string(num * 8) + ", ?";
            answer = std::to_string(num * 16);
            points = 20;
            maxAttempts = 3;
            break;
        }

        case Difficulty::EXPERT: {
            description = "What comes next: 2, 6, 12, 20, 30, ?";
            answer = "42";
            points = 25;
            maxAttempts = 3;
            break;
        }
    }

    return std::make_unique<PatternPuzzle>(id, title, description, description, answer,
                                            points, difficulty, maxAttempts);
}
