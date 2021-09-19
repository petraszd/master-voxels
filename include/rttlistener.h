#ifndef _RTTLISTENER_H_
#define _RTTLISTENER_H_

#include <OgreRenderTargetListener.h>
#include <OgreSceneNode.h>

class RttListener : public Ogre::RenderTargetListener
{
    protected:
        Ogre::Entity *m_dirCube;
        Ogre::SceneNode *m_cubes;

    public:
        RttListener(Ogre::Entity *dirCube, Ogre::SceneNode *cubesNode);
        virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent &event);
        virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent &event);

    protected:
        void setSceneVisible(bool visible);
};

#endif

