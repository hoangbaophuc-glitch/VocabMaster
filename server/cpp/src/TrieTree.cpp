#include "TrieTree.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <set>

TrieTree::TrieTree() : root(new TrieNode()), wordCount(0) {}

TrieTree::~TrieTree() {
    delete root; // Bộ hủy của TrieNode sẽ đệ quy hủy các con của nó
}

std::string TrieTree::toLower(const std::string& s) const {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

std::string TrieTree::extractString(const std::string& str, size_t& startPos) {
    size_t firstQuote = str.find('"', startPos);
    if (firstQuote == std::string::npos) return "";
    size_t secondQuote = str.find('"', firstQuote + 1);
    if (secondQuote == std::string::npos) return "";
    startPos = secondQuote + 1;
    return str.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

void TrieTree::insert(const Word& word) {
    TrieNode* current = root;
    std::string cleanWord = toLower(word.getSpelling());
    
    for (char c : cleanWord) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = new TrieNode();
        }
        current = current->children[c];
    }
    
    if (!current->isEndOfWord) {
        wordCount++;
        wordList.push_back(word);
    }
    current->isEndOfWord = true;
    current->wordData = word;
}

bool TrieTree::isValidWord(const std::string& word) const {
    TrieNode* current = root;
    std::string cleanWord = toLower(word);
    
    for (char c : cleanWord) {
        if (current->children.find(c) == current->children.end()) {
            return false;
        }
        current = current->children.at(c);
    }
    return current->isEndOfWord;
}

Word TrieTree::getWordDetails(const std::string& word) const {
    TrieNode* current = root;
    std::string cleanWord = toLower(word);
    
    for (char c : cleanWord) {
        if (current->children.find(c) == current->children.end()) {
            return Word();
        }
        current = current->children.at(c);
    }
    if (current->isEndOfWord) {
        return current->wordData;
    }
    return Word();
}

Word TrieTree::getRandomWord(const std::vector<std::string>& usedWords) const {
    if (wordList.empty()) return Word();

    if (usedWords.empty()) {
        int randomIndex = std::rand() % wordList.size();
        return wordList[randomIndex];
    }

    std::set<std::string> usedSet;
    for (const auto& w : usedWords) {
        usedSet.insert(toLower(w));
    }

    std::vector<Word> validWords;
    for (const auto& item : wordList) {
        if (usedSet.find(toLower(item.getSpelling())) == usedSet.end()) {
            validWords.push_back(item);
        }
    }

    if (validWords.empty()) {
        // Hết từ mới, lấy đại 1 từ cũ bất kỳ
        int randomIndex = std::rand() % wordList.size();
        return wordList[randomIndex];
    }

    int randomIndex = std::rand() % validWords.size();
    return validWords[randomIndex];
}

void TrieTree::dfs(TrieNode* node, const std::string& currentWord, std::vector<Word>& results, int maxWords, int maxLength) const {
    if (results.size() >= (size_t)maxWords) return;
    if (currentWord.length() > (size_t)maxLength) return;

    if (node->isEndOfWord) {
        results.push_back(node->wordData);
    }

    for (const auto& pair : node->children) {
        dfs(pair.second, currentWord + pair.first, results, maxWords, maxLength);
    }
}

std::vector<Word> TrieTree::findWordsStartingWith(const std::string& prefix, int maxWords, int maxLength) const {
    TrieNode* current = root;
    std::vector<Word> results;
    std::string cleanPrefix = toLower(prefix);

    for (char c : cleanPrefix) {
        if (current->children.find(c) == current->children.end()) {
            return results;
        }
        current = current->children.at(c);
    }

    dfs(current, cleanPrefix, results, maxWords, maxLength);
    return results;
}

bool TrieTree::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::string currentSpelling = "";
    std::string currentIpa = "";
    std::string currentEn = "";
    std::string currentVi = "";
    std::string currentCategory = "";

    while (std::getline(file, line)) {
        if (line.find('{') == std::string::npos && line.find('}') == std::string::npos && line.find(':') == std::string::npos) {
            continue;
        }

        size_t pos = 0;
        std::string first = extractString(line, pos);
        if (first.empty()) {
            // Kiểm tra xem có kết thúc một từ không
            if (line.find('}') != std::string::npos && !currentSpelling.empty()) {
                Word newWord(currentSpelling, currentIpa, currentEn, currentVi, currentCategory);
                this->insert(newWord);
                currentSpelling = "";
                currentIpa = "";
                currentEn = "";
                currentVi = "";
                currentCategory = "";
            }
            continue;
        }

        size_t colon = line.find(':', pos - 1);
        if (colon != std::string::npos) {
            // Kiểm tra xem có bắt đầu cụm từ mới không
            if (line.find('{', colon) != std::string::npos) {
                currentSpelling = first;
            } else {
                // Đọc các trường thuộc tính
                std::string value = extractString(line, pos);
                if (first == "vi") currentVi = value;
                else if (first == "en") currentEn = value;
                else if (first == "ipa") currentIpa = value;
                else if (first == "category") currentCategory = value;
            }
        }
    }
    file.close();
    return true;
}

int TrieTree::getWordCount() const {
    return wordCount;
}

const std::vector<Word>& TrieTree::getWordList() const {
    return wordList;
}
