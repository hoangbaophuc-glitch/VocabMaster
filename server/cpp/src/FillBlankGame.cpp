#include "FillBlankGame.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <set>

FillBlankGame::FillBlankGame(const TrieTree& dict, int timeLimit, int maxAttempts)
    : GameMode(dict, timeLimit, maxAttempts) {}

std::string FillBlankGame::toLower(const std::string& s) const {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

std::string FillBlankGame::getCurrentTimestamp() const {
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

void FillBlankGame::generateNextRound() {
    secretWord = dictionary.getRandomWord(solvedHistory);
    std::string wordSpelling = secretWord.getSpelling();
    
    if (wordSpelling.empty()) {
        gameOver = true;
        endGameMessage = "Từ điển trống rỗng hoặc đã chơi hết tất cả các từ!";
        return;
    }

    blankedWord = wordSpelling;
    int hiddenCount = std::max(1, (int)wordSpelling.length() / 2);
    std::set<int> hiddenIndices;
    
    while ((int)hiddenIndices.size() < hiddenCount) {
        int r = std::rand() % wordSpelling.length();
        hiddenIndices.insert(r);
    }

    for (int idx : hiddenIndices) {
        blankedWord[idx] = '_';
    }

    wrongCount = 0;
}

void FillBlankGame::start() {
    score = 0;
    wrongCount = 0;
    gameOver = false;
    endGameMessage = "";
    solvedHistory.clear();
    generateNextRound();
    std::cout << "\n🎮 CHẾ ĐỘ ĐIỀN TỪ ĐÃ BẮT ĐẦU!\n";
}

bool FillBlankGame::playTurn(Player& player, const std::string& input, double timeTaken) {
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

void FillBlankGame::displayState() const {
    std::cout << "\n---------------------------------------\n";
    std::cout << " Điểm hiện tại: " << score << " | Lỗi: " << wrongCount << "/" << maxAttempts << "\n";
    std::cout << " Định nghĩa gợi ý: \n";
    std::cout << "   - Tiếng Anh: " << secretWord.getMeaningEn() << "\n";
    std::cout << "   - Tiếng Việt: " << secretWord.getMeaningVi() << "\n";
    std::cout << "---------------------------------------\n";
    std::cout << " Điền các chữ cái còn thiếu vào ô trống sau: \n";
    std::cout << " 👉 Từ cần điền:   ";
    for (char c : blankedWord) {
        std::cout << c << " ";
    }
    std::cout << "\n---------------------------------------\n";
}

void FillBlankGame::useHint(Player& player) {
    if (player.getHintCount() <= 0) {
        std::cout << "❌ Bạn không có Kính lúp gợi ý nào!\n";
        return;
    }

    player.setHintCount(player.getHintCount() - 1);
    std::cout << "🔍 [Gợi ý từ AI]: Đáp án chính là từ '" << secretWord.getSpelling() << "'. Hãy gõ lại từ này nhé!\n";
    player.saveToFile();
}

void FillBlankGame::useRevive(Player& player) {
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
