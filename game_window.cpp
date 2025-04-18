#include "game_window.hpp"
#include "board.hpp"
#include "leaderboard_window.hpp"
#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <SFML/System.hpp>

void updateLeaderboard(const std::string& username, int finalTime);

void runGameWindow(const std::string &username) {
    int columns = 0, rows = 0, mineCount = 0;
    std::ifstream configFile("files/config.cfg");
    if (!configFile) {
        std::cerr << "Error opening config file.\n";
        return;
    }
    configFile >> columns >> rows >> mineCount;
    configFile.close();
    
    const int tileSize = 32;
    int windowWidth = columns * tileSize;
    int windowHeight = rows * tileSize + 100;

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(windowWidth, windowHeight)),
                            "Minesweeper - " + username,
                            sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    sf::Texture hiddenTexture, revealedTexture, flagTexture, mineTexture;
    hiddenTexture.loadFromFile("files/images/tile_hidden.png");
    revealedTexture.loadFromFile("files/images/tile_revealed.png");
    flagTexture.loadFromFile("files/images/flag.png");
    mineTexture.loadFromFile("files/images/mine.png");

    sf::Texture faceHappyTexture, faceWinTexture, faceLoseTexture, debugTexture, leaderboardButtonTexture, pauseTexture, playTexture;
    faceHappyTexture.loadFromFile("files/images/face_happy.png");
    faceWinTexture.loadFromFile("files/images/face_win.png");
    faceLoseTexture.loadFromFile("files/images/face_lose.png");
    debugTexture.loadFromFile("files/images/debug.png");
    leaderboardButtonTexture.loadFromFile("files/images/leaderboard.png");
    pauseTexture.loadFromFile("files/images/pause.png");
    playTexture.loadFromFile("files/images/play.png");

    sf::Font font;
    font.openFromFile("files/font.ttf");

    Board board(rows, columns, mineCount, hiddenTexture, revealedTexture, flagTexture, font);
    board.initialize();

    sf::Sprite faceButton(faceHappyTexture);
    sf::Sprite debugButton(debugTexture);
    sf::Sprite leaderboardButton(leaderboardButtonTexture);
    sf::Sprite pauseButton(pauseTexture);

    float uiY = 10.f;
    faceButton.setPosition(sf::Vector2f(windowWidth/2.f - faceHappyTexture.getSize().x/2.f, uiY));
    debugButton.setPosition(sf::Vector2f(10.f, uiY));
    leaderboardButton.setPosition(sf::Vector2f(windowWidth - leaderboardButtonTexture.getSize().x - 10.f, uiY));
    pauseButton.setPosition(sf::Vector2f(windowWidth - leaderboardButtonTexture.getSize().x - pauseTexture.getSize().x - 20.f, uiY));

    sf::Text mineCounterText(font, "", 20);
    mineCounterText.setFillColor(sf::Color::White);
    mineCounterText.setPosition(sf::Vector2f(10.f, 70.f));

    sf::Text timerText(font, "0", 20);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(sf::Vector2f(windowWidth - 50.f, 70.f));

    sf::Clock gameClock;
    sf::Time pausedTime = sf::Time::Zero;
    bool paused = false;
    bool gameOver = false;
    bool gameWon = false;
    bool debugMode = false;

    while (window.isOpen()) {
        std::optional<sf::Event> event = window.pollEvent();
        while (event.has_value()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto *mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (mouseEvent->button == sf::Mouse::Button::Left &&
                        faceButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        board.initialize();
                        gameOver = false;
                        gameWon = false;
                        paused = false;
                        pausedTime = sf::Time::Zero;
                        gameClock.restart();
                        faceButton.setTexture(faceHappyTexture);
                    } else if (mouseEvent->button == sf::Mouse::Button::Left &&
                               debugButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        debugMode = !debugMode;
                    } else if (mouseEvent->button == sf::Mouse::Button::Left &&
                               leaderboardButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        showLeaderboard();
                    } else if (mouseEvent->button == sf::Mouse::Button::Left &&
                               pauseButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        paused = !paused;
                        pauseButton.setTexture(paused ? playTexture : pauseTexture);
                        if (!paused) {
                            gameClock.restart();
                        }
                    } else if (mousePos.y > 100 && !gameOver && !gameWon && !paused) {
                        if (mouseEvent->button == sf::Mouse::Button::Left) {
                            bool hitMine = board.revealTileAt(mousePos, 100);
                            if (hitMine) {
                                gameOver = true;
                                faceButton.setTexture(faceLoseTexture);
                                pausedTime = paused ? pausedTime : pausedTime + gameClock.getElapsedTime();
                            }
                        } else if (mouseEvent->button == sf::Mouse::Button::Right) {
                            board.toggleFlagAt(mousePos, 100);
                        }
                    }
                }
            }
            event = window.pollEvent();
        }

        sf::Time elapsed;
        if (gameOver || gameWon) {
            elapsed = pausedTime;
        } else if (paused) {
            elapsed = pausedTime;
        } else {
            elapsed = pausedTime + gameClock.getElapsedTime();
        }

        timerText.setString(std::to_string(static_cast<int>(elapsed.asSeconds())));
        mineCounterText.setString("Mines: " + std::to_string(board.getRemainingMines()));

        int totalTiles = rows * columns;
        int revealedCount = board.getTotalRevealed();
        if (!gameOver && !gameWon && revealedCount == totalTiles - mineCount) {
            gameWon = true;
            faceButton.setTexture(faceWinTexture);
            int finalTime = static_cast<int>(elapsed.asSeconds());
	    std::cout << "WIN DETECTED, final time: " << finalTime << std::endl;
            updateLeaderboard(username, finalTime);
            pausedTime = elapsed;
        }

        window.clear();
        board.draw(window);

        if (debugMode) {
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < columns; c++) {
                    if (board.getTile(r, c).isMine() && !board.getTile(r, c).isRevealed()) {
                        sf::Sprite mineSprite(mineTexture);
                        mineSprite.setPosition(board.getTile(r, c).getSprite().getPosition());
                        window.draw(mineSprite);
                    }
                }
            }
        }

        window.draw(faceButton);
        window.draw(debugButton);
        window.draw(leaderboardButton);
        window.draw(pauseButton);
        window.draw(mineCounterText);
        window.draw(timerText);

        window.display();
    }
}

void updateLeaderboard(const std::string& username, int finalTime) {
    std::cout << "Updating leaderboard with: " << username << " " << finalTime << std::endl;
    std::ifstream fileIn("files/leaderboard.txt");
    std::vector<std::pair<std::string, int>> entries;
    std::string line;
    while (std::getline(fileIn, line)) {
        std::istringstream iss(line);
        std::string name;
        int time;
        iss >> name >> time;
        entries.emplace_back(name, time);
    }
    fileIn.close();

    entries.emplace_back(username + "*", finalTime);
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    if (entries.size() > 5)
        entries.resize(5);

    std::cout << "Final leaderboard:\n";
    for (const auto& [name, time] : entries) {
        std::cout << name << " " << time << "\n";
    }

    std::ofstream fileOut("files/leaderboard.txt", std::ios::trunc);
    for (const auto& [name, time] : entries) {
        fileOut << name << " " << time << "\n";
    }
    fileOut.close();
    std::cout << "Leaderboard file updated.\n";
}