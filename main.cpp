// main.cpp
#include "welcome_window.hpp"
#include "game_window.hpp"
#include <iostream>
#include <string>

int main() {
    std::string username = getUsername();
    std::cout << "Username entered: " << username << "\n";
    if (!username.empty())
        runGameWindow(username);
    else
        std::cout << "No username entered; exiting.\n";
    return 0;
}
