#include <cube.h>
#include <OgreSubEntity.h>
#include <OgreTechnique.h>
#include <OgreMath.h>
#include <OgreSceneNode.h>
#include <OgreMatrix4.h>
#include <volume.h>
#include <QDebug>

using namespace Ogre;

Cube::Cube(Entity *entity) :
    m_entity(entity)
{
    m_buffer = m_entity->getSubEntity(0)
        ->getMaterial()
        ->getTechnique(0)
        ->getPass(0)
        ->getTextureUnitState(1)
        ->_getTexturePtr()
        ->getBuffer();
    // Ok.. A little bit long call...
}

Cube::~Cube()
{
}

// TODO: Just delete everything and rewrite this method from scratch
// It's bad, bad. So BAD I should change my profesion...
// But it works...
void Cube::gap(const Ogre::Ray &ray)
{
    std::pair<bool, Real> interTest =
        ray.intersects(m_entity->getWorldBoundingBox());

    if (!interTest.first)
        return;
    Real t = interTest.second;

    SceneNode *node = m_entity->getParentSceneNode();
    Vector3 pos = node->convertWorldToLocalPosition(ray.getPoint(t));

    lock();

    Volume vol(m_buffer->getCurrentLock());

    // Assumes that width == height == depth
    // TODO: make these constants as config.cfg params
    Real step = 1.0 / (vol.width() * 0.5);
    unsigned int x, y, z;
    unsigned int gapSize = vol.width() / 20;
    do {
        x = vol.xFromRelative(pos.x);
        y = vol.yFromRelative(pos.y);
        z = vol.zFromRelative(pos.z);

        vol.hide(x, y, z, gapSize);

        t += step;
        pos = node->convertWorldToLocalPosition(ray.getPoint(t));
    } while (pos.x > -0.5 && pos.x < 1.5 && pos.y > -0.5 && pos.y < 1.5 && pos.z > -0.5 && pos.z < 1.5);
    // Cheap trick.

    unlock();
}

void Cube::lock()
{
    m_buffer->lock(HardwareBuffer::HBL_NORMAL);
}

void Cube::unlock()
{
    m_buffer->unlock();
}

