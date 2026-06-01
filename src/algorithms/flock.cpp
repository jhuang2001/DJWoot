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

const std::vector<Boid>& Flock::getBoids() const {
    return boids;
}

} // namespace boid
