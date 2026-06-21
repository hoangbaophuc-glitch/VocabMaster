#ifndef BOTAI_H
#define BOTAI_H

#include "Word.h"
#include "TrieTree.h"
#include <string>
#include <vector>

class BotAI {
private:
    static void shuffleWords(std::vector<Word>& words);

public:
    // AI Bot chọn từ dựa trên chữ cái cuối và độ khó. Trả về đối tượng Word trống nếu bí từ.
    static Word chooseWord(char lastChar, const std::vector<std::string>& usedWords, 
                            const std::string& difficulty, const TrieTree& dictionary);
};

#endif // BOTAI_H
