#include "Store.h"
#include <iostream>

void Store::showStoreMenu(Player& player) {
    while (true) {
        std::cout << "\n=======================================\n";
        std::cout << "         CỬA HÀNG VẬT PHẨM 🛒\n";
        std::cout << "=======================================\n";
        std::cout << " Số dư V-Coins của bạn: 💰 " << player.getVCoins() << " V-Coins\n";
        std::cout << "---------------------------------------\n";
        std::cout << "1. 🔍 Kính lúp Gợi ý (50 V-Coins)  - Đang có: " << player.getHintCount() << "\n";
        std::cout << "   [AI gợi ý 1 từ hợp lệ khi bạn bí]\n";
        std::cout << "2. 👑 Avatar VIP     (200 V-Coins) - Trạng thái: " << (player.getIsVip() ? "Đã sở hữu" : "Chưa sở hữu") << "\n";
        std::cout << "   [Khung viền vàng nổi bật danh tiếng]\n";
        std::cout << "3. ❤️ Bùa Hồi Sinh   (500 V-Coins) - Đang có: " << player.getReviveCount() << "\n";
        std::cout << "   [Dùng để tiếp tục chơi khi thất bại]\n";
        std::cout << "4. ⬅ Trở về sảnh chính\n";
        std::cout << "=======================================\n";
        std::cout << "Vui lòng chọn vật phẩm muốn mua (1-4): ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "❌ Lựa chọn không hợp lệ. Vui lòng chọn lại!\n";
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (choice == 4) {
            break;
        }

        if (choice < 1 || choice > 3) {
            std::cout << "❌ Lựa chọn không hợp lệ. Vui lòng chọn từ 1-4!\n";
            continue;
        }

        buyItem(player, choice);
    }
}

bool Store::buyItem(Player& player, int itemId) {
    int price = 0;
    std::string itemName = "";

    if (itemId == 1) {
        price = 50;
        itemName = "Kính lúp Gợi ý";
    } else if (itemId == 2) {
        price = 200;
        itemName = "Avatar VIP";
        if (player.getIsVip()) {
            std::cout << "⚠️ Bạn đã sở hữu Avatar VIP rồi!\n";
            return false;
        }
    } else if (itemId == 3) {
        price = 500;
        itemName = "Bùa Hồi Sinh";
    }

    if (player.getVCoins() >= price) {
        player.setVCoins(player.getVCoins() - price);
        
        if (itemId == 1) {
            player.setHintCount(player.getHintCount() + 1);
        } else if (itemId == 2) {
            player.setIsVip(true);
        } else if (itemId == 3) {
            player.setReviveCount(player.getReviveCount() + 1);
        }

        std::cout << "🎉 Đã mua thành công: " << itemName << "!\n";
        player.saveToFile(); // Lưu lại ví tiền và túi đồ ngay lập tức
        return true;
    } else {
        std::cout << "❌ Bạn không đủ V-Coins! Bạn thiếu " << (price - player.getVCoins()) << " V-Coins nữa.\n";
        return false;
    }
}
