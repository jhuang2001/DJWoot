#include "flock.h"

namespace boid {

Flock::Flock() = default;

void Flock::addBoid(const Boid& boid) {
    boids.push_back(boid);
}

void Flock::tick(float width, float height) {
    std::vector<Boid*> boidPtrs;
    boidPtrs.reserve(boids.size());
    for (Boid& boid : boids) {
        boidPtrs.push_back(&boid);
    }

    for (Boid& boid : boids) {
        boid.updateFromFlock(boidPtrs);
        boid.avoidEdges(width, height);
    }

    for (Boid& boid : boids) {
        boid.update();
    }
}

void Flock::addAttractor(const Vector2& position) {
    attractors->insert(position);
}

void Flock::removeAttractor(const Vector2& position) {
    attractors->erase(position);
}


const std::vector<Boid>& Flock::getBoids() const {
    return boids;
}

} // namespace boid
