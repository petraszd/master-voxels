#include <volume.h>

Volume::Volume(const Ogre::PixelBox &box)
{
    m_elems = static_cast<VolumeElem*>(box.data);
    m_w = box.getWidth();
    m_h = box.getHeight();
    m_d = box.getDepth();
}

Volume::Volume(Ogre::uint8 *raw,
               unsigned int width,
               unsigned int height,
               unsigned int depth)
{
    m_elems = (VolumeElem*)&raw[0];
    m_w = width;
    m_h = height;
    m_d = depth;
}

bool Volume::hasElem(unsigned int x,
                     unsigned int y,
                     unsigned int z)
{
    return x < m_w && y < m_h && z < m_d;
}


void Volume::hide(unsigned int x,
          unsigned int y,
          unsigned int z,
          int radius)
{
    if (!hasElem(x + radius, y + radius, z + radius))
        return;
    if (!hasElem(x - radius, y - radius, z - radius))
        return;

    for (unsigned int iz = z - radius; iz <= z + radius; ++iz) {
        for (unsigned int iy = y - radius; iy <= y + radius; ++iy) {
            for (unsigned int ix = x - radius; ix <= x + radius; ++ix) {
                elem(ix, iy, iz).a = 0;
            }
        }
    }
}

