#include "vector2.h"

#include <cmath>

float Vector2::length() const {
    return std::sqrt(x * x + y * y);
}

float Vector2::lengthSquared() const {
    return x * x + y * y;
}

Vector2 Vector2::normalized() const {
    const float len = length();
    return len > 0.0f ? Vector2{x / len, y / len} : Vector2{0.0f, 0.0f};
}

void Vector2::limit(float maxLength) {
    const float magSq = lengthSquared();
    if (magSq > maxLength * maxLength && magSq > 0.0f) {
        const float mag = std::sqrt(magSq);
        x = (x / mag) * maxLength;
        y = (y / mag) * maxLength;
    }
}

bool Vector2::isZero() const {
    return x == 0.0f && y == 0.0f;
}

float distance(const Vector2& a, const Vector2& b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

