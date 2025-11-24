// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <utility>

namespace ConquestForPlanetX
{

class Player {
public:
    using Position = std::pair<int,int>;

    Player() = default;
    Player(std::string name, char icon, Position pos)
    {
        name_ = name;
        icon_ = icon;
        pos_ = pos;
    }

    // Name accessors
    const std::string& getName() 
    {
        return name_; 
    }
    void setName(std::string n) 
    { 
        name_ = std::move(n); 
    }

    // Icon accessors
    char getIcon() 
    { 
        return icon_; 
    }
    void setIcon(char c) 
    { 
        icon_ = c; 
    }

    // Position accessors
    Position getPosition()
    { 
        return pos_; 
    }
    void setPosition(Position p)
    { 
        pos_ = p; 
    }

    // Movement
    void moveTo(int x, int y)
    { 
        pos_.first = x; pos_.second = y; 
    }
    void moveBy(int dx, int dy) 
    {
        pos_.first += dx; pos_.second += dy; 
    }

    // Comparison
    bool operator==(const Player& other)
    {
        return pos_ == other.pos_;
    }
    bool operator!=(const Player& other)
    { 
        return !(pos_ == other.pos_); 
    }

private:
    std::string name_ = "Player";
    char icon_ = 'P';
    Position pos_{0,0};
};

}

#endif 