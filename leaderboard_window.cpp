#include "leaderboard_window.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <optional>

void showLeaderboard() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(400, 400)), "Leaderboard", sf::Style::Titlebar | sf::Style::Close);

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

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    std::ostringstream oss;
    for (size_t i = 0; i < lines.size(); i++) {
        std::string entry = lines[i];
        bool isNew = false;
        if (!entry.empty() && entry.back() == '*') {
            isNew = true;
            entry.pop_back();
        }
        oss << (i + 1) << ". " << entry << (isNew ? " *" : "") << "\n\n";
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
