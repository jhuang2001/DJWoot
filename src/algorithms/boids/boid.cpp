#include "boid.h"

#include <cmath>
#include <random>
#include <iostream>

namespace boid {

Boid::Boid(float startX, float startY, float startVX, float startVY, int type, bool isDebug)
    : position(startX, startY),
      velocity(startVX, startVY),
      acceleration(0.0f, 0.0f),
      maxSpeed(25.0f),
      maxForce(0.3f),
      perceptionRadius(100.0f),
      debug(isDebug),
      boidType(type) {
    if (velocity.isZero()) {
        static std::mt19937 engine{std::random_device{}()};
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        velocity = Vector2{dist(engine), dist(engine)}.normalized();
    }
}

void Boid::update() {
    velocity += acceleration;
    velocity.limit(maxSpeed);
    position += velocity;
    acceleration = Vector2{0.0f, 0.0f};
}

void Boid::applyForce(const Vector2& force) {
    acceleration += force;
}

Vector2 Boid::seek(const Vector2& target) const {
    Vector2 desired = target - position;
    if (desired.isZero()) {
        return Vector2{0.0f, 0.0f};
    }
    desired = desired.normalized() * maxSpeed;
    Vector2 steer = desired - velocity;
    return steer;
}

Vector2 Boid::separate(const std::vector<Boid*>& neighbors) const {
    Vector2 steer{0.0f, 0.0f};
    float separationWeight = 20.0f;
    float minSeparationDist = 50.0f;
    for (const Boid* other : neighbors) {
        const float d = distance(position, other->position);
        if (d > 0.0f && d < minSeparationDist) {
            // Apply abnormally strong repulsion if too close
            if (d < 5.0f) {
                steer += (position - other->position).normalized() * 50.0f;
                continue;
            }
            Vector2 diff = position - other->position;
            diff = diff / d;
            steer += diff;
        }
    }
    if (!steer.isZero()) {
        steer = steer * separationWeight;
        steer -= velocity;
    }
    return steer;
}

Vector2 Boid::align(const std::vector<Boid*>& neighbors) const {
    Vector2 sum{0.0f, 0.0f};
    float alignmentWeight = 2.5f;
    int count = 0;
    for (const Boid* other : neighbors) {
        if (other->boidType == boidType) {
            sum += other->velocity;
            ++count;
        }
    }
    if (count == 0) {
        return Vector2{0.0f, 0.0f};
    }
    sum /= static_cast<float>(count);
    sum = sum.normalized() * maxSpeed;
    Vector2 steer = (sum - velocity) * alignmentWeight;
    return steer;
}

Vector2 Boid::cohesion(const std::vector<Boid*>& neighbors) const {
    Vector2 sum{0.0f, 0.0f};
    float cohesionWeight = 1.0f;
    int count = 0;
    for (const Boid* other : neighbors) {
        if (other->boidType == boidType) {
            sum += other->position;
            ++count;
        }
    }
    if (count == 0) {
        return Vector2{0.0f, 0.0f};
    }
    sum /= static_cast<float>(count);
    return seek(sum) * cohesionWeight;
}

void Boid::updateFromFlock(const std::vector<Boid*>& boids) {
    float forwardBias = 0.00f;

    // Filter neighbors within perception radius
    std::vector<Boid*> neighbors;
    for (Boid* other : boids) {
        if (other == this) {
            continue;
        }
        const float d = distance(position, other->position);
        if (d > 0.0f && d < perceptionRadius) {
            neighbors.push_back(other);
        }
    }

    Vector2 forwardForce = velocity.normalized() * forwardBias;
    if (!neighbors.empty()) {
        separationForce = separate(neighbors);
        alignmentForce = align(neighbors);
        cohesionForce = cohesion(neighbors);

        Vector2 totalForce = separationForce + alignmentForce + cohesionForce + forwardForce;
        totalForce.limit(maxForce);
        applyForce(totalForce);

        if (isDebug()) {
            // std::cout << "Boid at (" << position.x << ", " << position.y << ") has " << neighbors.size() << " neighbors\n";
            // std::cout << " separationForce: (" << separationForce.x << ", " << separationForce.y << ")"
            //           << " alignmentForce: (" << alignmentForce.x << ", " << alignmentForce.y << ")"
            //           << " cohesionForce: (" << cohesionForce.x << ", " << cohesionForce.y << ")\n";
        }
    } else {
        separationForce = Vector2{0.0f, 0.0f};
        alignmentForce = Vector2{0.0f, 0.0f};
        cohesionForce = Vector2{0.0f, 0.0f};
        applyForce(forwardForce);
    }
}

void Boid::avoidEdges(float width, float height) {
    const float edgeMargin = 10.0f;
    const float edgeStrength = 25.0f;
    edgeForce = {0.0f, 0.0f};

    if (position.x < edgeMargin) {
        edgeForce.x += (edgeMargin - position.x) / edgeMargin * edgeStrength;
    } else if (position.x > width - edgeMargin) {
        edgeForce.x -= (position.x - (width - edgeMargin)) / edgeMargin * edgeStrength;
    }

    if (position.y < edgeMargin) {
        edgeForce.y += (edgeMargin - position.y) / edgeMargin * edgeStrength;
    } else if (position.y > height - edgeMargin) {
        edgeForce.y -= (position.y - (height - edgeMargin)) / edgeMargin * edgeStrength;
    }

    if (!edgeForce.isZero()) {
        applyForce(edgeForce);
    }

    if (isDebug()) {
        // std::cout << "Boid at (" << position.x << ", " << position.y << ") has edgeForce: (" << edgeForce.x << ", " << edgeForce.y << ")\n";
    }
}

} // namespace boid
