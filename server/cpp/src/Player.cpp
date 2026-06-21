#include "Player.h"
#include <fstream>
#include <iostream>

Player::Player() 
    : username(""), vCoins(200), highscore(0), hintCount(0), reviveCount(0), isVip(false) {}

Player::Player(const std::string& username) 
    : username(username), vCoins(200), highscore(0), hintCount(0), reviveCount(0), isVip(false) {}

std::string Player::getUsername() const { return username; }
int Player::getVCoins() const { return vCoins; }
int Player::getHighscore() const { return highscore; }
int Player::getHintCount() const { return hintCount; }
int Player::getReviveCount() const { return reviveCount; }
bool Player::getIsVip() const { return isVip; }
const std::vector<Flashcard>& Player::getFlashcards() const { return flashcards; }

void Player::setUsername(const std::string& username) { this->username = username; }
void Player::setVCoins(int coins) { this->vCoins = coins; }
void Player::addVCoins(int amount) { this->vCoins += amount; }
void Player::setHighscore(int score) { if (score > highscore) highscore = score; }
void Player::setHintCount(int count) { this->hintCount = count; }
void Player::setReviveCount(int count) { this->reviveCount = count; }
void Player::setIsVip(bool vip) { this->isVip = vip; }

void Player::addFlashcard(const Word& word, const std::string& addedAt) {
    // Tránh trùng lặp từ trong danh sách flashcard
    for (const auto& card : flashcards) {
        if (card.getWord().getSpelling() == word.getSpelling()) {
            return;
        }
    }
    flashcards.push_back(Flashcard(word, addedAt));
}

bool Player::loadFromFile(const std::string& name) {
    this->username = name;
    std::ifstream file(username + "_data.txt");
    if (!file.is_open()) {
        // Tài khoản mới tạo, sử dụng thiết lập mặc định
        vCoins = 200;
        highscore = 0;
        hintCount = 0;
        reviveCount = 0;
        isVip = false;
        flashcards.clear();
        return false;
    }

    file >> vCoins >> highscore >> hintCount >> reviveCount >> isVip;
    file.ignore(); // Bỏ qua ký tự xuống dòng

    flashcards.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Tách dữ liệu bằng dấu gạch đứng |
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);

        if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos ||
            p4 == std::string::npos || p5 == std::string::npos) {
            continue;
        }

        std::string spelling = line.substr(0, p1);
        std::string ipa = line.substr(p1 + 1, p2 - p1 - 1);
        std::string en = line.substr(p2 + 1, p3 - p2 - 1);
        std::string vi = line.substr(p3 + 1, p4 - p3 - 1);
        std::string cat = line.substr(p4 + 1, p5 - p4 - 1);
        std::string ts = line.substr(p5 + 1);

        flashcards.push_back(Flashcard(Word(spelling, ipa, en, vi, cat), ts));
    }
    file.close();
    return true;
}

void Player::saveToFile() const {
    if (username.empty()) return;
    std::ofstream file(username + "_data.txt");
    if (!file.is_open()) return;

    file << vCoins << " " << highscore << " " << hintCount << " " << reviveCount << " " << isVip << "\n";
    for (const auto& card : flashcards) {
        Word w = card.getWord();
        file << w.getSpelling() << "|" << w.getIpa() << "|" << w.getMeaningEn() << "|"
             << w.getMeaningVi() << "|" << w.getCategory() << "|" << card.getAddedAt() << "\n";
    }
    file.close();
}
