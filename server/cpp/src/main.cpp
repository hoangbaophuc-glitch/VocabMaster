#include "GameController.h"
#include "TrieTree.h"
#include "BotAI.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <set>

// Hàm helper để escape chuỗi JSON an toàn
std::string escapeJson(const std::string& input) {
    std::string output = "";
    for (char c : input) {
        if (c == '"') output += "\\\"";
        else if (c == '\\') output += "\\\\";
        else if (c == '\n') output += "\\n";
        else if (c == '\r') output += "\\r";
        else if (c == '\t') output += "\\t";
        else output += c;
    }
    return output;
}

// Hàm helper để split chuỗi bằng ký tự phân tách
std::vector<std::string> splitString(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    // Khởi tạo hạt giống ngẫu nhiên dựa trên thời gian thực
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Chế độ API phục vụ cho Server Node.js gọi tiến trình con (child_process)
    if (argc > 1) {
        TrieTree dictionary;
        // Kiểm tra và nạp từ điển từ 3 đường dẫn có thể xảy ra (Chạy trực tiếp, chạy từ root, chạy từ Server Render)
        if (!dictionary.loadFromFile("dictionary.json") && 
            !dictionary.loadFromFile("cpp/dictionary.json") && 
            !dictionary.loadFromFile("../cpp/dictionary.json")) {
            std::cout << "{\"success\":false,\"error\":\"Không thể nạp tệp từ điển dictionary.json\"}\n";
            return 1;
        }

        std::string command = argv[1];

        // LƯỢT CHƠI CHÍNH (--play)
        if (command == "--play") {
            if (argc < 3) {
                std::cout << "{\"success\":false,\"error\":\"Thiếu đối số cho --play (cần tối thiểu từ của người chơi)\"}\n";
                return 1;
            }
            std::string playerWord = argv[2];
            std::string usedWordsStr = (argc > 3) ? argv[3] : "";
            std::string botLastChar = (argc > 4) ? argv[4] : "";
            std::string difficulty = (argc > 5) ? argv[5] : "easy";
            int timeLimit = (argc > 6) ? std::atoi(argv[6]) : 15;
            double timeTaken = (argc > 7) ? std::atof(argv[7]) : 0.0;

            // Làm sạch từ của người chơi
            std::string cleanWord = playerWord;
            std::transform(cleanWord.begin(), cleanWord.end(), cleanWord.begin(), [](unsigned char c) {
                return std::tolower(c);
            });
            // Trim
            cleanWord.erase(cleanWord.begin(), std::find_if(cleanWord.begin(), cleanWord.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            cleanWord.erase(std::find_if(cleanWord.rbegin(), cleanWord.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), cleanWord.end());

            if (cleanWord.empty()) {
                std::cout << "{\"success\":false,\"error\":\"Vui lòng nhập từ.\"}\n";
                return 0;
            }

            // Chặn số và ký tự đặc biệt
            if (!std::all_of(cleanWord.begin(), cleanWord.end(), ::isalpha)) {
                std::cout << "{\"success\":false,\"error\":\"Chỉ được nhập chữ cái tiếng Anh hợp lệ.\"}\n";
                return 0;
            }

            // Kiểm tra luật nối chữ
            if (!botLastChar.empty() && cleanWord.front() != std::tolower(botLastChar[0])) {
                std::string errMsg = "Từ phải bắt đầu bằng chữ '";
                errMsg += std::toupper(botLastChar[0]);
                errMsg += "'.";
                std::cout << "{\"success\":false,\"error\":\"" << errMsg << "\"}\n";
                return 0;
            }

            // Kiểm tra từ điển
            if (!dictionary.isValidWord(cleanWord)) {
                std::string errMsg = "Từ '" + cleanWord + "' không có thật trong tiếng Anh!";
                std::cout << "{\"success\":false,\"error\":\"" << errMsg << "\"}\n";
                return 0;
            }

            // Phân tích danh sách từ đã sử dụng
            std::vector<std::string> usedWords = splitString(usedWordsStr, ',');
            std::set<std::string> usedSet;
            for (const auto& w : usedWords) {
                std::string cw = w;
                std::transform(cw.begin(), cw.end(), cw.begin(), [](unsigned char c) { return std::tolower(c); });
                usedSet.insert(cw);
            }

            // Kiểm tra trùng lặp
            if (usedSet.find(cleanWord) != usedSet.end()) {
                std::cout << "{\"success\":false,\"error\":\"Từ này đã được sử dụng trong ván đấu.\"}\n";
                return 0;
            }

            // Lấy nghĩa của từ người chơi nhập
            Word playerWordDetails = dictionary.getWordDetails(cleanWord);

            // Tính điểm lượt đi
            int score = cleanWord.length() * 10;
            bool isCombo = false;
            if (timeTaken < 5.0) {
                score *= 2;
                isCombo = true;
            }

            // Thêm từ của người chơi vào lịch sử
            usedWords.push_back(cleanWord);

            // Bot trả lời
            char lastLetter = cleanWord.back();
            Word botWordResponse = BotAI::chooseWord(lastLetter, usedWords, difficulty, dictionary);

            if (botWordResponse.getSpelling().empty()) {
                // Bot đã bí từ
                std::cout << "{";
                std::cout << "\"success\":true,";
                std::cout << "\"score\":" << score << ",";
                std::cout << "\"isCombo\":" << (isCombo ? "true" : "false") << ",";
                std::cout << "\"playerDefinition\":{";
                std::cout << "\"en\":\"" << escapeJson(playerWordDetails.getMeaningEn()) << "\",";
                std::cout << "\"vi\":\"" << escapeJson(playerWordDetails.getMeaningVi()) << "\",";
                std::cout << "\"ipa\":\"" << escapeJson(playerWordDetails.getIpa()) << "\"";
                std::cout << "},";
                std::cout << "\"botWord\":null,";
                std::cout << "\"botDefinition\":null,";
                std::cout << "\"message\":\"Bot đã bí! Bạn là người chiến thắng!\"";
                std::cout << "}\n";
                return 0;
            }

            // Bot ra từ thành công
            usedWords.push_back(botWordResponse.getSpelling());

            std::cout << "{";
            std::cout << "\"success\":true,";
            std::cout << "\"score\":" << score << ",";
            std::cout << "\"isCombo\":" << (isCombo ? "true" : "false") << ",";
            std::cout << "\"playerDefinition\":{";
            std::cout << "\"en\":\"" << escapeJson(playerWordDetails.getMeaningEn()) << "\",";
            std::cout << "\"vi\":\"" << escapeJson(playerWordDetails.getMeaningVi()) << "\",";
            std::cout << "\"ipa\":\"" << escapeJson(playerWordDetails.getIpa()) << "\"";
            std::cout << "},";
            std::cout << "\"botWord\":\"" << escapeJson(botWordResponse.getSpelling()) << "\",";
            std::cout << "\"botDefinition\":{";
            std::cout << "\"en\":\"" << escapeJson(botWordResponse.getMeaningEn()) << "\",";
            std::cout << "\"vi\":\"" << escapeJson(botWordResponse.getMeaningVi()) << "\",";
            std::cout << "\"ipa\":\"" << escapeJson(botWordResponse.getIpa()) << "\"";
            std::cout << "},";
            std::cout << "\"updatedUsedWords\":[";
            for (size_t i = 0; i < usedWords.size(); ++i) {
                std::cout << "\"" << escapeJson(usedWords[i]) << "\"";
                if (i < usedWords.size() - 1) std::cout << ",";
            }
            std::cout << "]";
            std::cout << "}\n";
            return 0;
        }

        // MINI-GAMES (--minigame)
        else if (command == "--minigame") {
            if (argc < 3) {
                std::cout << "{\"success\":false,\"error\":\"Thiếu đối số cho --minigame\"}\n";
                return 1;
            }
            std::string mode = argv[2];
            std::string usedWordsStr = (argc > 3) ? argv[3] : "";

            std::vector<std::string> usedWords = splitString(usedWordsStr, ',');
            Word data = dictionary.getRandomWord(usedWords);
            if (data.getSpelling().empty()) {
                std::cout << "{\"success\":false,\"error\":\"Từ điển trống!\"}\n";
                return 0;
            }

            std::string word = data.getSpelling();

            std::cout << "{";
            std::cout << "\"success\":true,";
            std::cout << "\"word\":\"" << escapeJson(word) << "\",";
            std::cout << "\"definition\":{";
            std::cout << "\"en\":\"" << escapeJson(data.getMeaningEn()) << "\",";
            std::cout << "\"vi\":\"" << escapeJson(data.getMeaningVi()) << "\",";
            std::cout << "\"ipa\":\"" << escapeJson(data.getIpa()) << "\"";
            std::cout << "},";

            if (mode == "scramble") {
                std::string scrambled = word;
                int attempts = 0;
                while (scrambled == word && word.length() > 1 && attempts < 10) {
                    for (int i = scrambled.length() - 1; i > 0; i--) {
                        int j = std::rand() % (i + 1);
                        std::swap(scrambled[i], scrambled[j]);
                    }
                    attempts++;
                }
                std::cout << "\"scrambled\":\"" << escapeJson(scrambled) << "\"";
            } else if (mode == "fillblank") {
                std::string blanked = word;
                int hiddenCount = std::max(1, (int)word.length() / 2);
                std::set<int> hiddenIndices;
                while ((int)hiddenIndices.size() < hiddenCount) {
                    int r = std::rand() % word.length();
                    hiddenIndices.insert(r);
                }
                std::string blankedSpaced = "";
                for (size_t i = 0; i < word.length(); ++i) {
                    if (hiddenIndices.find(i) != hiddenIndices.end()) {
                        blankedSpaced += "_ ";
                    } else {
                        blankedSpaced += word[i];
                        blankedSpaced += " ";
                    }
                }
                if (!blankedSpaced.empty()) {
                    blankedSpaced.pop_back(); // Xóa khoảng trắng thừa ở cuối
                }
                std::cout << "\"blanked\":\"" << escapeJson(blankedSpaced) << "\"";
            }
            std::cout << "}\n";
            return 0;
        }

        // GỢI Ý TỪ VỰNG (--hint)
        else if (command == "--hint") {
            if (argc < 3) {
                std::cout << "{\"success\":false,\"error\":\"Thiếu đối số cho --hint\"}\n";
                return 1;
            }
            std::string lastCharStr = argv[2];
            std::string usedWordsStr = (argc > 3) ? argv[3] : "";

            if (lastCharStr.empty()) {
                std::cout << "{\"success\":false,\"error\":\"Thiếu chữ cái bắt đầu\"}\n";
                return 0;
            }

            char lastChar = std::tolower(lastCharStr[0]);
            std::string prefix = "";
            prefix += lastChar;

            std::vector<Word> possibleWords = dictionary.findWordsStartingWith(prefix, 5000, 12);
            std::vector<std::string> usedWords = splitString(usedWordsStr, ',');
            
            std::set<std::string> usedSet;
            for (const auto& w : usedWords) {
                std::string cw = w;
                std::transform(cw.begin(), cw.end(), cw.begin(), [](unsigned char c) { return std::tolower(c); });
                usedSet.insert(cw);
            }

            std::vector<Word> validWords;
            for (const auto& item : possibleWords) {
                std::string spelling = item.getSpelling();
                std::transform(spelling.begin(), spelling.end(), spelling.begin(), [](unsigned char c) { return std::tolower(c); });
                if (usedSet.find(spelling) == usedSet.end()) {
                    validWords.push_back(item);
                }
            }

            if (validWords.empty()) {
                std::cout << "{\"success\":false,\"error\":\"Kho từ điển đã cạn kiệt, không tìm thấy gợi ý!\"}\n";
                return 0;
            }

            int randomIndex = std::rand() % validWords.size();
            std::string hintWord = validWords[randomIndex].getSpelling();

            std::cout << "{\"success\":true,\"hint\":\"" << escapeJson(hintWord) << "\"}\n";
            return 0;
        }

        std::cout << "{\"success\":false,\"error\":\"Lệnh CLI không hợp lệ\"}\n";
        return 1;
    }

    // --- CHẾ ĐỘ CONSOLE INTERACTIVE TRUYỀN THỐNG ---
    // Kích hoạt bảng mã UTF-8 trên Windows Console để hiển thị tiếng Việt có dấu chính xác
#ifdef _WIN32
    std::system("chcp 65001 > nul");
#endif

    std::cout << "=======================================\n";
    std::cout << "     ĐANG KHỞI CHẠY VOCABMASTER AI     \n";
    std::cout << "=======================================\n";

    // Khởi tạo và chạy bộ điều khiển trò chơi
    GameController controller;
    controller.run();

    return 0;
}
