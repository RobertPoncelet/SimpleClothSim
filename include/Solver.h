#ifndef SOLVER_H
#define SOLVER_H

#include "Common.h"

/// @file Solver.h
/// @brief Source file for the Solver class that works for the Cloth.
/// @author Robert Poncelet
/// @version 1.0
/// @date 21/03/15
/// @class Solver
/// @brief Does most of the "heavy lifting" of the cloth simulation, i.e. calculating the forces and
/// positions of the cloth components.

class Solver
{
public:
    /// @brief Constructor for the Solver class.
    Solver();

    /// @brief Destructor for the Solver class.
    ~Solver();

    /// @brief Advance the simulation forward a frame according to its current state.
    /// @param[in,out] _springs A pointer to a vector containing all the springs in the Cloth.
    /// @param[in,out] _particles A pointer to a vector containing all the particles in the Cloth.
    /// @param[in] _time How much time has passed since the simulation began (only really used for
    /// wind).
    /// @param[in] _deltaSeconds The time in seconds since the last call to advance().
    void advance(std::vector<CS::Spring>* _springs, std::vector<CS::Particle>* _particles, const double &_time, const float &_deltaSeconds);

    /// @brief Apply forces to the particles the specified spring connects depending on the length
    /// of the spring.
    /// @param[in] _spring A pointer to the spring in question.
    void updateSpring(CS::Spring* _spring);

    /// @brief Update the particle's position depending on the forces it has accumulated from
    /// neighbouring springs and according to Verlet integration.
    /// @param[in,out] _particle A pointer to the particle in question.
    /// @param[in] _deltaSeconds The time in seconds since the last call to advance().
    void updateParticle(CS::Particle* _particle, float _deltaSeconds);

    /// @brief Applies a separating force to the specified particles if their radii intersect i.e.
    /// they have collided. Obselete; replaced by resolveCollisionTranslate().
    /// @param[in,out] _firstParticle One of the particles involved in the collision test.
    /// @param[in,out] _secondParticle The other particle involved in the collision test.
    /// @return Whether there was a collision; useful for debugging.
    bool resolveCollision(CS::Particle* _firstParticle, CS::Particle* _secondParticle);

    /// @brief Similar to resolveCollision() but simply translates the particles away from each other
    /// rather than applying a force; because of the way Verlet integration works, the end result is
    /// the same as neatly applying an impulse.
    /// @param[in,out] _firstParticle One of the particles involved in the collision test.
    /// @param[in,out] _secondParticle The other particle involved in the collision test.
    /// @return Whether there was a collision; useful for debugging.
    bool resolveCollisionTranslate(CS::Particle *_firstParticle, CS::Particle *_secondParticle);

    /// @brief Whether to check for and resolve collisions between internal particles.
    bool m_applySelfCollision;
    /// @brief Whether to check for and resolve collisions between cloth particles and the sphere.
    bool m_applySphereCollision;
    /// @brief Whether to apply a turbulent wind-like force to the particles.
    bool m_applyWind;
    /// @brief The strength of the gravity to apply to the particles.
    float m_gravity;
    /// @brief The speed of the simulation. DeltaSeconds is multiplied by this during advance().
    float m_speed;
    /// @brief A pointer to the collision-demo sphere.
    CS::Particle* m_sphere;
};

#endif // SOLVER_H
