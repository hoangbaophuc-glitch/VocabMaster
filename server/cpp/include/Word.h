#ifndef WORD_H
#define WORD_H

#include <string>

class Word {
private:
    std::string spelling;
    std::string ipa;
    std::string meaningEn;
    std::string meaningVi;
    std::string category;

public:
    // Constructors
    Word();
    Word(const std::string& spelling, const std::string& ipa, 
         const std::string& meaningEn, const std::string& meaningVi, 
         const std::string& category);

    // Getters
    std::string getSpelling() const;
    std::string getIpa() const;
    std::string getMeaningEn() const;
    std::string getMeaningVi() const;
    std::string getCategory() const;

    // Setters
    void setSpelling(const std::string& spelling);
    void setIpa(const std::string& ipa);
    void setMeaningEn(const std::string& meaningEn);
    void setMeaningVi(const std::string& meaningVi);
    void setCategory(const std::string& category);

    // Utilities
    void displayCard() const;
};

#endif // WORD_H
