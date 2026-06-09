#ifndef DJWOOT_FLOCK_H
#define DJWOOT_FLOCK_H

#include "boid.h"
#include <vector>
#include <set>

namespace boid {

class Flock {
public:
    Flock();

    void addBoid(const Boid& boid);
    void tick(float width, float height);
    void addAttractor(const Vector2& position);
    void removeAttractor(const Vector2& position);
    void setAttractors(std::set<Vector2>* newAttractors) { attractors = newAttractors; }

    const std::vector<Boid>& getBoids() const;

private:
    std::vector<Boid> boids;
    std::set<Vector2>* attractors;
};

} // namespace boid

#endif // DJWOOT_FLOCK_H
