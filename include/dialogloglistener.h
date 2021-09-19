#ifndef _DIALOGLOGLISTENER_H_
#define _DIALOGLOGLISTENER_H_

#include <OgreLogManager.h>
#include <QDialog>
#include <QTextEdit>

class DialogLogListener : public QDialog, public Ogre::LogListener
{
    protected:
        QTextEdit *m_edit;
        Ogre::Log *m_log;
    public:
        DialogLogListener(QWidget *parent=0, Qt::WindowFlags f=0);
        ~DialogLogListener();

        void messageLogged(
                const Ogre::String &message,
                Ogre::LogMessageLevel lml,
                bool maskDebug,
                const Ogre::String &logName);
};

#endif

