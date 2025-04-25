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
          const sf::Texture &mineTexture,
          const sf::Font &font);

    void initialize();
    bool revealTileAt(const sf::Vector2i &pixelPos, int topOffset);
    void toggleFlagAt(const sf::Vector2i &pixelPos, int topOffset);
    void draw(sf::RenderWindow &window) const;
    int getRemainingMines() const;
    int getTotalRevealed() const;
    const Tile& getTile(int row, int col) const;
    Tile& getTileRef(int row, int col);
    void reset();

    void temporarilyRevealAll();
    void restoreTileVisuals();
    void revealAllMines(); // <- Added

private:
    int rows, columns, mineCount;
    int tileSize;
    std::vector<std::vector<Tile>> grid;

    const sf::Texture &hiddenTexture;
    const sf::Texture &revealedTexture;
    const sf::Texture &flagTexture;
    const sf::Texture &mineTexture;
    const sf::Font &font;

    void placeMines();
    void calculateAdjacentNumbers();
    void revealRecursive(int row, int col);
};

#endif // BOARD_HPP
