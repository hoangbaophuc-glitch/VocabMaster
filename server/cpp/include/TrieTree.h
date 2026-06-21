#ifndef TRIETREE_H
#define TRIETREE_H

#include "TrieNode.h"
#include "Word.h"
#include <string>
#include <vector>

class TrieTree {
private:
    TrieNode* root;
    int wordCount;
    std::vector<Word> wordList;

    // Helper functions
    void dfs(TrieNode* node, const std::string& currentWord, std::vector<Word>& results, int maxWords, int maxLength) const;
    std::string toLower(const std::string& s) const;
    static std::string extractString(const std::string& str, size_t& startPos);

public:
    TrieTree();
    ~TrieTree();

    void insert(const Word& word);
    bool isValidWord(const std::string& word) const;
    Word getWordDetails(const std::string& word) const;
    Word getRandomWord(const std::vector<std::string>& usedWords) const;
    std::vector<Word> findWordsStartingWith(const std::string& prefix, int maxWords = 50, int maxLength = 12) const;
    
    // Nạp từ điển từ file JSON dùng bộ phân tích tùy biến (không cần thư viện ngoài)
    bool loadFromFile(const std::string& filePath);

    int getWordCount() const;
    const std::vector<Word>& getWordList() const;
};

#endif // TRIETREE_H
