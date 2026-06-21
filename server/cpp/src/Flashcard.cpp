#include "Flashcard.h"
#include <iostream>

Flashcard::Flashcard() : word(Word()), addedAt("") {}

Flashcard::Flashcard(const Word& word, const std::string& addedAt) : word(word), addedAt(addedAt) {}

Word Flashcard::getWord() const { return word; }
void Flashcard::setWord(const Word& word) { this->word = word; }

std::string Flashcard::getAddedAt() const { return addedAt; }
void Flashcard::setAddedAt(const std::string& addedAt) { this->addedAt = addedAt; }

void Flashcard::displayCard() const {
    std::cout << "\n=======================================\n";
    std::cout << "[ FLASHCARD - MẶT TRƯỚC ]\n";
    std::cout << "  Từ vựng: " << word.getSpelling() << "\n";
    std::cout << "  (Được lưu vào lúc: " << addedAt << ")\n";
    std::cout << "=======================================\n";
    std::cout << "Nhấn Enter để lật thẻ học nghĩa...";
    std::cin.get(); // Chờ nhấn Enter
    std::cout << "\n[ FLASHCARD - MẶT SAU ]\n";
    word.displayCard();
    std::cout << "\nNhấn Enter để tiếp tục...";
    std::cin.get();
}
