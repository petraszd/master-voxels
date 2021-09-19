#include <multitexture.h>
#include <zzip/lib.h>

#include <QTextStream>
#include <QByteArray>
#include <QString>
#include <QDebug>

MultiTexture::MultiTexture()
{
    m_dimension = 0;
}

void MultiTexture::extractInfo(const char* info, unsigned int nInfo)
{
    QTextStream stream(QByteArray(info, nInfo), QIODevice::ReadOnly);

    stream >> m_dimension;

    unsigned int nFiles = m_dimension * m_dimension * m_dimension;

    for (unsigned int i = 0; i < nFiles; ++i) {
        MultiTextureFileInfo info;
        stream >> info.width >> info.height >> info.depth;
        info.fileName = stream.readLine().trimmed().toStdString();
        m_textures.append(info);
    }
}

