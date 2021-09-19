#include <config.h>
#include <OgreConfigFile.h>
#include <OgreRenderSystem.h>
#include <OgreResourceGroupManager.h>
#include <OgreViewport.h>
#include <OgreLogManager.h>
#include <OgreEntity.h>
#include <ogreapp.h>
#include <QString>

using namespace Ogre;

OgreApp::OgreApp() :
    m_root(0), m_window(0)
{
}

OgreApp::~OgreApp()
{
    delete m_root;
}

bool OgreApp::started()
{
    return m_root != 0;
}

void OgreApp::start(
        unsigned long display,
        unsigned int screen,
        unsigned long winId,
        int width, int height)
{
    m_root = new Root();

    defineResources();
    initWindow(display, screen, winId, width, height);
    initResources();

    customInit();

    m_time.start();
}

void OgreApp::renderOneFrame()
{
    m_root->_fireFrameStarted();

    m_root->renderOneFrame();

    m_root->_fireFrameRenderingQueued();
    m_root->_fireFrameEnded();

    animate(m_time.restart());
}

void OgreApp::defineResources()
{
    String secName, typeName, archName;
    ConfigFile cf;
    cf.load(getResourcesFileName());
    ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings =
            seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;

        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton()
                .addResourceLocation(archName, typeName, secName);
        }
    }
}

void OgreApp::initWindow(
        unsigned long display,
        unsigned int screen,
        unsigned long winId,
        int w, int h)
{
    // System
    RenderSystemList list = m_root->getAvailableRenderers();
    assert(list.size() > 0);

    RenderSystem *system = list.at(0);
    m_root->setRenderSystem(system);

    // Root
    m_root->initialise(false);

    // Window
#ifdef Q_WS_X11
    QString winHandle = QString("%1:%2:%3")
        .arg(display).arg(screen).arg(winId);
#endif
#ifndef Q_WS_X11
    QString winHandle = QString("%1").arg(winId);
#endif
    NameValuePairList pairs;
    pairs["parentWindowHandle"] = winHandle.toStdString();

    m_window = m_root->createRenderWindow(
            "RenderWindow" /* Name */,
            w, h, false /* Fullscreen */, &pairs);
    m_window->reposition(0, 0);
    m_window->setActive(true);
    m_window->setVisible(true);
}

void OgreApp::initResources()
{
    TextureManager::getSingleton()
        .setDefaultNumMipmaps(Config::getUInt("ogreapp/mipmaps"));

    ResourceGroupManager::getSingleton()
        .initialiseAllResourceGroups();
}

void OgreApp::resize(unsigned int w, unsigned int h)
{
    m_window->resize(w, h);
    m_window->reposition(0, 0);
    m_window->windowMovedOrResized();
    unsigned int i;
    for (i = 0; i < m_window->getNumViewports(); ++i) {
        Viewport *vp = m_window->getViewport(i);
        Camera *cam = vp->getCamera();
        Real ratio = (Real)vp->getActualWidth()
            / (Real)vp->getActualHeight();
        cam->setAspectRatio(ratio);
    }
    resized(m_window->getWidth(), m_window->getHeight());
}


float OgreApp::getFPS() const
{
    return m_window->getLastFPS();
}

