#include "GameController.h"
#include "WordChainGame.h"
#include "ScrambleGame.h"
#include "FillBlankGame.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>
#include <algorithm>

GameController::GameController() 
    : currentGame(nullptr), gameMode("chain"), difficulty("easy"), timeLimit(15), maxAttempts(3) {}

GameController::~GameController() {
    delete currentGame;
}

void GameController::showHeader() const {
    std::cout << "\n=======================================\n";
    std::cout << "   🚀 VocabMaster AI - TRÍ TUỆ TỪ VỰNG\n";
    std::cout << "=======================================\n";
    if (!player.getUsername().empty()) {
        std::cout << " 👤 Tài khoản: ";
        if (player.getIsVip()) {
            std::cout << "👑 \033[33m" << player.getUsername() << " [VIP]\033[0m"; // Màu vàng cho VIP
        } else {
            std::cout << player.getUsername();
        }
        std::cout << " | 💰 V-Coins: " << player.getVCoins() << "\n";
        std::cout << " 🏆 Kỷ lục: " << player.getHighscore() << " Điểm\n";
        std::cout << " 🎒 Vật phẩm: 🔍 Gợi ý (" << player.getHintCount() 
                  << ") | ❤️ Hồi sinh (" << player.getReviveCount() << ")\n";
        std::cout << "---------------------------------------\n";
    }
}

void GameController::run() {
    // Nạp từ điển khi bắt đầu
    std::cout << "⏳ Đang nạp từ điển từ file dictionary.json...\n";
    if (dictionary.loadFromFile("dictionary.json")) {
        std::cout << "🎉 Nạp thành công " << dictionary.getWordCount() << " từ vào cây Trie!\n";
    } else if (dictionary.loadFromFile("cpp/dictionary.json")) {
        std::cout << "🎉 Nạp thành công " << dictionary.getWordCount() << " từ vào cây Trie!\n";
    } else {
        std::cout << "❌ LỖI: Không tìm thấy file dictionary.json!\n";
        std::cout << "Ứng dụng sẽ hoạt động không có dữ liệu gốc. Bạn cần nhập file từ điển.\n";
    }

    // Màn hình Đăng nhập (AuthScreen)
    showHeader();
    std::string name;
    std::cout << "Nhập tên người chơi để bắt đầu: ";
    std::getline(std::cin, name);
    if (name.empty()) {
        name = "Player_Guest";
    }
    
    // Nạp dữ liệu tài khoản nếu đã lưu từ trước
    if (player.loadFromFile(name)) {
        std::cout << "👋 Chào mừng quay trở lại, " << name << "!\n";
    } else {
        player.setUsername(name);
        player.saveToFile();
        std::cout << "🎉 Đã khởi tạo tài khoản mới: " << name << " (Nhận 200 V-Coins khởi nghiệp)!\n";
    }

    showMainMenu();
}

void GameController::showMainMenu() {
    while (true) {
        showHeader();
        std::cout << "1. 🎮 CHIẾN (Bắt đầu trò chơi)\n";
        std::cout << "2. ⚙️ Tùy chỉnh chế độ chơi\n";
        std::cout << "3. 🛒 Cửa hàng vật phẩm\n";
        std::cout << "4. 📚 Thư viện Flashcard (" << player.getFlashcards().size() << " từ)\n";
        std::cout << "5. ❌ Thoát game\n";
        std::cout << "=======================================\n";
        std::cout << "Lựa chọn của bạn (1-5): ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "❌ Lựa chọn không hợp lệ!\n";
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (choice == 5) {
            std::cout << "👋 Tạm biệt " << player.getUsername() << "! Hẹn gặp lại bạn lần sau.\n";
            player.saveToFile();
            break;
        }

        switch (choice) {
            case 1:
                playGame();
                break;
            case 2:
                setSettingsMenu();
                break;
            case 3:
                store.showStoreMenu(player);
                break;
            case 4:
                showFlashcardsMenu();
                break;
            default:
                std::cout << "❌ Vui lòng chọn số từ 1 đến 5!\n";
        }
    }
}

