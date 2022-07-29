#include "PluginContext.h"

#include "Controller/ControllerTrackingAlgorithm.h"
#include "Controller/ControllerTrackedComponent.h"

PluginContext::PluginContext(QObject* parent, Config* cfg)
: IBioTrackerContext(parent)
{
    _cfg = cfg;
    ControllerTrackedComponent* ComponentController =
        new ControllerTrackedComponent(this,
                                       this,
                                       ENUMS::CONTROLLERTYPE::COMPONENT);
    ControllerTrackingAlgorithm* TrackingController =
        new ControllerTrackingAlgorithm(this,
                                        this,
                                        ENUMS::CONTROLLERTYPE::TRACKING);
    ComponentController->setConfig(_cfg);
    TrackingController->setConfig(_cfg);

    m_ControllersMap.insert(ENUMS::CONTROLLERTYPE::COMPONENT,
                            ComponentController);
    m_ControllersMap.insert(ENUMS::CONTROLLERTYPE::TRACKING,
                            TrackingController);
}

void PluginContext::createAppController()
{
    QMap<ENUMS::CONTROLLERTYPE, IController*>::iterator i;
    for (i = m_ControllersMap.begin(); i != m_ControllersMap.end(); ++i) {
        i.value()->createComponents();
    }
}

void PluginContext::connectController()
{
    QMap<ENUMS::CONTROLLERTYPE, IController*>::iterator i;
    for (i = m_ControllersMap.begin(); i != m_ControllersMap.end(); ++i) {
        i.value()->connectComponents();
    }
}
