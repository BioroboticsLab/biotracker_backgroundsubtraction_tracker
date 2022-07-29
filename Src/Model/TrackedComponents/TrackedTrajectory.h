#pragma once

#include "Utility/TrackedComponents/TrackedTrajectory.h"
#include "QList"
#include "QString"

namespace BST
{
    /**
     * This class inherits from the IModelTrackedTrajectory class and is
     * therefor part of the Composite Pattern. This class represents the
     * Composite class. This class is responsibility for the handling of Leaf
     * objects. Internaly this class uses a QList for storing Leaf object.
     *
     * Objects of this class have a QObject as parent.
     */
    class TrackedTrajectory : public ::TrackedTrajectory
    {
        Q_OBJECT

    public:
        TrackedTrajectory(QObject* parent = 0, QString name = "n.a.");
        ~TrackedTrajectory() override{};

        // ITrackedComponent interface
    public:
        void operate();

        // ITrackedObject interface
    public:
        void add(IModelTrackedComponent* comp, int pos = -1) override;
        bool remove(IModelTrackedComponent* comp) override;
        void clear() override;
        IModelTrackedComponent* getChild(int index) override;
        IModelTrackedComponent* getValidChild(int index) override;
        IModelTrackedComponent* getLastChild() override;
        int                     size() override;
        int                     validCount() override;
        void                    setValid(bool v) override;
        void                    triggerRecalcValid();

    private:
        int     g_calcValid  = 1;
        int     g_validCount = 0;
        int     _size        = 0;
        QString name;
    };

}
