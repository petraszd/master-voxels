#include <dialogloglistener.h>
#include <QHBoxLayout>

DialogLogListener::DialogLogListener(
        QWidget *parent, Qt::WindowFlags f
        ) : QDialog(parent, f), m_log(0)
{
    Ogre::LogManager *logMgr = new Ogre::LogManager();
    logMgr->setLogDetail(Ogre::LL_BOREME);
    m_log = logMgr->createLog("Ogre.log", true, false, false);

    m_log->addListener(this);

    QHBoxLayout *layout = new QHBoxLayout();

    m_edit = new QTextEdit(this);
    m_edit->setReadOnly(true);

    layout->addWidget(m_edit);

    setLayout(layout);
    setGeometry(0, 0, 800, 600);
}

DialogLogListener::~DialogLogListener()
{
    m_log->removeListener(this);
}

void DialogLogListener::messageLogged(
                const Ogre::String &message,
                Ogre::LogMessageLevel lml,
                bool maskDebug,
                const Ogre::String &logName)
{
    switch (lml) {
        case Ogre::LML_TRIVIAL:
            m_edit->setFontWeight(QFont::Normal);
            m_edit->setTextColor(QColor(128, 128, 128));
            break;
        case Ogre::LML_NORMAL:
            m_edit->setFontWeight(QFont::Normal);
            m_edit->setTextColor(QColor(0, 0, 0));
            break;
        case Ogre::LML_CRITICAL:
            m_edit->setFontWeight(QFont::Bold);
            m_edit->setTextColor(QColor(172, 0, 0));
            qCritical(message.c_str());
            break;
    }
    m_edit->append(message.c_str());
}

