#pragma once

enum class Color { Red, Green, Blue, Black, Size };
enum class GameState { Menu, Start, Close, Restart, GameOver, Victory, Highscore};
enum class shotTiming {Delayed, Instant};
enum class AttackType {Simple, Circular, Big, BeamAttack, CrossAttack, Size};
enum class Owner {Player, System, Enemy, Medic};

template <typename T>
T max(T firstElement, T secondElement) {
    if (firstElement > secondElement)
        return firstElement;
    return secondElement;
}

template <typename T>
T min(T firstElement, T secondElement) {
    if (firstElement < secondElement)
        return firstElement;
    return secondElement;
}

template <typename T>
T power(T firstElement, int exponent = 2) {
    T answer = 1;
    for (int i = 0; i < exponent; i++)
        answer *= firstElement;
    return answer;
}

template <typename T, typename B>
struct pair {
    T first;
    B second;
};

struct coordinates { 
    int y;
    int x;

    bool operator == (const coordinates& comparedCoords) const {
        if (this->y != comparedCoords.y || this->x != comparedCoords.x)
            return false;
        return true;
    }
};

struct coordinatesF {
    float y;
    float x;
};

static bool checkCollision(coordinates firstPosition, coordinates firstSize, coordinates secondPosition, coordinates secondSize) {
        
    if ((firstPosition.x + firstSize.x >= secondPosition.x) &&
        (secondPosition.x + secondSize.x >= firstPosition.x) &&
        (firstPosition.y + firstSize.y >= secondPosition.y) &&
        (secondPosition.y + secondSize.y >= firstPosition.y))
            return true;
    return false;
}
