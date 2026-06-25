#ifndef GAMEMODE_H
#define GAMEMODE_H

#include "Player.h"
#include "TrieTree.h"
#include <string>

class GameMode {
protected:
    const TrieTree& dictionary;
    int score;
    int timeLimit;
    int maxAttempts;
    int wrongCount;
    bool gameOver;
    std::string endGameMessage;

public:
    GameMode(const TrieTree& dict, int timeLimit, int maxAttempts) 
        : dictionary(dict), score(0), timeLimit(timeLimit), maxAttempts(maxAttempts), 
          wrongCount(0), gameOver(false), endGameMessage("") {}

    virtual ~GameMode() = default;

    // Khởi động ván chơi
    virtual void start() = 0;
    
    // Thực hiện một lượt đi. Trả về true nếu thành công, false nếu từ không hợp lệ hoặc trò chơi kết thúc.
    virtual bool playTurn(Player& player, const std::string& input, double timeTaken) = 0;
    
    // Hiển thị trạng thái hiện tại của trò chơi lên màn hình console
    virtual void displayState() const = 0;
    
    // Sử dụng vật phẩm gợi ý
    virtual void useHint(Player& player) = 0;
    
    // Sử dụng vật phẩm hồi sinh để tiếp tục ván đấu
    virtual void useRevive(Player& player) = 0;

    // Các Getters chung
    int getScore() const { return score; }
    bool isGameOver() const { return gameOver; }
    std::string getEndGameMessage() const { return endGameMessage; }
    int getTimeLimit() const { return timeLimit; }
    int getMaxAttempts() const { return maxAttempts; }
    int getWrongCount() const { return wrongCount; }
};

#endif // GAMEMODE_H
