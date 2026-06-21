#ifndef STORE_H
#define STORE_H

#include "Player.h"

class Store {
public:
    void showStoreMenu(Player& player);
    bool buyItem(Player& player, int itemId);
};

#endif // STORE_H
