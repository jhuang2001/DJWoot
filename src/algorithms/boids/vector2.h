#ifndef DJWOOT_VECTOR2_H
#define DJWOOT_VECTOR2_H

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;

    Vector2() = default;
    Vector2(float xValue, float yValue) : x(xValue), y(yValue) {}

    Vector2 operator+(const Vector2& rhs) const { return Vector2{x + rhs.x, y + rhs.y}; }
    Vector2 operator-(const Vector2& rhs) const { return Vector2{x - rhs.x, y - rhs.y}; }
    Vector2 operator*(float scalar) const { return Vector2{x * scalar, y * scalar}; }
    Vector2 operator/(float scalar) const { return Vector2{x / scalar, y / scalar}; }

    Vector2& operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
    Vector2& operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }
    
    bool operator<(const Vector2& rhs) const { return ((x < rhs.x) || (x == rhs.x && y < rhs.y)); }

    float length() const;
    float lengthSquared() const;
    Vector2 normalized() const;
    void limit(float maxLength);
    bool isZero() const;
};

float distance(const Vector2& a, const Vector2& b);

#endif // DJWOOT_VECTOR2_H
