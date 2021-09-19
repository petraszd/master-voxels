#include <rttlistener.h>
#include <OgreMovableObject.h>
#include <OgreEntity.h>

using namespace Ogre;

RttListener::RttListener(Entity *dirCube, SceneNode *cubesNode) :
    m_dirCube(dirCube), m_cubes(cubesNode)
{
}

void RttListener::preRenderTargetUpdate(const RenderTargetEvent &event)
{
    //setMaterialName("Cube/FirstPass");
    setSceneVisible(false);
}

void RttListener::postRenderTargetUpdate(const RenderTargetEvent &event)
{
    //setMaterialName("Cube/SecondPass");
    setSceneVisible(true);
}

void RttListener::setSceneVisible(bool visible)
{
    for(unsigned int i = 0; i < m_cubes->numChildren(); ++i) {
        SceneNode *node = (SceneNode*)(m_cubes->getChild(i));
        for(unsigned int j = 0; j < node->numAttachedObjects(); ++j) {
            Entity *ent = (Entity*)(node->getAttachedObject(j));
            //ent->setMaterialName(name);
            ent->setVisible(visible);
        }
    }

    m_dirCube->setVisible(!visible);
}

