#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "TrieTree.h"
#include "Player.h"
#include "GameMode.h"
#include "Store.h"
#include <string>

class GameController {
private:
    TrieTree dictionary;
    Player player;
    GameMode* currentGame;
    Store store;

    // Game settings
    std::string gameMode;      // "chain" (Nối từ), "scramble" (Xếp chữ), "fillblank" (Điền từ)
    std::string difficulty;    // "easy" (Dễ), "hard" (Khó)
    int timeLimit;             // 15, 10, 5 giây
    int maxAttempts;           // 1, 3, 5, 9999 (Vô hạn)

    void showHeader() const;
    void showMainMenu();
    void setSettingsMenu();
    void showFlashcardsMenu();
    void playGame();

public:
    GameController();
    ~GameController();

    void run();
};

#endif // GAMECONTROLLER_H
