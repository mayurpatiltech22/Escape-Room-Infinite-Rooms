#ifndef ESCAPE_ROOM_PUZZLE_H
#define ESCAPE_ROOM_PUZZLE_H

#include <string>

class Puzzle {
private:
    int id_;
    std::string title_;
    std::string description_;
    int points_;
    bool solved_;

protected:
    virtual bool isCorrectAnswer(const std::string& answer) const = 0;

public:
    Puzzle(int id, std::string title, std::string description, int points);
    virtual ~Puzzle() = default;

    int getId() const;
    const std::string& getTitle() const;
    const std::string& getDescription() const;
    int getPoints() const;
    bool isSolved() const;
    void setSolved(bool solved);

    // Returns true for a correct answer. Solved puzzles cannot be unsolved.
    bool submitAnswer(const std::string& answer);
    virtual void display() const;
};

class MathPuzzle : public Puzzle {
private:
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    MathPuzzle(int id, const std::string& title, const std::string& description,
               const std::string& correctAnswer, int points = 10);
};

class LogicPuzzle : public Puzzle {
private:
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    LogicPuzzle(int id, const std::string& title, const std::string& description,
                const std::string& correctAnswer, int points = 15);
};

class WordPuzzle : public Puzzle {
private:
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    WordPuzzle(int id, const std::string& title, const std::string& description,
               const std::string& correctAnswer, int points = 10);
};

class SequencePuzzle : public Puzzle {
private:
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    SequencePuzzle(int id, const std::string& title, const std::string& description,
                   const std::string& correctAnswer, int points = 12);
};

#endif
