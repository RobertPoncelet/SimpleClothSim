#ifndef CLOTH_H
#define CLOTH_H

#include "Common.h"
//#include "Spring.h"
#include "Solver.h"
#include <math.h>
#include <iostream>

//since m_particles is a one-dimensional array representing a two-dimensional grid,
//this macro is used to return the array index from two "co-ordinates" for readability
//(this system is faster than having a two-dimensional vector, right?)
#define PARTICLEINDEX(_x,_y) (_x)+((_y)*m_widthNum)


/// @file Cloth.h
/// @brief The main class that handles the cloth simulation.
/// @author Robert Poncelet
/// @version 1.0
/// @date 21/03/15
/// @class Cloth
/// @brief The main cloth simulation class which is mainly composed of a vector of particles, a
/// vector of springs and a solver.
class Cloth
{

public:

    /// @brief Constructor for the Cloth class.
    /// @param[in] _info A struct containing all the information needed to construct the cloth e.g.
    /// number of particles etc.
    Cloth(const CS::ClothInfo &_info);

    /// @brief Constructor for the Cloth class that uses the default construction parameters.
    Cloth();

    /// @brief Destructor for the Cloth class; does nothing special.
    ~Cloth();

    /// @brief The sphere used for demonstrating collision with the cloth. It is treated simply as
    /// as a particle like those of the cloth itself, but with no connected springs.
    CS::Particle m_sphere;

    /// @brief Advances the simulation to the next frame.
    /// @param[in] _time The time since the simulation started running. Probably obsolete.
    /// @param[in] _deltaSeconds The time in seconds since advance() was last called.
    void advance(const double &_time, const float &_deltaSeconds);

    /// @brief Returns the number of bytes needed to represent all the vertices in a tightly-packed array:
    /// 4 * (number of particles) - three for XYZ and another for index (needed in my implementation
    /// so the shader knows which particle this references).
    unsigned int getPointsArraySizeCopy();

    /// @brief Returns the number of bytes all the cloth's particles take up in total.
    unsigned int getPointsArraySizeDirect();

    /// @brief Fills the specified array with GLfloats representing the positions of the cloth's
    /// particles and their indices.
    /// @param[out] _array[] A pointer to the first index in the array, i.e. the array itself.
    void getPoints(GLfloat _array[]);

    /// @brief Returns the number of indices of particles.
    unsigned int getIndicesArraySize();

    /// @brief Returns the number of bytes needed to store the indices of all the particles.
    unsigned int getIndicesArraySizeBytes();

    /// @brief Fills the specified array with GLuints representing the cloth's indices.
    /// @param[out] _array[] A pointer to the first index in the array, i.e. the array itself.
    void getIndices(GLuint _array[]);

    /// @brief Returns the memory address of the very first entry in the particle vector. Used by
    /// OpenGL when it copies data to its buffers.
    float* getFirstIndex()      {return &m_particles[0].m_isAnchored;}

    /// @brief Returns the number of particles the cloth has along its X axis.
    int getWidthNum() const     {return m_widthNum;}

    /// @brief Returns the number of particles the cloth has along its Y axis.
    int getHeightHum() const    {return m_heightNum;}

    /// @brief Toggles the application of a turbulent wind-like force to the cloth.
    void toggleWind()           {m_solver.m_applyWind = !m_solver.m_applyWind;}

    /// @brief Returns whether the simulation is in suspended animation.
    bool isPaused() const       {return m_isPaused;}

    /// @brief Toggle whether the simulation is in suspended animation.
    void togglePaused()         {m_isPaused = !m_isPaused;}

    /// @brief Resets the simulation variables without actually re-creating the object.
    /// @param[in] _info A struct containing the values to reset the simulation variables to.
    void reset(const CS::ClothInfo &_info);

    /// @brief Returns a vector of Vert structs representing the data needed by OpenGL to draw the
    /// cloth. Now obsolete and replaced by getPoints() and getIndices().
    /// @param[in] _drawType A GL enum (e.g. GL_TRIANGLES) specifying how to format the data
    /// depending on how we want to draw it.
    std::vector<CS::Vert> getDrawData(const GLenum &_drawType);

