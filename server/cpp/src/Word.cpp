#include "Word.h"
#include <iostream>

Word::Word() : spelling(""), ipa(""), meaningEn(""), meaningVi(""), category("") {}

Word::Word(const std::string& spelling, const std::string& ipa, 
           const std::string& meaningEn, const std::string& meaningVi, 
           const std::string& category)
    : spelling(spelling), ipa(ipa), meaningEn(meaningEn), meaningVi(meaningVi), category(category) {}

std::string Word::getSpelling() const { return spelling; }
std::string Word::getIpa() const { return ipa; }
std::string Word::getMeaningEn() const { return meaningEn; }
std::string Word::getMeaningVi() const { return meaningVi; }
std::string Word::getCategory() const { return category; }

void Word::setSpelling(const std::string& spelling) { this->spelling = spelling; }
void Word::setIpa(const std::string& ipa) { this->ipa = ipa; }
void Word::setMeaningEn(const std::string& meaningEn) { this->meaningEn = meaningEn; }
void Word::setMeaningVi(const std::string& meaningVi) { this->meaningVi = meaningVi; }
void Word::setCategory(const std::string& category) { this->category = category; }

void Word::displayCard() const {
    std::cout << "=======================================\n";
    std::cout << " WORD:      " << spelling << "\n";
    if (!ipa.empty()) {
        std::cout << " IPA:       " << ipa << "\n";
    }
    std::cout << "---------------------------------------\n";
    std::cout << " Def (EN):  " << meaningEn << "\n";
    std::cout << " Def (VI):  " << meaningVi << "\n";
    std::cout << " Category:  " << category << "\n";
    std::cout << "=======================================\n";
}
