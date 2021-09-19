#ifndef _OGREWIDGET_H_
#define _OGREWIDGET_H_

#include <dialogloglistener.h>
#include <QString>
#include <QPixmap>
#include <QPoint>
#include <QWidget>

class CubeApp;

class OgreWidget : public QWidget
{
    Q_OBJECT
    protected:
        CubeApp *m_cubeapp;
        QPoint m_mousePos;
        DialogLogListener *m_logDialog;
    public:
        OgreWidget(QWidget *parent=0);
        virtual ~OgreWidget();

    public slots:
        void setFilename(const QString& name);
        void toggleLog();
        void toggleLight(bool turnOn);
        void changeAlpha(double alpha);
        void changeTransfer(const QPixmap &pixmap);
        void changeDiffuseFactor(double factor);

    protected:
        void startOgreApp();

        virtual void paintEvent(QPaintEvent *event);
        virtual void resizeEvent(QResizeEvent *event);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);

        virtual QPaintEngine* paintEngine() const {
            return 0;
        }

    signals:
        void fpsChanged(const QString & value);
};

#endif

