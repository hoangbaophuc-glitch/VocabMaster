#ifndef WORDCHAINGAME_H
#define WORDCHAINGAME_H

#include "GameMode.h"
#include "Word.h"
#include <string>
#include <vector>

class WordChainGame : public GameMode {
private:
    std::vector<std::string> history;
    Word currentBotWord;
    std::string difficulty;

    std::string toLower(const std::string& s) const;
    std::string getCurrentTimestamp() const;

public:
    WordChainGame(const TrieTree& dict, int timeLimit, int maxAttempts, const std::string& difficulty);
    
    void start() override;
    bool playTurn(Player& player, const std::string& input, double timeTaken) override;
    void displayState() const override;
    void useHint(Player& player) override;
    void useRevive(Player& player) override;
};

#endif // WORDCHAINGAME_H