void GameController::setSettingsMenu() {
    while (true) {
        std::cout << "\n=======================================\n";
        std::cout << "        ⚙️ CÀI ĐẶT TRÒ CHƠI\n";
        std::cout << "=======================================\n";
        std::cout << "1. Chế độ chơi:  " 
                  << (gameMode == "chain" ? "Nối từ" : (gameMode == "scramble" ? "Xếp chữ" : "Điền từ")) << "\n";
        std::cout << "2. Độ khó:       " << (difficulty == "easy" ? "Dễ (Bot dễ)" : "Khó (Bot Heuristic)") << "\n";
        std::cout << "3. Thời gian:    " << timeLimit << " giây\n";
        std::cout << "4. Số lượt lỗi:  " << (maxAttempts >= 9999 ? "Vô hạn" : std::to_string(maxAttempts)) << "\n";
        std::cout << "5. ⬅ Trở lại sảnh chính\n";
        std::cout << "=======================================\n";
        std::cout << "Chọn mục muốn thay đổi (1-5): ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "❌ Lựa chọn không hợp lệ!\n";
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (choice == 5) break;

        switch (choice) {
            case 1: {
                std::cout << "Chọn chế độ (1: Nối từ, 2: Xếp chữ, 3: Điền từ): ";
                int m;
                std::cin >> m;
                std::cin.ignore(10000, '\n');
                if (m == 1) gameMode = "chain";
                else if (m == 2) gameMode = "scramble";
                else if (m == 3) gameMode = "fillblank";
                break;
            }
            case 2: {
                std::cout << "Chọn độ khó (1: Dễ, 2: Khó): ";
                int d;
                std::cin >> d;
                std::cin.ignore(10000, '\n');
                if (d == 1) difficulty = "easy";
                else if (d == 2) difficulty = "hard";
                break;
            }
            case 3: {
                std::cout << "Chọn thời gian suy nghĩ (5s, 10s, 15s): ";
                int t;
                std::cin >> t;
                std::cin.ignore(10000, '\n');
                if (t == 5 || t == 10 || t == 15) timeLimit = t;
                break;
            }
            case 4: {
                std::cout << "Chọn giới hạn lượt sai (1, 3, 5, 0: Vô hạn): ";
                int a;
                std::cin >> a;
                std::cin.ignore(10000, '\n');
                if (a == 0) maxAttempts = 9999;
                else if (a > 0) maxAttempts = a;
                break;
            }
            default:
                std::cout << "❌ Lựa chọn không hợp lệ!\n";
        }
    }
}

void GameController::showFlashcardsMenu() {
    const auto& cards = player.getFlashcards();
    if (cards.empty()) {
        std::cout << "\n📚 Thư viện Flashcard trống rỗng! Hãy tham gia thi đấu để Bot tự động lưu từ mới vào đây nhé.\n";
        std::cout << "Nhấn Enter để quay lại...";
        std::string temp;
        std::getline(std::cin, temp);
        return;
    }

    while (true) {
        std::cout << "\n=======================================\n";
        std::cout << "        📚 THƯ VIỆN FLASHCARD\n";
        std::cout << "=======================================\n";
        std::cout << "Bạn đang có " << cards.size() << " từ trong sổ tay ôn tập.\n";
        std::cout << "1. Xem danh sách toàn bộ từ vựng\n";
        std::cout << "2. Lật thẻ học từng từ (Interactive)\n";
        std::cout << "3. ⬅ Trở lại sảnh chính\n";
        std::cout << "=======================================\n";
        std::cout << "Lựa chọn của bạn (1-3): ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (choice == 3) break;

        if (choice == 1) {
            std::cout << "\n--- DANH SÁCH TỪ ĐÃ LƯU ---\n";
            for (size_t i = 0; i < cards.size(); ++i) {
                std::cout << i + 1 << ". " << std::left << std::setw(15) << cards[i].getWord().getSpelling()
                          << " | Nghĩa: " << cards[i].getWord().getMeaningVi() << "\n";
            }
            std::cout << "\nNhấn Enter để quay lại...";
            std::string temp;
            std::getline(std::cin, temp);
        } else if (choice == 2) {
            for (size_t i = 0; i < cards.size(); ++i) {
                std::cout << "\n[Thẻ thứ " << i + 1 << " / " << cards.size() << "]";
                cards[i].displayCard();
                
                std::cout << "\nBạn có muốn ôn từ tiếp theo? (y: Có, n: Dừng lại): ";
                std::string ans;
                std::getline(std::cin, ans);
                if (ans != "y" && ans != "Y") {
                    break;
                }
            }
        }
    }
}

