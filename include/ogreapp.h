#ifndef _OGREAPP_H_
#define _OGREAPP_H_

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <QTime>

class OgreApp
{
    protected:
        Ogre::Root *m_root;
        Ogre::RenderWindow *m_window;
        QTime m_time;
    public:
        OgreApp();
        virtual ~OgreApp();

        void start(
                unsigned long display,
                unsigned int screen,
                unsigned long winId,
                int width, int height);
        bool started();
        void renderOneFrame();
        void resize(unsigned int w, unsigned int h);

        float getFPS() const;

    protected:
        virtual void defineResources();
        void initWindow(
                unsigned long display,
                unsigned int screen,
                unsigned long winId,
                int w, int h);
        virtual void initResources();
        virtual void resized(unsigned int w, unsigned int h) {
        }
        virtual void customInit() {
        }
        virtual void animate(int milisecs) {
        }

        virtual const char* getResourcesFileName() {
            return "resources.cfg";
        }
};

#endif

