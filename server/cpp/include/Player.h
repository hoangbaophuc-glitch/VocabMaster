#ifndef PLAYER_H
#define PLAYER_H

#include "Flashcard.h"
#include "Word.h"
#include <string>
#include <vector>

class Player {
private:
    std::string username;
    int vCoins;
    int highscore;
    int hintCount;
    int reviveCount;
    bool isVip;
    std::vector<Flashcard> flashcards;

public:
    Player();
    Player(const std::string& username);

    // Getters
    std::string getUsername() const;
    int getVCoins() const;
    int getHighscore() const;
    int getHintCount() const;
    int getReviveCount() const;
    bool getIsVip() const;
    const std::vector<Flashcard>& getFlashcards() const;

    // Setters & Modifiers
    void setUsername(const std::string& username);
    void setVCoins(int coins);
    void addVCoins(int amount);
    void setHighscore(int score);
    void setHintCount(int count);
    void setReviveCount(int count);
    void setIsVip(bool vip);
    
    void addFlashcard(const Word& word, const std::string& addedAt);
    
    // Lưu và nạp dữ liệu người chơi vào file cục bộ
    void saveToFile() const;
    bool loadFromFile(const std::string& username);
};

#endif // PLAYER_H
