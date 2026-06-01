#ifndef DJWOOT_FLOCK_H
#define DJWOOT_FLOCK_H

#include "boid.h"
#include <vector>

namespace boid {

class Flock {
public:
    Flock();

    void addBoid(const Boid& boid);
    void tick(float width, float height);

    const std::vector<Boid>& getBoids() const;

private:
    std::vector<Boid> boids;
};

} // namespace boid

#endif // DJWOOT_FLOCK_H
