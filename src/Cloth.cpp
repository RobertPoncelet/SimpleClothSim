#include "Cloth.h"
#define WIDTH 2.56f
#define HEIGHT 1.636f
#define SPRINGCONSTANT 1024.0f
#define DAMPINGCONSTANT 512.0f
#define MASS 1.0f

Cloth::Cloth() : m_sphere(0, 1.0f, 1.0f, ngl::Vec3(0.0f, 0.0f, -2.0f)), m_isPaused(false), m_widthNum(16), m_heightNum(16)
{
    CS::ClothInfo info;
    info.dampingConstant = 512.0f;
    info.height = 1.636f;
    info.heightNum = 16;
    info.sphereRadius = 1.0f;
    info.springConstant = 1024.0f;
    info.width = 2.56f;
    info.widthNum = 16;
    info.anchoredBottomLeft = false;
    info.anchoredBottomRight = false;
    info.anchoredTopLeft = true;
    info.anchoredTopRight = true;

    reset(info);
}

Cloth::~Cloth()
{

}

void Cloth::advance(const double &_time, const float &_deltaSeconds)
{
    m_solver.advance(&m_springs, &m_particles, _time, _deltaSeconds);
}

//NOTE: Particle "co-ordinates" (indices) are referred to in a similar way to pixels in an image
//i.e. X increases going "right", Y increases going "down"