void GameController::playGame() {
    delete currentGame;
    currentGame = nullptr;

    if (gameMode == "chain") {
        currentGame = new WordChainGame(dictionary, timeLimit, maxAttempts, difficulty);
    } else if (gameMode == "scramble") {
        currentGame = new ScrambleGame(dictionary, timeLimit, maxAttempts);
    } else {
        currentGame = new FillBlankGame(dictionary, timeLimit, maxAttempts);
    }

    currentGame->start();

    while (true) {
        if (currentGame->isGameOver()) {
            std::cout << "\n=======================================\n";
            std::cout << "           KẾT THÚC TRẬN ĐẤU           \n";
            std::cout << "=======================================\n";
            std::cout << currentGame->getEndGameMessage() << "\n";
            std::cout << "=======================================\n";
            
            // Cập nhật kỷ lục và lưu file tài khoản
            player.setHighscore(currentGame->getScore());
            player.saveToFile();

            // Nếu người chơi có Bùa hồi sinh
            if (player.getReviveCount() > 0) {
                std::cout << "❤️ Bạn đang có " << player.getReviveCount() << " Bùa Hồi Sinh.\n";
                std::cout << "Bạn có muốn dùng Hồi Sinh để khôi phục HP và chơi tiếp? (y/n): ";
                std::string ans;
                std::getline(std::cin, ans);
                if (ans == "y" || ans == "Y") {
                    currentGame->useRevive(player);
                    continue; // Tiếp tục chơi tiếp ván hiện tại!
                }
            }
            break; // Thoát vòng lặp game
        }

        currentGame->displayState();
        std::cout << "Nhập câu trả lời (hoặc gõ '/hint' để gợi ý, '/surrender' để bỏ cuộc):\n";
        std::cout << "> ";
        
        auto startTime = std::chrono::steady_clock::now();
        std::string input;
        std::getline(std::cin, input);
        auto endTime = std::chrono::steady_clock::now();
        double timeTaken = std::chrono::duration<double>(endTime - startTime).count();

        // 1. Kiểm tra lệnh đặc biệt
        if (input == "/hint") {
            currentGame->useHint(player);
            continue;
        } else if (input == "/surrender") {
            std::cout << "🏳️ Bạn đã quyết định đầu hàng!\n";
            int coinsEarned = currentGame->getScore() / 4;
            player.addVCoins(coinsEarned);
            player.saveToFile();
            std::cout << "Nhận được 💰 " << coinsEarned << " V-Coins.\n";
            break;
        }

        // 2. Kiểm tra hết giờ (Timeout)
        if (timeTaken > timeLimit) {
            std::cout << "\n⏳ HẾT GIỜ! Bạn trả lời mất " << std::fixed << std::setprecision(1) << timeTaken 
                      << " giây (Thời gian cho phép: " << timeLimit << "s).\n";
            int coinsEarned = currentGame->getScore() / 2;
            player.addVCoins(coinsEarned);
            player.saveToFile();
            std::cout << "Game Over! Bạn nhận được 💰 " << coinsEarned << " V-Coins.\n";
            
            // Hỏi dùng bùa hồi sinh ngay khi hết giờ
            if (player.getReviveCount() > 0) {
                std::cout << "❤️ Bạn đang có " << player.getReviveCount() << " Bùa Hồi Sinh.\n";
                std::cout << "Bạn có muốn dùng Hồi Sinh để tiếp tục không? (y/n): ";
                std::string ans;
                std::getline(std::cin, ans);
                if (ans == "y" || ans == "Y") {
                    player.setReviveCount(player.getReviveCount() - 1);
                    std::cout << "❤️ Hồi sinh thành công! Bắt đầu lại lượt đấu...\n";
                    player.saveToFile();
                    continue;
                }
            }
            break;
        }

        // 3. Thực hiện lượt chơi
        currentGame->playTurn(player, input, timeTaken);
    }

    std::cout << "Nhấn Enter để quay lại sảnh chờ...";
    std::string temp;
    std::getline(std::cin, temp);
}
