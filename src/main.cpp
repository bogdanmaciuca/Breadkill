#include <iostream>
#include <exception>
#include "game.h"

int main() {
    try {
        Game game;
        game.Run();
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what();
    }
}

