#ifndef _VOLUME_H_
#define _VOLUME_H_

#include <OgrePrerequisites.h>
#include <OgrePixelFormat.h>
#include <OgreVector3.h>
#include <QtGlobal>

class VolumeElem
{
    public:
        void setRGBA(Ogre::uint8 r, Ogre::uint8 g, Ogre::uint8 b,
                     Ogre::uint8 a) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }

        void setRGB(Ogre::uint8 r, Ogre::uint8 g, Ogre::uint8 b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        void setRGB(const Ogre::Vector3 &v) {
            this->r = v.x;
            this->g = v.y;
            this->b = v.z;
        }

        Ogre::uint8 r;
        Ogre::uint8 g;
        Ogre::uint8 b;
        Ogre::uint8 a;
};

class Volume
{
    private:
        VolumeElem* m_elems;
        unsigned int m_w;
        unsigned int m_h;
        unsigned int m_d;
    public:
        Volume(const Ogre::PixelBox &box);
        Volume(Ogre::uint8 *raw,
               unsigned int width,
               unsigned int height,
               unsigned int depth);

        bool hasElem(unsigned int x,
                     unsigned int y,
                     unsigned int z);

        void hide(unsigned int x,
                  unsigned int y,
                  unsigned int z,
                  int radius);

        unsigned int xFromRelative(Ogre::Real x) const {
            return (unsigned int)(qAbs(x) * m_w);
        }

        unsigned int yFromRelative(Ogre::Real y) const {
            return (unsigned int)(qAbs(y) * m_h);
        }

        unsigned int zFromRelative(Ogre::Real z) const {
            return (unsigned int)(qAbs(z) * m_d);
        }

        VolumeElem& elem(unsigned int x,
                         unsigned int y,
                         unsigned int z) {
            return m_elems[m_h * m_w * z + m_w * y + x];
        }

        VolumeElem& elem(unsigned int index) {
            return m_elems[index];
        }

        unsigned int width() const {
            return m_w;
        }

        unsigned int height() const {
            return m_h;
        }

        unsigned int depth() const {
            return m_d;
        }
};

#endif

