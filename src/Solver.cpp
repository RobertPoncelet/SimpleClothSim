
#include "Solver.h"
#include <iostream>
#include <math.h>
#include <ngl/NGLStream.h>

#define IMPULSE_SCALE 2.0f
#define AIR_RESISTANCE -256.f

Solver::Solver() : m_applySelfCollision(false), m_applySphereCollision(true), m_applyWind(false), m_gravity(32.0f), m_speed(1.0f)
{
}

Solver::~Solver()
{

}

void Solver::advance(std::vector<CS::Spring>* _springs, std::vector<CS::Particle>* _particles, const double &_time, const float &_deltaSeconds)
{
    //calculate the springs' forces acting on the particles
    for(std::vector<CS::Spring>::iterator it=_springs->begin(); it!=_springs->end(); ++it)
    {
        updateSpring(&(*it));
    }

    //calculate other forces and then update particle positions accordingly
    for(std::vector<CS::Particle>::iterator it=_particles->begin(); it!=_particles->end(); ++it)
    {
        if(!(*it).m_isAnchored)
        {
            //gravity
            (*it).addForce(ngl::Vec3(0,m_gravity,0));

            //air resistance
            ngl::Vec3 approximateVelocity = (*it).m_prevPos - (*it).m_pos;
            (*it).addForce(AIR_RESISTANCE * approximateVelocity);

            //wind
            if(m_applyWind)
            {
                ngl::Vec3 wind = 512.0f*ngl::Vec3(0.0f,0.0f,cos((_time*128+(*it).m_pos.m_x*32+(*it).m_pos.m_y*32)*0.1)*0.15);//arbitrary function to create energy in the system
                (*it).addForce(wind);
            }

            updateParticle(&(*it), _deltaSeconds);
        }
    }

    if (m_applySelfCollision)
    {
        //adjust for collisions
        for(std::vector<CS::Particle>::iterator first=_particles->begin(); first!=_particles->end()-1; ++first)
        {
            for(std::vector<CS::Particle>::iterator second=first+1; second!=_particles->end(); ++second)
            {
                    resolveCollisionTranslate(&(*first), &(*second));
            }
        }
    }

    if (m_sphere && m_applySphereCollision)
    {
        for(std::vector<CS::Particle>::iterator it=_particles->begin(); it!=_particles->end(); ++it)
        {
            resolveCollisionTranslate(&(*it), m_sphere);
        }
    }
}

void Solver::updateParticle(CS::Particle *_particle, float _deltaSeconds)
{
    float newDelta = _deltaSeconds * m_speed;
    ngl::Vec3 approximateVelocity = _particle->m_pos - _particle->m_prevPos;

    //Do Verlet integration here
    ngl::Vec3 moveBy = approximateVelocity
            + (newDelta * newDelta * _particle->getAcceleration());

    _particle->m_prevPos = _particle->m_pos;
    _particle->move(moveBy);
    _particle->resetForce();
}

void Solver::updateSpring(CS::Spring *_spring)
{
    CS::Particle* startParticle = _spring->m_startParticle;
    CS::Particle* endParticle = _spring->m_endParticle;

    ngl::Vec3 force = _spring->getSpringForce();
    ngl::Vec3 startVelocity = startParticle->m_pos - startParticle->m_prevPos;
    ngl::Vec3 endVelocity = endParticle->m_pos - endParticle->m_prevPos;
    ngl::Vec3 damping = (endVelocity-startVelocity) * _spring->m_dampingConstant;

    startParticle->addForce(force-damping);
    endParticle->addForce(damping-force);//the other direction
}

//==================== OBSOLETE ====================
bool Solver::resolveCollision(CS::Particle *_firstParticle, CS::Particle *_secondParticle)
{
    if (_firstParticle->m_isAnchored && _secondParticle->m_isAnchored)
    {
        //do nothing
        return false;
    }

    ngl::Vec3 firstPos = _firstParticle->m_pos;
    ngl::Vec3 secondPos = _secondParticle->m_pos;
    float combinedRadii = _firstParticle->m_radius + _secondParticle->m_radius;
    ngl::Vec3 between = secondPos - firstPos;
    if (between.lengthSquared() < combinedRadii * combinedRadii)
    {
        _firstParticle->resetForce();
        _secondParticle->resetForce();
        float penetration = combinedRadii - between.length();
        between.normalize();

        if (_firstParticle->m_isAnchored)
        {
            _secondParticle->addForce(-secondPos - (between * penetration * 2.0f * IMPULSE_SCALE));
        }
        else if (_secondParticle->m_isAnchored)
        {
            _firstParticle->addForce((between * penetration * 2.0f * IMPULSE_SCALE) - firstPos);
        }
        else
        {
            _firstParticle->addForce((between * penetration * IMPULSE_SCALE) - firstPos);
            _secondParticle->addForce(-secondPos - (between * penetration * IMPULSE_SCALE));
        }

        return true;
    }
    return false;
}

bool Solver::resolveCollisionTranslate(CS::Particle *_firstParticle, CS::Particle *_secondParticle)
{
    if (_firstParticle->m_isAnchored && _secondParticle->m_isAnchored)
    {
        //do nothing
        return false;
    }

    ngl::Vec3 firstPos = _firstParticle->m_pos;
    ngl::Vec3 secondPos = _secondParticle->m_pos;
    float combinedRadii = _firstParticle->m_radius + _secondParticle->m_radius;
    ngl::Vec3 between = secondPos - firstPos;
    if (between.lengthSquared() < combinedRadii * combinedRadii)
    {
        _firstParticle->resetForce();
        _secondParticle->resetForce();
        float penetration = combinedRadii - between.length();
        between.normalize();

//        simple collision resolution
//        (no different for us since mass is always 1)
//        if (_firstParticle->m_isAnchored)
//        {
//            _secondParticle->move(between * penetration);
//        }
//        else if (_secondParticle->m_isAnchored)
//        {
//            _firstParticle->move(-between * penetration);
//        }
//        else
//        {
//            _firstParticle->move(-between * penetration * 0.5f);
//            _secondParticle->move(between * penetration * 0.5f);
//        }

        float inverseMassA;
        float inverseMassB;

        if (_firstParticle->m_isAnchored)
        {
            inverseMassA = 0.f;
        }
        else
        {
            inverseMassA = 1.f/_firstParticle->m_mass;
        }

        if (_secondParticle->m_isAnchored)
        {
            inverseMassB = 0.f;
        }
        else
        {
            inverseMassB = 1.f/_firstParticle->m_mass;
        }

        float weightA = inverseMassA / (inverseMassA + inverseMassB);
        float weightB = inverseMassB / (inverseMassA + inverseMassB);

        _firstParticle->move(-between * penetration * weightA);
        _secondParticle->move(between * penetration * weightB);

        return true;
    }
    return false;
}
