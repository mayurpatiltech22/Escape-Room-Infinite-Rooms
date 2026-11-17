#ifndef ESCAPE_ROOM_PUZZLE_H
#define ESCAPE_ROOM_PUZZLE_H

#include <memory>
#include <string>
#include <vector>

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD,
    EXPERT
};

class Puzzle {
private:
    int id_;
    std::string title_;
    std::string description_;
    int points_;
    bool solved_;
    Difficulty difficulty_;
    int attempts_;
    int maxAttempts_;
    std::vector<std::string> hints_;
    int hintsRevealed_;

protected:
    virtual bool isCorrectAnswer(const std::string& answer) const = 0;

public:
    Puzzle(int id, std::string title, std::string description, int points,
           Difficulty difficulty = Difficulty::MEDIUM, int maxAttempts = 3);
    virtual ~Puzzle() = default;

    int getId() const;
    const std::string& getTitle() const;
    const std::string& getDescription() const;
    int getPoints() const;
    bool isSolved() const;
    void setSolved(bool solved);
    Difficulty getDifficulty() const;
    int getAttempts() const;
    int getMaxAttempts() const;
    int getHintsRevealed() const;
    int getTotalHints() const;

    void addHint(const std::string& hint);
    std::string getNextHint();
    bool hasMoreHints() const;

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
               const std::string& correctAnswer, int points = 10,
               Difficulty difficulty = Difficulty::EASY, int maxAttempts = 3);
};

class LogicPuzzle : public Puzzle {
private:
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    LogicPuzzle(int id, const std::string& title, const std::string& description,
                const std::string& correctAnswer, int points = 15,
                Difficulty difficulty = Difficulty::MEDIUM, int maxAttempts = 3);
};

class WordPuzzle : public Puzzle {
private:
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    WordPuzzle(int id, const std::string& title, const std::string& description,
               const std::string& correctAnswer, int points = 10,
               Difficulty difficulty = Difficulty::EASY, int maxAttempts = 3);
};

class SequencePuzzle : public Puzzle {
private:
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    SequencePuzzle(int id, const std::string& title, const std::string& description,
                   const std::string& correctAnswer, int points = 12,
                   Difficulty difficulty = Difficulty::MEDIUM, int maxAttempts = 3);
};

class CipherPuzzle : public Puzzle {
private:
    std::string cipherText_;
    int shift_;
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    CipherPuzzle(int id, const std::string& title, const std::string& description,
                 const std::string& cipherText, int shift, const std::string& correctAnswer,
                 int points = 20, Difficulty difficulty = Difficulty::HARD, int maxAttempts = 3);
};

class PatternPuzzle : public Puzzle {
private:
    std::string pattern_;
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    PatternPuzzle(int id, const std::string& title, const std::string& description,
                  const std::string& pattern, const std::string& correctAnswer,
                  int points = 15, Difficulty difficulty = Difficulty::MEDIUM, int maxAttempts = 3);
};

class RiddlePuzzle : public Puzzle {
private:
    std::string correctAnswer_;
protected:
    bool isCorrectAnswer(const std::string& answer) const override;
public:
    RiddlePuzzle(int id, const std::string& title, const std::string& description,
                 const std::string& correctAnswer,
                 int points = 18, Difficulty difficulty = Difficulty::HARD, int maxAttempts = 3);
};

class PuzzleGenerator {
public:
    static std::unique_ptr<MathPuzzle> generateDynamicMathPuzzle(int id, Difficulty difficulty);
    static std::unique_ptr<SequencePuzzle> generateDynamicSequencePuzzle(int id, Difficulty difficulty);
    static std::unique_ptr<CipherPuzzle> generateDynamicCipherPuzzle(int id, Difficulty difficulty);
    static std::unique_ptr<PatternPuzzle> generateDynamicPatternPuzzle(int id, Difficulty difficulty);

private:
    static int generateRandomNumber(int min, int max);
    static std::string generateCaesarCipher(const std::string& text, int shift);
};

#endif
