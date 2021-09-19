#ifndef _TEXTUREUTIL_H_
#define _TEXTUREUTIL_H_

#include <multitexture.h>
#include <OgreTexture.h>
#include <OgreVector3.h>
#include <volume.h>
#include <zzip/lib.h>

class TextureUtil
{
    public:
        static MultiTexture readMultiTexture(
                const char* fileName);
    private:
        static Ogre::TexturePtr getRaw(
                const Ogre::String &textureName,
                unsigned int dim);
        static void fillFromRaw(
                Volume &volume,
                ZZIP_FILE *boxFile,
                unsigned int xdim,
                unsigned int ydim,
                unsigned int zdim);
        //static unsigned int getTextureDim(
                //unsigned int xdim,
                //unsigned int ydim,
                //unsigned int zdim);
        //static void calculateGradients(Volume &volume);
        //static void sampleGradients(Volume &volume);
        //static Ogre::Vector3 average(
                //Volume &volume,
                //unsigned int x,
                //unsigned int y,
                //unsigned int z);
};

#endif

