#include "TrackedComponentFactory.h"

#include "TrackedTrajectory.h"
#include "TrackedElement.h"

namespace BST
{

    TrackedComponentFactory::TrackedComponentFactory()
    {
    }

    TrackedComponentFactory::~TrackedComponentFactory()
    {
    }

    QList<QString> TrackedComponentFactory::getElementTypes()
    {
        return QList<QString>{"TrackedElement"};
    }

    IModelTrackedComponent* TrackedComponentFactory::createTrackedElement(
        QString name)
    {
        return new BST::TrackedElement(this, "n.a.");
    }

    IModelTrackedComponent* TrackedComponentFactory::createTrackedObject(
        QString name)
    {
        BST::TrackedTrajectory* t = new BST::TrackedTrajectory();
        BST::TrackedElement*    e = new BST::TrackedElement(this, "n.a.", 0);
        t->add(e, 0);
        return t;
    }

    IModelTrackedComponent* TrackedComponentFactory::createTrackedTrajectory(
        QString name)
    {
        return new BST::TrackedTrajectory();
    }
}