//==================== OBSOLETE ====================
std::vector<CS::Vert> Cloth::getDrawData(const GLenum &_drawType)
{
    std::vector<CS::Vert> vec;
    CS::Vert v;
    v.uv.set(0.0f,0.0f);
    v.n.set(0.0f, 0.0f, 1.0f);

    switch(_drawType)
    {//begin switch -------------------------------------------------------------

        case GL_POINTS:
        {
            for (std::vector<CS::Particle>::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
            {
                v.p.set((*it).m_pos);
                vec.push_back(v);
            }
            break;
        }

        case GL_LINES:
        {
            for (unsigned int i=0; i<m_springs.size(); ++i)
            {
                    v.p.set(m_springs[i].m_startParticle->m_pos);
                    vec.push_back(v);

                    v.p.set(m_springs[i].m_endParticle->m_pos);
                    vec.push_back(v);
            }
            break;
        }

        case GL_TRIANGLES:
        {
            //int count = 0;
            for (int x=0; x<m_widthNum-1; ++x)
            {
                for (int y=0; y<m_heightNum-1; ++y)
                {
                    //precompute normals so we don't do more than necessary
                    ngl::Vec3 normalTopLeft = makeNormal(x,y);
                    ngl::Vec3 normalTopRight = makeNormal(x+1,y);
                    ngl::Vec3 normalBottomLeft = makeNormal(x,y+1);
                    ngl::Vec3 normalBottomRight = makeNormal(x+1,y+1);

                    //first triangle
                    //top-left
                    v.p.set(m_particles[PARTICLEINDEX(x,y)].m_pos);
                    v.uv.set(ngl::Vec2((float)x/(float)(m_widthNum-1),(float)y/(float)(m_heightNum-1)));
                    v.n.set(normalTopLeft);
                    vec.push_back(v);

                    //top-right
                    v.p.set(m_particles[PARTICLEINDEX((x+1),y)].m_pos);
                    v.uv.set(ngl::Vec2((float)(x+1)/(float)(m_widthNum-1),(float)y/(float)(m_heightNum-1)));
                    v.n.set(normalTopRight);
                    vec.push_back(v);

                    //bottom-left
                    v.p.set(m_particles[PARTICLEINDEX(x,(y+1))].m_pos);
                    v.uv.set(ngl::Vec2((float)x/(float)(m_widthNum-1),(float)(y+1)/(float)(m_heightNum-1)));
                    v.n.set(normalBottomLeft);
                    vec.push_back(v);

                    //second triangle
                    //bottom-left again
                    vec.push_back(v);

                    //bottom-right
                    v.p.set(m_particles[PARTICLEINDEX(x+1,(y+1))].m_pos);
                    v.uv.set(ngl::Vec2((float)(x+1)/(float)(m_widthNum-1),(float)(y+1)/(float)(m_heightNum-1)));
                    v.n.set(normalBottomRight);
                    vec.push_back(v);

                    //top-right
                    v.p.set(m_particles[PARTICLEINDEX(x+1,y)].m_pos);
                    v.uv.set(ngl::Vec2((float)(x+1)/(float)(m_widthNum-1),(float)y/(float)(m_heightNum-1)));
                    v.n.set(normalTopRight);
                    vec.push_back(v);
                }
            }
            break;
        }

        default:
        {
            break;
        }
    }//end switch -------------------------------------------------------------

    return vec;
}

//before calling this function, make sure you have enough memory allocated; getPointsArraySize() will tell you how many GLfloats you need
//should be called every frame
void Cloth::getPoints(GLfloat _array[])
{
    unsigned int arrayIndex = 0;
    unsigned int vertIndex = 0;
    for(std::vector<CS::Particle>::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
    {
        _array[arrayIndex++] = (*it).m_pos.m_x;
        _array[arrayIndex++] = (*it).m_pos.m_y;
        _array[arrayIndex++] = (*it).m_pos.m_z;
        _array[arrayIndex++] = GLfloat(vertIndex++);
    }
}

unsigned int Cloth::getPointsArraySizeCopy()
{
    return (unsigned int)m_particles.size() * 16;
}

unsigned int Cloth::getPointsArraySizeDirect()
{
    return (unsigned int)m_particles.size() * sizeof(CS::Particle);
}

//before calling this function, make sure you have enough memory allocated; getIndicesArraySize() will tell you how many GLuints you need
//should be called once for each reset()
void Cloth::getIndices(GLuint _array[])
{
    unsigned int index = 0;
    for(GLuint y = 0; y < (GLuint)m_heightNum - 1; ++y)
    {
        for(GLuint x = 0; x < (GLuint)m_widthNum - 1; ++x)
        {
            //===== TRIANGLE 1 =====
            _array[index++] = PARTICLEINDEX(x,y);     //top-left
            _array[index++] = PARTICLEINDEX(x+1,y);   //top-right
            _array[index++] = PARTICLEINDEX(x,y+1);   //bottom-left
            //===== TRIANGLE 2 =====
            _array[index++] = PARTICLEINDEX(x,y+1);   //bottom-left
            _array[index++] = PARTICLEINDEX(x+1,y+1); //bottom-right
            _array[index++] = PARTICLEINDEX(x+1,y);   //top-right
        }
    }
}

unsigned int Cloth::getIndicesArraySize()
{
    return 6 * (m_widthNum - 1) * (m_heightNum - 1);
}

unsigned int Cloth::getIndicesArraySizeBytes()
{
    return 6 * (m_widthNum - 1) * (m_heightNum - 1) * sizeof(GLuint);
}

//==================== OBSOLETE ====================
ngl::Vec3 Cloth::makeNormal(const int &_x, const int &_y)
{
    CS::Particle* particle = particleAt(_x, _y);

    int clampedX = clamp(_x,1,m_widthNum-2);
    int clampedY = clamp(_y,1,m_heightNum-2);

    ngl::Vec3 upVector = particle->m_pos - particleAt(clampedX,clampedY+1)->m_pos;
    ngl::Vec3 downVector = particleAt(clampedX,clampedY-1)->m_pos - particle->m_pos;
    ngl::Vec3 leftVector = particle->m_pos - particleAt(clampedX-1,clampedY)->m_pos;
    ngl::Vec3 rightVector = particleAt(clampedX+1,clampedY)->m_pos - particle->m_pos;

    ngl::Vec3 NEnormal = upVector.cross(rightVector);
    ngl::Vec3 NWnormal = upVector.cross(leftVector);
    ngl::Vec3 SWnormal = downVector.cross(leftVector);
    ngl::Vec3 SEnormal = downVector.cross(rightVector);

    ngl::Vec3 normal = ngl::Vec3(NWnormal+NEnormal+SEnormal+SWnormal);
    //no need to normalize since this is done in the shader anyway
    //normal.normalize();

    return normal;
}

void Cloth::addSpring(const unsigned int &_x1, const unsigned int &_y1, const unsigned int &_x2, const unsigned int &_y2, const float &_springConstant, const float &_dampingConstant)
{
    m_springs.push_back( CS::Spring( particleAt(_x1, _y1), particleAt(_x2, _y2), _springConstant, _dampingConstant ) );
}

CS::Particle* Cloth::particleAt(const unsigned int &_x, const unsigned int &_y)
{
    if (_x < (const unsigned int)m_widthNum && _y < (const unsigned int)m_heightNum)
    {
        return &m_particles[_x+(_y*m_widthNum)];
    }
    return NULL;
}

void Cloth::reset(const CS::ClothInfo &_info)
{
    m_widthNum = _info.widthNum;
    m_heightNum = _info.heightNum;

    //make radius slightly shorter than the minimum distance between particles
    float radius = _info.width/_info.widthNum > _info.height/_info.heightNum ? _info.height/_info.heightNum : _info.width/_info.widthNum;
    radius*=0.5f;

    m_particles.clear();

    //generate particles
    int count = 0;
    for (int y=0; y<_info.heightNum; ++y)
    {
        for (int x=0; x<_info.widthNum; ++x)
        {
            float xPos = x * (_info.width/_info.widthNum) - _info.width/2.0f;
            float yPos = y * (_info.height/_info.heightNum) - _info.height/2.0f;
            ngl::Vec3 pos = ngl::Vec3(xPos, yPos, 0.0f);
            CS::Particle part = CS::Particle(count++,MASS,radius,pos);
            m_particles.push_back(part);
        }
    }

    m_springs.clear();

    //generate horizontal structural springs
    for (int x=0; x<_info.widthNum-1; ++x)
    {
        for (int y=0; y<_info.heightNum; ++y)
        {
            addSpring(x,y,x+1,y,_info.springConstant,_info.dampingConstant);
        }
    }

    //generate vertical structural springs
    for (int x=0; x<_info.widthNum; ++x)
    {
        for (int y=0; y<_info.heightNum-1; ++y)
        {
            addSpring(x,y,x,y+1,_info.springConstant,_info.dampingConstant);
        }
    }

    //generate horizontal bend springs
    for (int x=0; x<_info.widthNum-3; ++x)
    {
        for (int y=0; y<_info.heightNum; ++y)
        {
            addSpring(x,y,x+3,y,_info.springConstant,_info.dampingConstant);
        }
    }

    //generate vertical bend springs
    for (int x=0; x<_info.widthNum; ++x)
    {
        for (int y=0; y<_info.heightNum-3; ++y)
        {
            addSpring(x,y,x,y+3,_info.springConstant,_info.dampingConstant);
        }
    }

    //generate top-left to bottom-right shear springs
    for (int x=0; x<_info.widthNum-1; ++x)
    {
        for (int y=0; y<_info.heightNum-1; ++y)
        {
            addSpring(x,y,x+1,y+1,_info.springConstant,_info.dampingConstant);
        }
    }

    //generate top-right to bottom-left shear springs
    for (int x=1; x<_info.widthNum; ++x)//start from 1 as these springs extend backwards in the x axis
    {
        for (int y=0; y<_info.heightNum-1; ++y)
        {
            addSpring(x,y,x-1,y+1,_info.springConstant,_info.dampingConstant);
        }
    }

    if (_info.anchoredTopLeft)
    {
        setAnchoredCorner(0, true);
    }
    if (_info.anchoredTopRight)
    {
        setAnchoredCorner(1, true);
    }
    if (_info.anchoredBottomLeft)
    {
        setAnchoredCorner(2, true);
    }
    if (_info.anchoredBottomRight)
    {
        setAnchoredCorner(3, true);
    }

    //do an arbitrary force on a particle so not all initial movement is in the XY plane
    particleAt(m_widthNum/2,m_heightNum/2)->addForce(ngl::Vec3(-0.5f,-0.5f,-0.5f));

    m_sphere.m_isAnchored = true;
    m_solver.m_sphere = &m_sphere;
}

void Cloth::setSpringConstant(const float &_constant)
{
    for (std::vector<CS::Spring>::iterator it = m_springs.begin(); it != m_springs.end(); ++it)
    {
        (*it).m_springConstant = _constant;
    }
}

void Cloth::setDampingConstant(const float &_constant)
{
    for (std::vector<CS::Spring>::iterator it = m_springs.begin(); it != m_springs.end(); ++it)
    {
        (*it).m_dampingConstant = _constant;
    }
}

void Cloth::setAnchoredCorner(const unsigned int &_corner, const bool &_anchored)
{
    switch(_corner)
    {
        case 0 : particleAt(0,m_heightNum-1)->m_isAnchored = _anchored;            break;
        case 1 : particleAt(m_widthNum-1,m_heightNum-1)->m_isAnchored = _anchored; break;
        case 2 : particleAt(0,0)->m_isAnchored = _anchored;                        break;
        case 3 : particleAt(m_widthNum-1,0)->m_isAnchored = _anchored;             break;
        default: break;
    }
}
