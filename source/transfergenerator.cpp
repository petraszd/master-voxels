#include <config.h>
#include <transfergenerator.h>
#include <QColorDialog>
#include <QLinearGradient>
#include <QDebug>

// ----
// Knob
// ----
TransferKnob::TransferKnob(const QRectF &rect,
        QGraphicsItem *parent) :
    QGraphicsRectItem(rect, parent)
{
    double hue = qrand() / double(RAND_MAX);
    setBrush(QColor::fromHsvF(hue, 1.0, 1.0, 1.0));
    resetPen();
    //setCursor(QCursor(Qt::SizeHorCursor));

    m_bounds.min = 0.0;
    m_bounds.max = 1.0;
}

TransferKnob::~TransferKnob()
{
}

void TransferKnob::mouseDoubleClickEvent(
        QGraphicsSceneMouseEvent *event)
{
    QColor newCol = QColorDialog::getColor(
                brush().color(), 0, "Select Color",
                QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
    if (!newCol.isValid())
        return;
    setBrush(QBrush(newCol));
    resetPen();
}

void TransferKnob::mouseMoveEvent(
        QGraphicsSceneMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (m_bounds.isValid(event->scenePos().x())) {
        setPos(QPointF(event->scenePos().x(), pos().y()));
    }
}

void TransferKnob::resetPen()
{
    setPen(QPen(brush().color().valueF() > 0.5 ? Qt::black : Qt::white ));
}


// -----
// Scene
// -----
TransferScene::TransferScene(const QRectF &sceneRect,
        QObject *parent) :
    QGraphicsScene(sceneRect, parent)
{
}

TransferScene::~TransferScene()
{
}

void TransferScene::mouseDoubleClickEvent(
        QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem*> knobs = items(event->scenePos());
    if(knobs.size() != 0) {
        QGraphicsScene::mouseDoubleClickEvent(event);
        return;
    }

    TransferKnob *knob = new TransferKnob(
            QRectF(-0.01, 0.0, 0.02, 1.0), NULL);
    knob->setPos(event->scenePos().x(), 0.0);
    addItem(knob);
}

void TransferScene::drawBackground(QPainter *painter,
        const QRectF &rect)
{
    QGraphicsView *view = views().first();
    painter->save();
    painter->scale(1.0 / view->width(), 1.0 / view->height());

    painter->setBrush(QBrush(QPixmap(QString(":/back.png"))));
    painter->drawRect(QRect(0, 0, view->width(), view->height()));

    painter->restore();

    QBrush withGradient(calculateGradient());
    painter->fillRect(rect, withGradient);

    QPixmap pixmap(Config::getUInt("transfer/size"), 1);
    pixmap.fill(QColor(0, 0, 0, 0));

    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    pixmapPainter.scale(pixmap.width(), 1.0);
    pixmapPainter.fillRect(rect, withGradient);
    pixmapPainter.end();
    emit changed(pixmap);
}

bool knobsCmp(QGraphicsItem *item1, QGraphicsItem *item2)
{
    return item1->pos().x() < item2->pos().x();
}

QLinearGradient TransferScene::calculateGradient()
{
    QList<QGraphicsItem*> knobs = items();
    qSort(knobs.begin(), knobs.end(), knobsCmp);

    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0.0, QColor::fromRgbF(1.0, 1.0, 1.0, 0.0));
    foreach(QGraphicsItem *item, knobs) {
        TransferKnob *knob = static_cast<TransferKnob*>(item);
        gradient.setColorAt(knob->pos().x(), knob->brush().color());
    }
    if (!knobs.size() || knobs.last()->pos().x() != 1.0)
        gradient.setColorAt(1.0, QColor::fromRgbF(1.0, 1.0, 1.0, 0.0));

    return gradient;
}

// ----
// View
// ----
TransferGenerator::TransferGenerator(QWidget *parent) :
    QGraphicsView(parent), m_scene(0)
{
    m_scene = new TransferScene(QRectF(0.0, 0.0, 1.0, 1.0), this);
    setScene(m_scene);

    connect(m_scene, SIGNAL(changed(QPixmap)),
            this, SIGNAL(changed(QPixmap)));

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

TransferGenerator::~TransferGenerator()
{
    delete m_scene;
}

void TransferGenerator::resizeEvent(QResizeEvent *event)
{
    fitInView(m_scene->sceneRect());
}

