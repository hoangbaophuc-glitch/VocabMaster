#include "BotAI.h"
#include <algorithm>
#include <cstdlib>
#include <set>
#include <cctype>

void BotAI::shuffleWords(std::vector<Word>& words) {
    if (words.empty()) return;
    for (int i = words.size() - 1; i > 0; --i) {
        int j = std::rand() % (i + 1);
        std::swap(words[i], words[j]);
    }
}

Word BotAI::chooseWord(char lastChar, const std::vector<std::string>& usedWords, 
                        const std::string& difficulty, const TrieTree& dictionary) {
    std::string prefix = "";
    prefix += std::tolower(lastChar);

    // Tìm tối đa 5000 từ bắt đầu bằng lastChar từ cây Trie
    std::vector<Word> possibleWords = dictionary.findWordsStartingWith(prefix, 5000, 12);

    // Lọc các từ đã bị dùng
    std::set<std::string> usedSet;
    for (const auto& w : usedWords) {
        std::string clean = w;
        std::transform(clean.begin(), clean.end(), clean.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        usedSet.insert(clean);
    }

    std::vector<Word> validWords;
    for (const auto& item : possibleWords) {
        std::string wordSpelling = item.getSpelling();
        std::transform(wordSpelling.begin(), wordSpelling.end(), wordSpelling.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        if (usedSet.find(wordSpelling) == usedSet.end()) {
            validWords.push_back(item);
        }
    }

    if (validWords.empty()) {
        return Word(); // Bot bị bí từ
    }

    // Heuristics dựa trên độ khó
    if (difficulty == "hard" || difficulty == "khó") {
        // Trộn ngẫu nhiên trước
        shuffleWords(validWords);

        // Đẩy các từ khó kết thúc bằng: x, y, z, q, k, v, w lên đầu
        std::sort(validWords.begin(), validWords.end(), [](const Word& a, const Word& b) {
            std::string sa = a.getSpelling();
            std::string sb = b.getSpelling();
            char charA = std::tolower(sa.empty() ? ' ' : sa.back());
            char charB = std::tolower(sb.empty() ? ' ' : sb.back());

            std::string rareChars = "xyzqkvw";
            int scoreA = (rareChars.find(charA) != std::string::npos) ? 100 : 0;
            int scoreB = (rareChars.find(charB) != std::string::npos) ? 100 : 0;
            return scoreB < scoreA; // Sắp xếp giảm dần điểm số (từ khó lên trước)
        });
    } else {
        // Chế độ dễ: Trộn ngẫu nhiên hoàn toàn
        shuffleWords(validWords);
    }

    return validWords[0];
}
