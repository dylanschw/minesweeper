#include "tile.hpp"

Tile::Tile(const sf::Texture &hiddenTexture)
    : revealed(false), flagged(false), mine(false), adjacentMineCount(0), sprite(hiddenTexture)
{
}

bool Tile::isRevealed() const { return revealed; }
bool Tile::isFlagged() const { return flagged; }
bool Tile::isMine() const { return mine; }
int Tile::getAdjacentMineCount() const { return adjacentMineCount; }

void Tile::setMine(bool value) { mine = value; }
void Tile::setAdjacentMineCount(int count) { adjacentMineCount = count; }

void Tile::reveal() { revealed = true; flagged = false; }
void Tile::toggleFlag() { if (!revealed) flagged = !flagged; }

void Tile::setSpriteTexture(const sf::Texture &texture) {
    sprite.setTexture(texture);
}

void Tile::setPosition(const sf::Vector2f &pos) {
    sprite.setPosition(pos);
}

const sf::Sprite& Tile::getSprite() const {
    return sprite;
}

void Tile::reset(const sf::Texture &hiddenTexture) {
    revealed = false;
    flagged = false;
    mine = false;
    adjacentMineCount = 0;
    sprite.setTexture(hiddenTexture);
}
