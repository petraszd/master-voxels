#ifndef _CUBEAPP_H_
#define _CUBEAPP_H_

#include <rttlistener.h>
#include <ogreapp.h>
#include <OgreString.h>
#include <OgreSceneManager.h>

class CubeApp : public OgreApp
{
    private:
        RttListener *m_rttListener;
        Ogre::RenderTexture *m_renderTex;
        Ogre::SceneManager *m_sMgr;
        Ogre::SceneNode *m_camNode;
        Ogre::SceneNode *m_cubesNode;
        Ogre::SceneNode *m_visibleCubes;
        Ogre::Entity *m_dirCube;
        Ogre::Vector3 m_dir;
        Ogre::TexturePtr m_transferTex;

        bool m_rotateAround;

        float m_shaderFactor;
        float m_shaderAlpha;
        bool m_shaderUseLight;

    public:
        CubeApp();
        ~CubeApp();

        void mouse(int x, int y);

        void downOn();
        void downOff();
        void upOn();
        void upOff();
        void leftOn();
        void leftOff();
        void rightOn();
        void rightOff();

        void open3DTexture(const QString &name);
        void shoot();

        void useLight(bool use);
        void setAlpha(Ogre::Real alpha);
        void setDiffuseFactor(Ogre::Real factor);
        void setTransferTexData(Ogre::uchar *data);

        void toggleRotateAround();
    protected:
        virtual void customInit();
        virtual void resized(unsigned int w, unsigned int h);
        virtual void animate(int milisecs);

        void createSceneManager();
        void createCamera();
        void createScene();
        void showOverlays();
        void createRtt();
        void createColorsTexure();
        void createTransferTexture();
        Ogre::Pass* createVisibleCube(
                const Ogre::String &name,
                Ogre::SceneNode *parentNode);

        void assignRTTTexture();
        void setCubesShaderValue(const Ogre::String &name,
                Ogre::Real value);
        void clearVisibleCubes();
};

#endif

