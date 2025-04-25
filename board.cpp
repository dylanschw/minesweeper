#include "board.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

Board::Board(int rows, int columns, int mineCount,
             const sf::Texture &hiddenTexture,
             const sf::Texture &revealedTexture,
             const sf::Texture &flagTexture,
             const sf::Texture &mineTexture,
             const sf::Font &font)
    : rows(rows), columns(columns), mineCount(mineCount), tileSize(32),
      hiddenTexture(hiddenTexture), revealedTexture(revealedTexture),
      flagTexture(flagTexture), mineTexture(mineTexture), font(font)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    grid.resize(rows);
    for (int r = 0; r < rows; r++) {
        grid[r].reserve(columns);
        for (int c = 0; c < columns; c++) {
            grid[r].push_back(Tile(hiddenTexture));
            grid[r].back().setPosition(sf::Vector2f(static_cast<float>(c * tileSize),
                                                    static_cast<float>(r * tileSize + 100)));
        }
    }
}

void Board::initialize() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            grid[r][c].reset(hiddenTexture);
        }
    }
    placeMines();
    calculateAdjacentNumbers();
}

void Board::placeMines() {
    int placedMines = 0;
    while (placedMines < mineCount) {
        int r = std::rand() % rows;
        int c = std::rand() % columns;
        if (!grid[r][c].isMine()) {
            grid[r][c].setMine(true);
            placedMines++;
        }
    }
}

void Board::calculateAdjacentNumbers() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            if (grid[r][c].isMine())
                continue;
            int count = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < columns) {
                        if (grid[nr][nc].isMine())
                            count++;
                    }
                }
            }
            grid[r][c].setAdjacentMineCount(count);
        }
    }
}

bool Board::revealTileAt(const sf::Vector2i &pixelPos, int topOffset) {
    int boardX = pixelPos.x;
    int boardY = pixelPos.y - topOffset;
    if (boardY < 0) return false;
    int col = boardX / tileSize;
    int row = boardY / tileSize;
    if (row < 0 || row >= rows || col < 0 || col >= columns)
        return false;
    if (grid[row][col].isRevealed() || grid[row][col].isFlagged())
        return false;

    grid[row][col].reveal();
    grid[row][col].setSpriteTexture(revealedTexture);

    if (!grid[row][col].isMine() && grid[row][col].getAdjacentMineCount() == 0) {
        revealRecursive(row, col);
    }
    return grid[row][col].isMine();
}

void Board::revealRecursive(int row, int col) {
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            int nr = row + dr;
            int nc = col + dc;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < columns) {
                if (!grid[nr][nc].isRevealed() && !grid[nr][nc].isFlagged()) {
                    grid[nr][nc].reveal();
                    grid[nr][nc].setSpriteTexture(revealedTexture);
                    if (grid[nr][nc].getAdjacentMineCount() == 0 && !grid[nr][nc].isMine()) {
                        revealRecursive(nr, nc);
                    }
                }
            }
        }
    }
}

void Board::toggleFlagAt(const sf::Vector2i &pixelPos, int topOffset) {
    int boardX = pixelPos.x;
    int boardY = pixelPos.y - topOffset;
    if (boardY < 0) return;
    int col = boardX / tileSize;
    int row = boardY / tileSize;
    if (row < 0 || row >= rows || col < 0 || col >= columns)
        return;
    if (grid[row][col].isRevealed())
        return;
    grid[row][col].toggleFlag();
    if (grid[row][col].isFlagged())
        grid[row][col].setSpriteTexture(flagTexture);
    else
        grid[row][col].setSpriteTexture(hiddenTexture);
}

void Board::draw(sf::RenderWindow &window) const {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            const Tile& tile = grid[r][c];
            if (tile.isFlagged()) {
    // Draw the revealed tile background first
    sf::Sprite baseSprite(revealedTexture);
    baseSprite.setPosition(tile.getSprite().getPosition());
    window.draw(baseSprite);

    // Draw the flag on top
    sf::Sprite flagSprite(flagTexture);
    flagSprite.setPosition(tile.getSprite().getPosition());
    window.draw(flagSprite);
} else {
    // Otherwise just draw the tile’s texture
    window.draw(tile.getSprite());
}


            if (tile.isRevealed() && !tile.isMine() && tile.getAdjacentMineCount() > 0) {
                sf::Text number(font, std::to_string(tile.getAdjacentMineCount()), 16);
                number.setFillColor(sf::Color::Blue);
                sf::Vector2f pos = tile.getSprite().getPosition();
                number.setPosition(sf::Vector2f(pos.x + tileSize / 2.f - 5.f, pos.y + tileSize / 2.f - 10.f));
                window.draw(number);

            }
        }
    }
}

int Board::getRemainingMines() const {
    int flaggedCount = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            if (grid[r][c].isFlagged())
                flaggedCount++;
        }
    }
    return mineCount - flaggedCount;
}

int Board::getTotalRevealed() const {
    int count = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            if (grid[r][c].isRevealed())
                count++;
        }
    }
    return count;
}

const Tile& Board::getTile(int row, int col) const {
    return grid[row][col];
}

Tile& Board::getTileRef(int row, int col) {
    return grid[row][col];
}

void Board::reset() {
    initialize();
}

void Board::temporarilyRevealAll() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            grid[r][c].setSpriteTexture(revealedTexture);
        }
    }
}

void Board::restoreTileVisuals() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            Tile &tile = grid[r][c];
            if (tile.isRevealed()) {
                tile.setSpriteTexture(revealedTexture);
            } else if (tile.isFlagged()) {
                tile.setSpriteTexture(flagTexture);
            } else {
                tile.setSpriteTexture(hiddenTexture);
            }
        }
    }
}

void Board::revealAllMines() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            Tile& tile = grid[r][c];
            if (tile.isMine()) {
		tile.reveal();
                tile.setSpriteTexture(mineTexture);
            }
        }
    }
}
