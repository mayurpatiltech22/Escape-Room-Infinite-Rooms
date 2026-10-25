#include "Puzzle.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>

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

Puzzle::Puzzle(int id, std::string title, std::string description, int points)
    : id_(id),
      title_(std::move(title)),
      description_(std::move(description)),
      points_(points < 0 ? 0 : points),
      solved_(false) {}

int Puzzle::getId() const { return id_; }
const std::string& Puzzle::getTitle() const { return title_; }
const std::string& Puzzle::getDescription() const { return description_; }
int Puzzle::getPoints() const { return points_; }
bool Puzzle::isSolved() const { return solved_; }
void Puzzle::setSolved(bool solved) { solved_ = solved; }

bool Puzzle::submitAnswer(const std::string& answer) {
    if (solved_) {
        return true;
    }

    if (isCorrectAnswer(answer)) {
        solved_ = true;
        return true;
    }
    return false;
}

void Puzzle::display() const {
    std::cout << "\n[" << id_ << "] " << title_ << '\n'
              << description_ << '\n'
              << "Points: " << points_ << '\n'
              << "Status: " << (solved_ ? "Solved" : "Unsolved") << '\n';
}

MathPuzzle::MathPuzzle(int id, const std::string& title,
                       const std::string& description,
                       const std::string& correctAnswer, int points)
    : Puzzle(id, title, description, points),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool MathPuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

LogicPuzzle::LogicPuzzle(int id, const std::string& title,
                         const std::string& description,
                         const std::string& correctAnswer, int points)
    : Puzzle(id, title, description, points),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool LogicPuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

WordPuzzle::WordPuzzle(int id, const std::string& title,
                       const std::string& description,
                       const std::string& correctAnswer, int points)
    : Puzzle(id, title, description, points),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool WordPuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}

SequencePuzzle::SequencePuzzle(int id, const std::string& title,
                               const std::string& description,
                               const std::string& correctAnswer, int points)
    : Puzzle(id, title, description, points),
      correctAnswer_(normalizeAnswer(correctAnswer)) {}

bool SequencePuzzle::isCorrectAnswer(const std::string& answer) const {
    return normalizeAnswer(answer) == correctAnswer_;
}
