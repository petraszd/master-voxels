#ifndef _CUBE_H_
#define _CUBE_H_

#include <OgreEntity.h>
#include <OgreRay.h>
#include <OgreHardwarePixelBuffer.h>

class Cube
{
    private:
        Ogre::Entity *m_entity;
        Ogre::HardwarePixelBufferSharedPtr m_buffer;
    public:
        Cube(Ogre::Entity *cube);
        virtual ~Cube();

        void gap(const Ogre::Ray &ray);

    private:
        void lock();
        void unlock();
};

#endif

