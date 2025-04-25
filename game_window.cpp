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

void drawDigits(sf::RenderWindow &window, const sf::Texture &texture, int value, sf::Vector2f position) {
    const int digitWidth = 21;
    const int digitHeight = 32;
    std::string text = std::to_string(value);
    for (char ch : text) {
        int index = (ch == '-') ? 10 : ch - '0';
        sf::IntRect rect(sf::Vector2i(index * digitWidth, 0), sf::Vector2i(digitWidth, digitHeight));
        sf::Sprite digitSprite(texture, rect);
        digitSprite.setPosition(position);
        window.draw(digitSprite);
        position.x += digitWidth;
    }
}

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
    bool success = true;
    success &= hiddenTexture.loadFromFile("files/images/tile_hidden.png");
    success &= revealedTexture.loadFromFile("files/images/tile_revealed.png");
    success &= flagTexture.loadFromFile("files/images/flag.png");
    success &= mineTexture.loadFromFile("files/images/mine.png");

    sf::Texture faceHappyTexture, faceWinTexture, faceLoseTexture, debugTexture, leaderboardButtonTexture, pauseTexture, playTexture;
    success &= faceHappyTexture.loadFromFile("files/images/face_happy.png");
    success &= faceWinTexture.loadFromFile("files/images/face_win.png");
    success &= faceLoseTexture.loadFromFile("files/images/face_lose.png");
    success &= debugTexture.loadFromFile("files/images/debug.png");
    success &= leaderboardButtonTexture.loadFromFile("files/images/leaderboard.png");
    success &= pauseTexture.loadFromFile("files/images/pause.png");
    success &= playTexture.loadFromFile("files/images/play.png");

    sf::Texture digitTexture;
    success &= digitTexture.loadFromFile("files/images/digits.png");

    sf::Font font;
    success &= font.openFromFile("files/font.ttf");

    if (!success) {
        std::cerr << "Failed to load one or more resources.\n";
        return;
    }

    Board board(rows, columns, mineCount, hiddenTexture, revealedTexture, flagTexture, mineTexture, font);
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

    sf::Clock gameClock;
    sf::Time pausedTime = sf::Time::Zero;
    bool paused = false;
    bool gameOver = false;
    bool gameWon = false;
    bool debugMode = false;
    bool leaderboardOpen = false;

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
                        leaderboardOpen = false;
                        pausedTime = sf::Time::Zero;
                        gameClock.restart();
                        faceButton.setTexture(faceHappyTexture);
                    } else if (!paused && !gameWon &&
                               mouseEvent->button == sf::Mouse::Button::Left &&
                               debugButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        debugMode = !debugMode;
                    } else if (mouseEvent->button == sf::Mouse::Button::Left &&
                               leaderboardButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        leaderboardOpen = true;
                        paused = true;
                        pauseButton.setTexture(playTexture);
                        pausedTime += gameClock.getElapsedTime();
                        board.temporarilyRevealAll();
                        debugMode = false;
                        window.clear();
                        board.draw(window);
                        window.draw(faceButton);
                        window.draw(debugButton);
                        window.draw(leaderboardButton);
                        window.draw(pauseButton);
                        window.draw(mineCounterText);
                        float digitYOffset = (pauseButton.getGlobalBounds().size.y - 32.f) / 2.f;
                        drawDigits(window, digitTexture, static_cast<int>((pausedTime).asSeconds()),
                                   sf::Vector2f(pauseButton.getPosition().x - 70.f, pauseButton.getPosition().y + digitYOffset));
                        window.display();

                        showLeaderboard(username); //  Pass username to display asterisk
                        board.restoreTileVisuals();
                        leaderboardOpen = false;
                        paused = false;
                        pauseButton.setTexture(pauseTexture);
                        gameClock.restart();
                    } else if (!gameOver && !gameWon &&
                               mouseEvent->button == sf::Mouse::Button::Left &&
                               pauseButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        paused = !paused;
                        pauseButton.setTexture(paused ? playTexture : pauseTexture);
                        if (paused) {
                            pausedTime += gameClock.getElapsedTime();
                            board.temporarilyRevealAll();
                        } else {
                            gameClock.restart();
                            board.restoreTileVisuals();
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
        if (paused || gameOver || gameWon) {
            elapsed = pausedTime;
        } else {
            elapsed = pausedTime + gameClock.getElapsedTime();
        }

        int totalTiles = rows * columns;
        int revealedCount = board.getTotalRevealed();
        if (!gameOver && !gameWon && revealedCount == totalTiles - mineCount) {
            gameWon = true;
            faceButton.setTexture(faceWinTexture);
            int finalTime = static_cast<int>(elapsed.asSeconds());
            updateLeaderboard(username, finalTime);
            pausedTime = elapsed;

            //  Flag all unflagged mines
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < columns; c++) {
                    Tile& tile = board.getTileRef(r, c);
                    if (tile.isMine() && !tile.isFlagged()) {
                        tile.toggleFlag();
                        tile.setSpriteTexture(flagTexture);
                    }
                }
            }
        }

        mineCounterText.setString("Mines: " + std::to_string(board.getRemainingMines()));

        window.clear();
        board.draw(window);

        if ((debugMode && !paused && !leaderboardOpen)) {
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

        if (gameOver) {
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < columns; c++) {
                    const Tile& tile = board.getTile(r, c);
                    if (tile.isMine()) {
                        sf::Sprite mineSprite(mineTexture);
                        mineSprite.setPosition(tile.getSprite().getPosition());
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

        float digitYOffset = (pauseButton.getGlobalBounds().size.y - 32.f) / 2.f;
        drawDigits(window, digitTexture, static_cast<int>(elapsed.asSeconds()),
                   sf::Vector2f(pauseButton.getPosition().x - 70.f, pauseButton.getPosition().y + digitYOffset));

        window.display();
    }
}

void updateLeaderboard(const std::string& username, int finalTime) {
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

    entries.emplace_back(username, finalTime);  //  no asterisk written
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    if (entries.size() > 5)
        entries.resize(5);

    std::ofstream fileOut("files/leaderboard.txt", std::ios::trunc);
    for (const auto& [name, time] : entries) {
        fileOut << name << " " << time << "\n";  // write clean name + time
    }
    fileOut.close();
}
