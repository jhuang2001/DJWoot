#ifndef DJWOOT_BOID_H
#define DJWOOT_BOID_H

#include "vector2.h"
#include <vector>

namespace boid {

class Boid {
public:
    Boid(float startX, float startY, float startVX = 0.0f, float startVY = 0.0f, int type = 0, bool isDebug = false);

    void update();
    void applyForce(const Vector2& force);
    void updateFromFlock(const std::vector<Boid*>& boids);
    void avoidEdges(float width, float height);
    bool isDebug() const { return debug; }
    int getType() const { return boidType; }

    Vector2 getSeparationForce() const { return separationForce; }
    Vector2 getAlignmentForce() const { return alignmentForce; }
    Vector2 getCohesionForce() const { return cohesionForce; }
    Vector2 getEdgeForce() const { return edgeForce; }

    Vector2 position;
    Vector2 velocity;

private:
    Vector2 acceleration;
    float maxSpeed;
    float maxForce;
    float perceptionRadius;
    bool debug;

    Vector2 separationForce = {0.0f, 0.0f}, alignmentForce = {0.0f, 0.0f}, cohesionForce = {0.0f, 0.0f}, edgeForce = {0.0f, 0.0f};
    int boidType = 0;

    Vector2 seek(const Vector2& target) const;
    Vector2 separate(const std::vector<Boid*>& neighbors) const;
    Vector2 align(const std::vector<Boid*>& neighbors) const;
    Vector2 cohesion(const std::vector<Boid*>& neighbors) const;
};

} // namespace boid

#endif // DJWOOT_BOID_H
