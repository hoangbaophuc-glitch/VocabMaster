#ifndef FILLBLANKGAME_H
#define FILLBLANKGAME_H

#include "GameMode.h"
#include "Word.h"
#include <string>
#include <vector>

class FillBlankGame : public GameMode {
private:
    Word secretWord;
    std::string blankedWord;
    std::vector<std::string> solvedHistory;

    std::string toLower(const std::string& s) const;
    std::string getCurrentTimestamp() const;
    void generateNextRound();

public:
    FillBlankGame(const TrieTree& dict, int timeLimit, int maxAttempts);

    void start() override;
    bool playTurn(Player& player, const std::string& input, double timeTaken) override;
    void displayState() const override;
    void useHint(Player& player) override;
    void useRevive(Player& player) override;
};

#endif // FILLBLANKGAME_H
