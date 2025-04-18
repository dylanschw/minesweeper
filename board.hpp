#ifndef BOARD_HPP
#define BOARD_HPP

#include "tile.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class Board {
public:
    Board(int rows, int columns, int mineCount,
          const sf::Texture &hiddenTexture,
          const sf::Texture &revealedTexture,
          const sf::Texture &flagTexture,
          const sf::Font &font);

    // Initializes the board (resets tiles, places mines, calculates numbers).
    void initialize();

    // Reveals the tile at the given pixel position (with top offset for UI).
    // Returns true if a mine is revealed (i.e. game over).
    bool revealTileAt(const sf::Vector2i &pixelPos, int topOffset);

    // Toggles a flag at the given pixel position.
    void toggleFlagAt(const sf::Vector2i &pixelPos, int topOffset);

    // Draws the board (tiles and numbers) on the provided window.
    void draw(sf::RenderWindow &window) const;

    // Returns the number of mines remaining (mineCount minus flagged tiles).
    int getRemainingMines() const;

    // Returns the total number of revealed tiles on the board.
    int getTotalRevealed() const;

    // Returns the tile at the given board coordinates (row, col).
    const Tile& getTile(int row, int col) const;
    Tile& getTileRef(int row, int col);

    // Resets the board (reinitializes all tile states, places mines, etc.).
    void reset();

private:
    int rows, columns, mineCount;
    int tileSize; // Constant tile size (e.g., 32 pixels)
    std::vector<std::vector<Tile>> grid;

    const sf::Texture &hiddenTexture;
    const sf::Texture &revealedTexture;
    const sf::Texture &flagTexture;
    const sf::Font &font;

    // Helper methods.
    void placeMines();
    void calculateAdjacentNumbers();
    void revealRecursive(int row, int col);
};

#endif // BOARD_HPP
