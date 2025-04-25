#include "welcome_window.hpp"
#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <string>
#include <cctype>
#include <fstream>

std::string toProperCase(const std::string& input) {
    std::string result;
    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];
        if (i == 0)
            result += std::toupper(ch);
        else
            result += std::tolower(ch);
    }
    return result;
}

std::string getUsername() {
    int welcomeWidth = 600, welcomeHeight = 400;
    std::ifstream config("files/config.cfg");
    if (config.is_open()) {
        int skip1, skip2, skip3;
        config >> skip1 >> skip2 >> skip3 >> welcomeWidth >> welcomeHeight;
        config.close();
    }

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(welcomeWidth, welcomeHeight)), "Welcome to Minesweeper!");

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

    std::string username;

    while (window.isOpen()) {
        std::optional<sf::Event> event = window.pollEvent();
        while (event.has_value()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::TextEntered>()) {
                const auto* textEvent = event->getIf<sf::Event::TextEntered>();
                if (textEvent) {
                    if (textEvent->unicode == 8) {
                        if (!username.empty())
                            username.pop_back();
                    } else if (textEvent->unicode == 13) {
                        if (!username.empty())
                            window.close();
                    } else {
                        char ch = static_cast<char>(textEvent->unicode);
                        if (username.size() < 10 && std::isalpha(ch))
                            username.push_back(ch);
                    }

                    std::string formatted = toProperCase(username) + "|";
                    userInput.setString(formatted);

                    //  Dynamic horizontal centering
                    sf::Rect<float> bounds = userInput.getLocalBounds();
float textWidth = bounds.size.x;  // SFML 3.0 uses `size.x` for width
userInput.setPosition(sf::Vector2f((window.getSize().x - textWidth) / 2.f, 100.f));


                }
            }

            event = window.pollEvent();
        }

        window.clear();
        window.draw(prompt);
        window.draw(userInput);
        window.display();
    }

    return toProperCase(username);
}
