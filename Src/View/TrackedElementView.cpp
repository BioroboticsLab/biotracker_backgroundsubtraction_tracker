#include "TrackedElementView.h"
#include "../Model/TrackedComponents/TrackedElement.h"
#include "../Model/TrackedComponents/TrackedTrajectory.h"
#include "QBrush"
#include "QPainter"
#include "QGraphicsScene"
#include <QGraphicsSceneHoverEvent>
#include "Utility/misc.h"
#include "QDebug"

class QGraphicsSceneHoverEvent;

TrackedElementView::TrackedElementView(QGraphicsItem* parent,
                                       IController*   controller,
                                       IModel*        model)
: IViewTrackedComponent(parent, controller, model)
{
    BST::TrackedElement* elem = dynamic_cast<BST::TrackedElement*>(getModel());
    _boundingRect             = QRectF(0, 0, 100, 100);
}

void TrackedElementView::rcvDimensionUpdate(int x, int y)
{
    _boundingRect = QRectF(0, 0, x, y);
    update();
}

QRectF TrackedElementView::boundingRect() const
{

    return _boundingRect;
}

void TrackedElementView::paint(QPainter*                       painter,
                               const QStyleOptionGraphicsItem* option,
                               QWidget*                        widget)
{
    QGraphicsScene* scene = this->scene();

    // Dev note: You could do some meaningful drawing here.
    // So far, the core does everything we need.
}

void TrackedElementView::getNotified()
{
    update();
}
