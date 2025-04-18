#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>

class Tile {
public:
    // Constructor: Creates a tile using the given hidden texture.
    Tile(const sf::Texture &hiddenTexture);

    // Getters.
    bool isRevealed() const;
    bool isFlagged() const;
    bool isMine() const;
    int getAdjacentMineCount() const;

    // Setters.
    void setMine(bool value);
    void setAdjacentMineCount(int count);

    // Actions.
    void reveal();
    void toggleFlag();

    // Set the sprite's texture.
    void setSpriteTexture(const sf::Texture &texture);

    // Set the position using an sf::Vector2f.
    void setPosition(const sf::Vector2f &pos);

    // Get the sprite for drawing.
    const sf::Sprite& getSprite() const;

    // Reset tile state.
    void reset(const sf::Texture &hiddenTexture);

private:
    bool revealed;
    bool flagged;
    bool mine;
    int adjacentMineCount;
    sf::Sprite sprite;
};

#endif // TILE_HPP
