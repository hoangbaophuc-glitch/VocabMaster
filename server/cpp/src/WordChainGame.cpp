#include "WordChainGame.h"
#include "BotAI.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

WordChainGame::WordChainGame(const TrieTree& dict, int timeLimit, int maxAttempts, const std::string& diff)
    : GameMode(dict, timeLimit, maxAttempts), difficulty(diff) {}

std::string WordChainGame::toLower(const std::string& s) const {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

std::string WordChainGame::getCurrentTimestamp() const {
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

void WordChainGame::start() {
    score = 0;
    wrongCount = 0;
    history.clear();
    gameOver = false;
    endGameMessage = "";
    currentBotWord = Word();
    std::cout << "\n🎮 CHẾ ĐỘ NỐI TỪ VỚI BOT ĐÃ BẮT ĐẦU!\n";
}

bool WordChainGame::playTurn(Player& player, const std::string& input, double timeTaken) {
    if (gameOver) return false;

    std::string cleanInput = toLower(input);
    
    // 1. Loại bỏ khoảng trắng thừa
    cleanInput.erase(cleanInput.begin(), std::find_if(cleanInput.begin(), cleanInput.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    cleanInput.erase(std::find_if(cleanInput.rbegin(), cleanInput.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), cleanInput.end());

    if (cleanInput.empty()) {
        std::cout << "❌ Vui lòng nhập từ.\n";
        return false;
    }

    // 2. Kiểm duyệt ký tự (chỉ cho phép chữ cái)
    if (!std::all_of(cleanInput.begin(), cleanInput.end(), ::isalpha)) {
        std::cout << "❌ Chỉ được nhập chữ cái tiếng Anh hợp lệ.\n";
        return false;
    }

    // 3. Kiểm tra luật nối từ (chữ đầu trùng chữ cuối của Bot)
    std::string botSpelling = currentBotWord.getSpelling();
    if (!botSpelling.empty() && cleanInput.front() != std::tolower(botSpelling.back())) {
        wrongCount++;
        std::cout << "❌ Sai luật! Từ của bạn phải bắt đầu bằng chữ '" 
                  << (char)std::toupper(botSpelling.back()) << "'.\n";
        if (wrongCount >= maxAttempts) {
            gameOver = true;
            int coinsEarned = score / 2;
            player.addVCoins(coinsEarned);
            std::stringstream ss;
            ss << "Thất bại! Bạn đã hết lượt (Sai " << wrongCount << " lần). Điểm đạt được: " << score 
               << ". Nhận được 💰 " << coinsEarned << " V-Coins.";
            endGameMessage = ss.str();
        } else {
            std::cout << "HP còn lại: " << (maxAttempts - wrongCount) << "/" << maxAttempts << "\n";
        }
        return false;
    }

    // 4. Kiểm tra xem từ có tồn tại trong từ điển không
    if (!dictionary.isValidWord(cleanInput)) {
        wrongCount++;
        std::cout << "❌ Từ '" << cleanInput << "' không tồn tại trong từ điển!\n";
        if (wrongCount >= maxAttempts) {
            gameOver = true;
            int coinsEarned = score / 2;
            player.addVCoins(coinsEarned);
            std::stringstream ss;
            ss << "Thất bại! Bạn đã hết lượt (Sai " << wrongCount << " lần). Điểm đạt được: " << score 
               << ". Nhận được 💰 " << coinsEarned << " V-Coins.";
            endGameMessage = ss.str();
        } else {
            std::cout << "HP còn lại: " << (maxAttempts - wrongCount) << "/" << maxAttempts << "\n";
        }
        return false;
    }

    // 5. Kiểm tra trùng lặp
    if (std::find(history.begin(), history.end(), cleanInput) != history.end()) {
        wrongCount++;
        std::cout << "❌ Từ này đã được sử dụng trước đó trong trận đấu!\n";
        if (wrongCount >= maxAttempts) {
            gameOver = true;
            int coinsEarned = score / 2;
            player.addVCoins(coinsEarned);
            std::stringstream ss;
            ss << "Thất bại! Bạn đã hết lượt (Sai " << wrongCount << " lần). Điểm đạt được: " << score 
               << ". Nhận được 💰 " << coinsEarned << " V-Coins.";
            endGameMessage = ss.str();
        } else {
            std::cout << "HP còn lại: " << (maxAttempts - wrongCount) << "/" << maxAttempts << "\n";
        }
        return false;
    }

    // Đúng luật! Cộng điểm
    wrongCount = 0;
    int turnScore = cleanInput.length() * 10;
    bool isCombo = false;
    if (timeTaken < 5.0) {
        turnScore *= 2;
        isCombo = true;
    }
    score += turnScore;
    history.push_back(cleanInput);

    std::cout << "✨ Chấp nhận! " << (isCombo ? "🔥 COMBO X2 Tốc độ! " : "") << "+" << turnScore << " Điểm.\n";

    // 6. Bot phản công
    char lastChar = cleanInput.back();
    Word botResponse = BotAI::chooseWord(lastChar, history, difficulty, dictionary);

    if (botResponse.getSpelling().empty()) {
        // Bot bị bí -> Người chơi chiến thắng!
        gameOver = true;
        int coinsEarned = score + 500;
        player.addVCoins(coinsEarned);
        player.setHighscore(score);
        std::stringstream ss;
        ss << "🎉 CHIẾN THẮNG! Bot đã bí từ. Thưởng nóng 500 V-Coins & " << score << " Điểm! Nhận tổng cộng 💰 " << coinsEarned << " V-Coins.";
        endGameMessage = ss.str();
        return true;
    }

    // Bot ra từ thành công
    currentBotWord = botResponse;
    history.push_back(toLower(currentBotWord.getSpelling()));
    
    // Tự động lưu từ mới của Bot vào Flashcard của người chơi
    player.addFlashcard(currentBotWord, getCurrentTimestamp());
    player.saveToFile();

    return true;
}

void WordChainGame::displayState() const {
    std::cout << "\n---------------------------------------\n";
    std::cout << " Điểm hiện tại: " << score << " | Lỗi: " << wrongCount << "/" << maxAttempts << "\n";
    if (history.empty()) {
        std::cout << " Lượt đầu tiên! Hãy gõ một từ tiếng Anh bất kỳ để khai chiến.\n";
    } else {
        std::cout << " Lịch sử nối từ gần nhất (tối đa 5 từ):\n";
        int size = history.size();
        int startIdx = std::max(0, size - 5);
        for (int i = startIdx; i < size; ++i) {
            std::cout << "   " << (i % 2 == 0 ? "👤 Bạn" : "🤖 Bot") << ": " << history[i] << "\n";
        }
        std::cout << "---------------------------------------\n";
        std::cout << "🤖 Bot vừa ra từ: " << currentBotWord.getSpelling() << "\n";
        std::cout << "   IPA: " << currentBotWord.getIpa() << "\n";
        std::cout << "   Nghĩa EN: " << currentBotWord.getMeaningEn() << "\n";
        std::cout << "👉 Bạn phải gõ một từ bắt đầu bằng chữ: '" 
                  << (char)std::toupper(currentBotWord.getSpelling().back()) << "'\n";
    }
    std::cout << "---------------------------------------\n";
}

void WordChainGame::useHint(Player& player) {
    if (player.getHintCount() <= 0) {
        std::cout << "❌ Bạn không có kính lúp gợi ý nào! Vui lòng vào Cửa hàng để mua.\n";
        return;
    }

    std::string botSpelling = currentBotWord.getSpelling();
    if (botSpelling.empty()) {
        std::cout << "⚠️ Đây là lượt đầu tiên, bạn có thể gõ từ bất kỳ, không cần gợi ý!\n";
        return;
    }

    char lastChar = botSpelling.back();
    Word hintWord = BotAI::chooseWord(lastChar, history, "easy", dictionary);

    if (hintWord.getSpelling().empty()) {
        std::cout << "⚠️ Kho từ điển đã cạn kiệt, không tìm thấy từ gợi ý phù hợp!\n";
    } else {
        player.setHintCount(player.getHintCount() - 1);
        std::cout << "🔍 [Gợi ý từ AI]: Bạn có thể dùng từ '" << hintWord.getSpelling() << "' (" << hintWord.getMeaningVi() << ").\n";
        player.saveToFile();
    }
}

void WordChainGame::useRevive(Player& player) {
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
    std::cout << "❤️ Sử dụng Bùa Hồi Sinh thành công! Bạn được tiếp tục chơi và hồi phục HP.\n";
    player.saveToFile();
}
