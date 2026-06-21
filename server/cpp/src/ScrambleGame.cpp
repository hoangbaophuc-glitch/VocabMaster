#include "ScrambleGame.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdlib>

ScrambleGame::ScrambleGame(const TrieTree& dict, int timeLimit, int maxAttempts)
    : GameMode(dict, timeLimit, maxAttempts) {}

std::string ScrambleGame::toLower(const std::string& s) const {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

std::string ScrambleGame::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm parts;
#ifdef _WIN32
    localtime_s(&parts, &now_c);
#else
    localtime_r(&now_c, &parts);
#endif
    std::stringstream ss;
    ss << std::put_time(&parts, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void ScrambleGame::generateNextRound() {
    secretWord = dictionary.getRandomWord(solvedHistory);
    std::string wordSpelling = secretWord.getSpelling();
    
    if (wordSpelling.empty()) {
        gameOver = true;
        endGameMessage = "Từ điển trống rỗng hoặc đã chơi hết tất cả các từ!";
        return;
    }

    scrambledWord = wordSpelling;
    int attempts = 0;
    
    // Thực hiện trộn chữ cái
    while (scrambledWord == wordSpelling && wordSpelling.length() > 1 && attempts < 10) {
        for (int i = scrambledWord.length() - 1; i > 0; i--) {
            int j = std::rand() % (i + 1);
            std::swap(scrambledWord[i], scrambledWord[j]);
        }
        attempts++;
    }
    
    wrongCount = 0;
}

void ScrambleGame::start() {
    score = 0;
    wrongCount = 0;
    gameOver = false;
    endGameMessage = "";
    solvedHistory.clear();
    generateNextRound();
    std::cout << "\n🎮 CHẾ ĐỘ XẾP CHỮ ĐÃ BẮT ĐẦU!\n";
}

bool ScrambleGame::playTurn(Player& player, const std::string& input, double timeTaken) {
    if (gameOver) return false;

    std::string cleanInput = toLower(input);
    std::string cleanSecret = toLower(secretWord.getSpelling());

    if (cleanInput == cleanSecret) {
        // Trả lời chính xác!
        std::cout << "🎉 Chính xác! Đáp án đúng là: " << secretWord.getSpelling() << "\n";
        
        int earnedScore = (timeLimit - (int)timeTaken + 10) * 5;
        score += earnedScore;
        
        solvedHistory.push_back(cleanSecret);
        
        // Lưu từ này vào Flashcards
        player.addFlashcard(secretWord, getCurrentTimestamp());
        player.saveToFile();

        // Sang màn mới
        generateNextRound();
        return true;
    } else {
        // Trả lời sai
        wrongCount++;
        std::cout << "❌ Sai rồi!\n";
        if (wrongCount >= maxAttempts) {
            gameOver = true;
            int coinsEarned = score / 2;
            player.addVCoins(coinsEarned);
            std::stringstream ss;
            ss << "Thua cuộc! Bạn đã sai quá " << maxAttempts << " lần. Đáp án đúng là: " 
               << secretWord.getSpelling() << " (" << secretWord.getMeaningVi() << "). Nhận được 💰 " << coinsEarned << " V-Coins.";
            endGameMessage = ss.str();
        } else {
            std::cout << "HP còn lại: " << (maxAttempts - wrongCount) << "/" << maxAttempts << "\n";
        }
        return false;
    }
}

void ScrambleGame::displayState() const {
    std::cout << "\n---------------------------------------\n";
    std::cout << " Điểm hiện tại: " << score << " | Lỗi: " << wrongCount << "/" << maxAttempts << "\n";
    std::cout << " Định nghĩa gợi ý: \n";
    std::cout << "   - Tiếng Anh: " << secretWord.getMeaningEn() << "\n";
    std::cout << "   - Tiếng Việt: " << secretWord.getMeaningVi() << "\n";
    std::cout << "---------------------------------------\n";
    std::cout << " Sắp xếp các chữ cái sau thành từ có nghĩa: \n";
    std::cout << " 👉 Từ xáo trộn:   " << toLower(scrambledWord) << "\n";
    std::cout << "---------------------------------------\n";
}

void ScrambleGame::useHint(Player& player) {
    if (player.getHintCount() <= 0) {
        std::cout << "❌ Bạn không có Kính lúp gợi ý nào!\n";
        return;
    }

    player.setHintCount(player.getHintCount() - 1);
    std::cout << "🔍 [Gợi ý từ AI]: Đáp án chính là từ '" << secretWord.getSpelling() << "'. Hãy gõ lại từ này nhé!\n";
    player.saveToFile();
}

void ScrambleGame::useRevive(Player& player) {
    if (player.getReviveCount() <= 0) {
        std::cout << "❌ Bạn không có Bùa Hồi Sinh nào!\n";
        return;
    }

    if (!gameOver) {
        std::cout << "⚠️ Trò chơi chưa kết thúc, bạn chưa cần dùng Hồi sinh.\n";
        return;
    }

    player.setReviveCount(player.getReviveCount() - 1);
    gameOver = false;
    wrongCount = 0;
    endGameMessage = "";
    std::cout << "❤️ Sử dụng Bùa Hồi Sinh thành công! Bạn tiếp tục giải từ này.\n";
    player.saveToFile();
}
