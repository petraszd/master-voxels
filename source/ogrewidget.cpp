#include <ogrewidget.h>
#include <cubeapp.h>
#ifdef Q_WS_X11
    #include <QX11Info>
#endif
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>

OgreWidget::OgreWidget(QWidget *parent) :
    QWidget(parent), m_cubeapp(new CubeApp()), m_logDialog(0)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start();

    m_mousePos = QPoint(0, 0);

    setAttribute(Qt::WA_NativeWindow, true);
    setFocusPolicy(Qt::StrongFocus);
}

OgreWidget::~OgreWidget()
{
    delete m_logDialog;
    delete m_cubeapp;
}

void OgreWidget::paintEvent(QPaintEvent *event)
{
    if (!m_cubeapp->started()) {
        startOgreApp();
    }
    m_cubeapp->renderOneFrame();

    emit fpsChanged(QString("%1").arg(m_cubeapp->getFPS(), 0, 'f', 2));
}

void OgreWidget::resizeEvent(QResizeEvent *event)
{
    if (m_cubeapp->started()) {
        m_cubeapp->resize(width(), height());
    }
}

void OgreWidget::startOgreApp()
{
    // Ogre Log Listener must be inited before Ogre::Root
    m_logDialog = new DialogLogListener(this, Qt::Dialog);

#ifdef Q_WS_X11
    const QX11Info  &x11 = x11Info();
    m_cubeapp->start(
            (unsigned long)x11.display(),
            (unsigned int)x11.screen(),
            (unsigned long)winId(),
            width(), height());
#endif
#ifndef Q_WS_X11
    m_cubeapp->start(0, 0, (unsigned long)winId(), width(), height());
#endif
}

void OgreWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QWidget::keyPressEvent(event);
        return;
    }
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            m_cubeapp->upOn();
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            m_cubeapp->downOn();
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            m_cubeapp->leftOn();
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            m_cubeapp->rightOn();
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void OgreWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QWidget::keyReleaseEvent(event);
        return;
    }
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            m_cubeapp->upOff();
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            m_cubeapp->downOff();
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            m_cubeapp->leftOff();
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            m_cubeapp->rightOff();
            break;
        case Qt::Key_R:
            m_cubeapp->toggleRotateAround();
            break;
        default:
            QWidget::keyReleaseEvent(event);
            break;
    }
}

void OgreWidget::mousePressEvent(QMouseEvent *event)
{
    if (Qt::RightButton == event->buttons()) {
        m_cubeapp->shoot();
    }
    m_mousePos = event->pos();
}

void OgreWidget::mouseMoveEvent(QMouseEvent *event)
{
    int x = m_mousePos.x() - event->pos().x();
    int y = m_mousePos.y() - event->pos().y();
    m_cubeapp->mouse(x, y);
    m_mousePos = event->pos();
}

void OgreWidget::setFilename(const QString& name)
{
    m_cubeapp->open3DTexture(name);
}

void OgreWidget::toggleLog()
{
    if (m_logDialog->isVisible())
        m_logDialog->hide();
    else
        m_logDialog->show();
}

void OgreWidget::toggleLight(bool turnOn)
{
    m_cubeapp->useLight(turnOn);
}

void OgreWidget::changeAlpha(double alpha)
{
    m_cubeapp->setAlpha(alpha);
}

void OgreWidget::changeTransfer(const QPixmap &pixmap)
{
    m_cubeapp->setTransferTexData(pixmap.toImage().scanLine(0));
}


void OgreWidget::changeDiffuseFactor(double factor)
{
    m_cubeapp->setDiffuseFactor(factor);
}

