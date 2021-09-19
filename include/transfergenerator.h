#ifndef _TRANSFERGENERETOR_H_
#define _TRANSFERGENERETOR_H_

#include <QGraphicsView>
#include <QRectF>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QPixmap>
#include <QLinearGradient>

class TransferKnob: public QGraphicsRectItem
{
    protected:
        struct Bound {
            qreal min;
            qreal max;

            bool isValid(qreal x) {
                return min <= x && x <= max;
            }
        } m_bounds;

    public:
        TransferKnob(const QRectF &rect,
                QGraphicsItem *parent = 0);
        ~TransferKnob();

    protected:
        // Need this method, because default implementation
        // prevents double click event
        virtual void mousePressEvent(
                QGraphicsSceneMouseEvent *event) {}

        virtual void mouseDoubleClickEvent(
                QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent(
                QGraphicsSceneMouseEvent *event);

        void resetPen();
};

class TransferScene: public QGraphicsScene
{
    Q_OBJECT

    public:
        TransferScene(const QRectF &sceneRect,
                QObject *parent = 0);
        ~TransferScene();
    protected:
        virtual void mouseDoubleClickEvent(
                QGraphicsSceneMouseEvent *mouseEvent);
        virtual void drawBackground(QPainter *painter,
                const QRectF &rect);
        QLinearGradient calculateGradient();

    signals:
        void changed(const QPixmap&);
};


class TransferGenerator : public QGraphicsView
{
    Q_OBJECT

    protected:
        TransferScene *m_scene;
    public:
        TransferGenerator(QWidget *parent = 0);
        virtual ~TransferGenerator();

    protected:
        virtual void resizeEvent(QResizeEvent *event);

    signals:
        void changed(const QPixmap&);
};

#endif

