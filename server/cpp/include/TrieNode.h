#ifndef TRIENODE_H
#define TRIENODE_H

#include "Word.h"
#include <map>

class TrieNode {
public:
    std::map<char, TrieNode*> children;
    bool isEndOfWord;
    Word wordData;

    TrieNode();
    ~TrieNode(); // Hủy đệ quy giải phóng toàn bộ cây
};

#endif // TRIENODE_H
