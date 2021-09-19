#ifndef _MULTITEXTURE_H_
#define _MULTITEXTURE_H_

#include <OgreString.h>
#include <QList>

struct MultiTextureFileInfo
{
    Ogre::String fileName;
    unsigned int width;
    unsigned int height;
    unsigned int depth;

    bool isValid() const {
        return width > 0 && height > 0 && depth > 0 && !fileName.empty();
    }

    unsigned int getMaxDim() const {
        return qMax(width, qMax(height, depth));
    }

    const Ogre::String textureName() const {
        assert(isValid());
        return fileName + "3DTexture";
    }
};

class MultiTexture
{
    private:
        unsigned int m_dimension;
        QList<MultiTextureFileInfo> m_textures;

    public:
        MultiTexture();
        void extractInfo(const char *info, unsigned int infoSize);

    public:
        unsigned int dimension() const {
            return m_dimension;
        }

        unsigned int texturesCount() const {
            return m_textures.size();
        }

        const MultiTextureFileInfo& at(unsigned int i) const {
            return m_textures[i];
        }

        const MultiTextureFileInfo& at(unsigned int x,
                                       unsigned int y,
                                       unsigned int z) const {
            return m_textures[z * m_dimension * m_dimension + y * m_dimension +  x];
        }

        bool hasTexture(unsigned int x,
                        unsigned int y,
                        unsigned int z) const {
            return at(x, y, z).isValid();
        }

        Ogre::String getName(unsigned int x,
                             unsigned int y,
                             unsigned int z) const {
            return at(x, y, z).textureName();
        }

        Ogre::String getName(unsigned int i) const {
            return at(i).textureName();
        }
};

#endif

