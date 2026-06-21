#ifndef FLASHCARD_H
#define FLASHCARD_H

#include "Word.h"
#include <string>

class Flashcard {
private:
    Word word;
    std::string addedAt;

public:
    Flashcard();
    Flashcard(const Word& word, const std::string& addedAt);

    // Getters & Setters
    Word getWord() const;
    void setWord(const Word& word);

    std::string getAddedAt() const;
    void setAddedAt(const std::string& addedAt);

    // Display
    void displayCard() const;
};

#endif // FLASHCARD_H
