// welcome_window.cpp
#include "welcome_window.hpp"
#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <string>
#include <cctype>

std::string getUsername() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(600, 400)), "Welcome to Minesweeper!");

    sf::Font font;
    if (!font.openFromFile("files/font.ttf")) {
        std::cerr << "Error loading font from files/font.ttf\n";
        return "";
    }

    sf::Text prompt(font, "Enter your name:", 24);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(sf::Vector2f(50.f, 50.f));

    sf::Text userInput(font, "", 24);
    userInput.setFillColor(sf::Color::Yellow);
    userInput.setPosition(sf::Vector2f(50.f, 100.f));

    std::string username;

    while (window.isOpen()) {
        std::optional<sf::Event> event = window.pollEvent();
        while (event.has_value()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::TextEntered>()) {
                const auto* textEvent = event->getIf<sf::Event::TextEntered>();
                if (textEvent) {
                    // Backspace (Unicode 8)
                    if (textEvent->unicode == 8) {
                        if (!username.empty())
                            username.pop_back();
                    }
                    // Enter (Unicode 13): if input exists, close window.
                    else if (textEvent->unicode == 13) {
                        if (!username.empty())
                            window.close();
                    }
                    // Accept only alphabetical characters (limit 10).
                    else {
                        char ch = static_cast<char>(textEvent->unicode);
                        if (username.size() < 10 && std::isalpha(ch))
                            username.push_back(ch);
                    }
                    userInput.setString(username + "|");
                }
            }
            event = window.pollEvent();
        }
        window.clear();
        window.draw(prompt);
        window.draw(userInput);
        window.display();
    }
    return username;
}
