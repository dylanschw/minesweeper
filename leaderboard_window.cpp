#include "leaderboard_window.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <optional>

void showLeaderboard(const std::string& currentPlayer) {
    int leaderboardWidth = 400, leaderboardHeight = 400;
    std::ifstream config("files/config.cfg");
    if (config.is_open()) {
        int skip1, skip2, skip3, skip4, skip5;
        config >> skip1 >> skip2 >> skip3 >> skip4 >> skip5 >> leaderboardWidth >> leaderboardHeight;
        config.close();
    }

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(leaderboardWidth, leaderboardHeight)), "Leaderboard", sf::Style::Titlebar | sf::Style::Close);

    sf::Font font;
    if (!font.openFromFile("files/font.ttf")) {
        std::cerr << "Error loading font from files/font.ttf\n";
        return;
    }

    std::ifstream file("files/leaderboard.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening files/leaderboard.txt\n";
        return;
    }

    std::vector<std::pair<std::string, int>> entries;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name;
        int time;
        iss >> name >> time;
        entries.emplace_back(name, time);
    }
    file.close();

    std::ostringstream oss;
    for (size_t i = 0; i < entries.size(); i++) {
        const std::string& name = entries[i].first;
        int time = entries[i].second;
        bool isNew = (name == currentPlayer);
        oss << (i + 1) << ". " << name << " " << time << (isNew ? " *" : "") << "\n\n";
    }

    sf::Text leaderboardText(font, oss.str(), 20);
    leaderboardText.setFillColor(sf::Color::White);
    leaderboardText.setPosition(sf::Vector2f(20.f, 20.f));

    while (window.isOpen()) {
        std::optional<sf::Event> event = window.pollEvent();
        while (event.has_value()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            event = window.pollEvent();
        }

        window.clear();
        window.draw(leaderboardText);
        window.display();
    }
}
