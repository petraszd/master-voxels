#include <textureutil.h>
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>

#include <iostream>
#include <cmath>
#include <volume.h>
#include <config.h>

#include <QVariant>
#include <QDebug>

using namespace Ogre;

MultiTexture TextureUtil::readMultiTexture(
        const char *archName)
{
    MultiTexture result;

    zzip_error_t zErr;
    ZZIP_DIR *zDir = zzip_dir_open(archName, &zErr);
    if (zErr != ZZIP_NO_ERROR) {
        return result;
    }

    ZZIP_FILE *zInfoFile = zzip_file_open(zDir, "info.txt", ZZIP_ONLYZIP);
    if (!zInfoFile) {
        zzip_dir_close(zDir);
        return result;
    }

    ZZIP_STAT zStat;
    zzip_file_stat(zInfoFile, &zStat);
    unsigned int infoSize = zStat.st_size;
    char infoBuf[infoSize];
    zzip_file_read(zInfoFile, infoBuf, infoSize);

    result.extractInfo(infoBuf, infoSize);

    for (unsigned int i = 0; i < result.texturesCount(); ++i) {
        MultiTextureFileInfo info = result.at(i);
        if (!info.isValid())
            continue;

        ZZIP_FILE *zBoxFile = zzip_file_open(zDir, info.fileName.c_str(), ZZIP_ONLYZIP);
        if (!zInfoFile)
            continue;
        unsigned int texDim = info.getMaxDim();
        TexturePtr tex = TextureUtil::getRaw(info.textureName(), texDim);
        HardwarePixelBufferSharedPtr hdBuffer = tex->getBuffer();
        hdBuffer->lock(HardwareBuffer::HBL_NORMAL);
        Volume volume(hdBuffer->getCurrentLock());
        fillFromRaw(volume, zBoxFile, info.width, info.height, info.depth);
        hdBuffer->unlock();
        zzip_file_close(zBoxFile);
    }
    zzip_file_close(zInfoFile);
    zzip_dir_close(zDir);

    return result;
}

void TextureUtil::fillFromRaw(
        Volume &volume,
        ZZIP_FILE *boxFile,
        unsigned int xdim,
        unsigned int ydim,
        unsigned int zdim)
{
    uint8 buffer[4];
    for (unsigned int z = 0; z < zdim; ++z) {
        for (unsigned int y = 0; y < ydim; ++y) {
            for (unsigned int x = 0; x < xdim; ++x) {
                zzip_file_read(boxFile, buffer, 4);
                volume.elem(x, y, z).setRGBA(
                    buffer[0], buffer[1],
                    buffer[2], buffer[3]
                );
            }
        }
    }
}

//void TextureUtil::sampleGradients(Volume &volume)
//{
    //unsigned int index = 0;
    //Vector3 *samples = new Vector3[volume.depth() * volume.width() * volume.height()];

    //// Too slow...
    //for (unsigned int z = 0; z < volume.depth(); ++z) {
        //for (unsigned int y = 0; y < volume.height(); ++y) {
            //for (unsigned int x = 0; x < volume.width(); ++x) {
                //samples[index++] = average(volume, x, y, z);
            //}
        //}
    //}

    //index = 0;
    //for (unsigned int z = 0; z < volume.depth(); ++z) {
        //for (unsigned int y = 0; y < volume.height(); ++y) {
            //for (unsigned int x = 0; x < volume.width(); ++x) {
                //volume.elem(x, y, z).setRGB(
                        //255 * samples[index].x,
                        //255 * samples[index].y,
                        //255 * samples[index].z);
                //index++;
            //}
        //}
    //}

    //delete []samples;
//}

//Vector3 TextureUtil::average(Volume &volume,
                //unsigned int x,
                //unsigned int y,
                //unsigned int z)
//{
    //unsigned int n = Config::getUInt("textureutil/average_len");
    //Vector3 normal = Vector3::ZERO;
    //for (unsigned int k = z - n; k < z + n; ++k) {
        //if (k >= volume.depth())
            //continue;
        //for (unsigned int j = y - n; j < y + n; ++j) {
            //if (j >= volume.height())
                //continue;
            //for (unsigned int i = x - n; i < x + n; ++i) {
                //if (i >= volume.width())
                    //continue;
                //VolumeElem el = volume.elem(i, j, k);
                //normal += Vector3(el.r, el.g, el.b);
            //}
        //}
    //}

    //if (!normal.isZeroLength()) {
        //normal.normalise();
    //}
    //return normal;
//}

TexturePtr TextureUtil::getRaw(const String &textureName,
        unsigned int dim)
{
    TextureManager *mgr = TextureManager::getSingletonPtr();
    mgr->remove(textureName);

    return mgr->createManual(textureName,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            TEX_TYPE_3D,
            dim, dim, dim, // width, height, depth
            0, // Number of mipmaps
            PF_BYTE_RGBA,
            TU_DYNAMIC);
}

