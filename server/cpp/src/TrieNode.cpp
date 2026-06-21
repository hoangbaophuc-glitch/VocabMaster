#include "TrieNode.h"

TrieNode::TrieNode() : isEndOfWord(false) {}

TrieNode::~TrieNode() {
    for (auto& pair : children) {
        delete pair.second; // Giải phóng đệ quy vùng nhớ của nút con
    }
}