    /// @brief Set whether the cloth's particles should collide with the sphere.
    /// @param[in] _shouldUse Whether the collisions should be applied.
    void setSphereCollisions(const bool &_shouldUse)    {m_solver.m_applySphereCollision = _shouldUse;}

    /// @brief Set whether the cloth's particles should collide with each other.
    /// @param[in] _shouldUse Whether the collisions should be applied.
    void setSelfCollisions(const bool &_shouldUse)      {m_solver.m_applySelfCollision = _shouldUse;}

    /// @brief Set the strength of the gravity that affects the particles.
    /// @param[in] _gravity The gravity strength (default is 32).
    void setGravity(const float &_gravity)              {m_solver.m_gravity = _gravity;}

    /// @brief Set the speed of the simulation. DeltaSeconds is multiplied by this during advance().
    /// @param[in] _speed The speed multiplier to use.
    void setSimSpeed(const float &_speed)               {m_solver.m_speed = _speed;}

    /// @brief Set the spring constant (stiffness) of all the springs in the cloth.
    /// @param[in] _constant The value to set the spring constant to.
    void setSpringConstant(const float &_constant);

    /// @brief Set the damping constant of all the springs in the cloth.
    /// @param[in] _constant The value to set the damping constant to.
    void setDampingConstant(const float &_constant);

    /// @brief Set the anchored state of the particle at a specified corner of the cloth.
    /// @param[in] _corner Which corner to set the state of:
    ///     0 = top left
    ///     1 = top right
    ///     2 = bottom left
    ///     3 = bottom right
    /// @param[in] _anchored Whether the cloth should be anchored or not.
    void setAnchoredCorner(const unsigned int &_corner, const bool &_anchored);

private:
    //attributes
    /// @brief Whether the simulation is in suspended animation.
    bool m_isPaused;

    /// @brief How many particles the cloth has along its X axis.
    int m_widthNum;

    /// @brief How many particles the cloth has along its Y axis.
    int m_heightNum;

    /// @brief A vector containing all the particles in the cloth.
    std::vector<CS::Particle> m_particles;

    /// @brief A vector containing all the springs that connect the cloth's particles.
    std::vector<CS::Spring> m_springs;

    /// @brief An object that handles the particles' and springs' physics and movement.
    Solver m_solver;

    //functions
    /// @brief Calculates the vertex normal for the specified particle. This is now obsolete as we
    /// do this on the shader.
    /// @param[in] _x The "index" of the specified particle along the X axis.
    /// @param[in] _y The "index" of the specified particle along the Y axis.
    /// @return The vertex normal at this particle's position.
    ngl::Vec3 makeNormal(const int &_x, const int &_y);

    /// @brief A simple clamp function.
    /// @param[in] _x The variable to clamp.
    /// @param[in] _min The lower limit to clamp to.
    /// @param[in] _max The upper limit to clamp to.
    /// @return The clamped integer.
    int clamp(const int &_x, const int &_min, const int &_max) {return std::min(_max, std::max(_min, _x));}

    /// @brief Returns the vector between two particles; used for readability and convenience.
    /// @param[in] _p1 The first particle.
    /// @param[in] _p2 The second particle.
    ngl::Vec3 vectorBetween(CS::Particle _p1, CS::Particle _p2) const {return _p2.m_pos - _p1.m_pos;}

    /// @brief Constructs a spring between the particles at the specified positions and adds it to
    /// our vector of springs.
    /// @param[in] _x1 The X index of the first particle.
    /// @param[in] _y1 The Y index of the first particle.
    /// @param[in] _x2 The X index of the second particle.
    /// @param[in] _y2 The Y index of the second particle.
    /// @param[in] _springConstant The spring constant of the spring to be created.
    /// @param[in] _springConstant The damping constant of the spring to be created.
    void addSpring(const unsigned int &_x1, const unsigned int &_y1, const unsigned int &_x2, const unsigned int &_y2, const float &_springConstant, const float &_dampingConstant);

    /// @brief Returns a pointer to the particle at the specified position.
    /// @param[in] _x The X index of the particle.
    /// @param[in] _y The Y index of the particle.
    CS::Particle* particleAt(const unsigned int &_x, const unsigned int &_y);

};

#endif // CLOTH_H
