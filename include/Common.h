#ifndef COMMON_H
#define COMMON_H

#include "ngl/Vec2.h"
#include "ngl/Vec3.h"
#include <vector>

/// @file Common.h
/// @brief A few useful structs are defined here.
/// @author Robert Poncelet
/// @version 1.0
/// @date 21/03/15

namespace CS
{
    /// @brief A struct containing the information needed to construct a cloth object.
    struct ClothInfo
    {
        /// @brief Whether the particle at the top left corner is anchored.
        bool anchoredTopLeft;
        /// @brief Whether the particle at the top right corner is anchored.
        bool anchoredTopRight;
        /// @brief Whether the particle at the bottom left corner is anchored.
        bool anchoredBottomLeft;
        /// @brief Whether the particle at the bottom right corner is anchored.
        bool anchoredBottomRight;
        /// @brief How many particles the cloth has in the X direction.
        int widthNum;
        /// @brief How many particles the cloth has in the Y direction.
        int heightNum;
        /// @brief The width of the cloth sheet.
        float width;
        /// @brief The height of the cloth sheet.
        float height;
        /// @brief The spring constant of all the cloth's springs.
        float springConstant;
        /// @brief The damping constant of all the cloth's springs.
        float dampingConstant;
        /// @brief The radius of the collision-demo sphere.
        float sphereRadius;

        /// @brief A default constructor for the struct.
        ClothInfo():widthNum(), heightNum(),width(),height(),springConstant(),dampingConstant(),sphereRadius()
        {;}
    };

    /// @brief A struct containing a position and common attributes of a vertex; useful for sending
    /// to OpenGL.
    typedef struct
    {
        /// @brief Texture co-ordinates for this vertex.
        ngl::Vec2 uv;
        /// @brief The vertex normal.
        ngl::Vec3 n;
        /// @brief The vertex position.
        ngl::Vec3 p;
    }
    Vert;

    /// @brief A structure containing all the attributes of a particle; this is used instead of a
    /// class so its members can be accessed by a memory address and an offset.
    struct Particle
    {
        /// @brief Whether this particle is "anchored", i.e. the cloth will hang from this point.
        /// This attribute is a float rather than a bool so that we know that the next attribute
        /// or vector component is always sizeof(float) bytes away.
        float m_isAnchored;
        /// @brief Where this particle is in the vector. Useful for sending to the shader so it
        /// knows where to look in the position/normal textures. It is a float so it can be used in
        /// the same vertex buffer as the positions, which are stored as floats too.
        float m_index;
        /// @brief The mass of the particle. Does not change in my implementation so has little
        /// effect on the simulation.
        float m_mass;
        /// @brief The particle's radius, used for spherical collisions.
        float m_radius;
        /// @brief The world-space position of the particle.
        ngl::Vec3 m_pos;
        /// @brief The world-space position of the particle at the previous simulation frame. Verlet
        /// integration uses this rather than a velocity vector.
        ngl::Vec3 m_prevPos;
        /// @brief The sum of the forces this particle has accumulated during one simulation step.
        ngl::Vec3 m_pendingForce;

        /// @brief Returns the acceleration of the particle. This is used in Verlet integration.
        ngl::Vec3 getAcceleration() const            {return m_isAnchored ? ngl::Vec3(0.0f,0.0f,0.0f) : -m_pendingForce/m_mass;}
        /// @brief Add to the pendingForce value, which will be applied to the particle and cleared
        /// once all springs have been updated.
        void addForce(const ngl::Vec3 &_force)       {m_pendingForce+=_force;}
        /// @brief Apply a relative translation, skipping the force step.
        void move(const ngl::Vec3 &_pos)             {m_pos+=_pos;}
        /// @brief Sets pendingForce to zero. Used after the result of the accumulated forces has
        /// actually been applied to the particle.
        void resetForce()                            {m_pendingForce=ngl::Vec3(0.0f,0.0f,0.0f);}

        /// @brief A default constructor for the particle.
        Particle():m_isAnchored(),m_index(),m_mass(),m_radius(),m_pos(),m_prevPos(),m_pendingForce()
        {;}

        /// @brief A constructor allowing all the attributes to be set.
        Particle(unsigned int index_,float mass_,float radius_,const ngl::Vec3&pos_):   m_isAnchored(false),
                                                                                        m_index(index_),
                                                                                        m_mass(mass_),
                                                                                        m_radius(radius_),
                                                                                        m_pos(pos_),
                                                                                        m_prevPos(ngl::Vec3(0.f,0.f,0.f)),
                                                                                        m_pendingForce(ngl::Vec3(0.f,0.f,0.f))
        {;}
    };

    /// @brief A struct containing all the attributes of a spring; these connect the particles to form
    /// the cloth surface and apply forces to them depending on their length.
    struct Spring
    {
        /// @brief Returns the "restoring force" the spring exerts on its endpoints according to Hooke's
        /// law, its length and its internal constants.
        ngl::Vec3 getSpringForce() const
        {
            //do Hooke's law stuff here
            float extension = (getLength()-getRestLength());
            return -m_springConstant*extension*getSpringVector(); //-kx as a vector
        }

        /// @brief Returns the vector between the spring's start and end particles.
        ngl::Vec3 getSpringVector() const
        {
            return ngl::Vec3(m_endParticle->m_pos - m_startParticle->m_pos);
        }

        /// @brief returns the rest length.
        float getRestLength() const {return m_restLength;}

        /// @brief returns the current length.
        float getLength() const {return getSpringVector().length();}

        /// @brief The first particle this spring connects.
        Particle* m_startParticle;
        /// @brief The second particle this spring connects.
        Particle* m_endParticle;
        /// @brief The spring (stiffness) constant of the spring.
        float m_springConstant;
        /// @brief The damping constant of the spring i.e. how quickly it will come to rest.
        float m_dampingConstant;
        /// @brief The default length the spring "tries" to maintain with its forces.
        float m_restLength;

        /// @brief The constructor for the Spring class.
        /// @param _startParticle The first particle to connect.
        /// @param _endParticle The second particle to connect.
        /// @param _springConstant The spring (stiffness) constant of the spring.
        /// @param _dampingConstant The damping constant of the spring i.e. how quickly it will come to
        /// rest.
        Spring(Particle* _startParticle, Particle* _endParticle, const float &_springConstant, const float &_dampingConstant):  m_startParticle(_startParticle),
                                                                                                                                m_endParticle(_endParticle),
                                                                                                                                m_springConstant(_springConstant),
                                                                                                                                m_dampingConstant(_dampingConstant),
                                                                                                                                m_restLength(getLength())
        {;}
    };
}

#endif // COMMON_H
