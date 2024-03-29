#ifndef BIOTRACKERPLUGIN_H
#define BIOTRACKERPLUGIN_H

#include "opencv2/core/core.hpp"
#include "Interfaces/IBioTrackerContext.h"

#include "Interfaces/IBioTrackerPlugin.h"

#include "QPointer"
#include "memory"
#include "QPoint"
#include "Config.h"

class Q_DECL_EXPORT BioTrackerPlugin : public IBioTrackerPlugin
{
    Q_OBJECT
    Q_INTERFACES(IBioTrackerPlugin)
    Q_PLUGIN_METADATA(IID IBioTrackerPlugin_iid FILE "plugin.json")

public:
    BioTrackerPlugin();
    ~BioTrackerPlugin();

    // IBioTrackerPlugin interface
    IView*                         getTrackerParameterWidget();
    IView*                         getTrackerElementsWidget();
    IModel*                        getTrackerComponentModel();
    IModelTrackedComponentFactory* getComponentFactory();

public:
    void init();
    void sendCorePermissions();

private:
    void connectInterfaces();
signals:
    void emitAreaDescriptorUpdate(IModelAreaDescriptor* areaDescr);
    void emitRemoveTrajectory(IModelTrackedTrajectory* trajectory);
    void emitAddTrajectory(QPoint pos);
    void emitRemoveTrajectoryId(int id);
    void emitValidateTrajectory(int id);
    void emitValidateEntity(IModelTrackedTrajectory* trajectory,
                            uint                     frameNumber);
    void emitRemoveTrackEntity(IModelTrackedTrajectory* trajectory,
                               uint                     frameNumber);
    void emitMoveElement(IModelTrackedTrajectory* trajectory,
                         uint                     frameNumber,
                         QPoint                   pos);
    void emitSwapIds(IModelTrackedTrajectory* trajectory0,
                     IModelTrackedTrajectory* trajectory1);
    void emitCurrentFrameNumber(uint frameNumber);
    void emitToggleFixTrack(IModelTrackedTrajectory* trajectory, bool toggle);
    void emitEntityRotation(IModelTrackedTrajectory* trajectory,
                            double                   angle,
                            uint                     frameNumber);

    void emitDimensionUpdate(int x, int y);

public slots:
    void receiveRemoveTrajectory(IModelTrackedTrajectory* trajectory);
    void receiveAddTrajectory(QPoint pos);
    void receiveSwapIds(IModelTrackedTrajectory* trajectory0,
                        IModelTrackedTrajectory* trajectory1);
    void receiveCurrentFrameNumberFromMainApp(uint frameNumber);

public slots:
    void receiveCurrentFrameFromMainApp(cv::Mat mat, uint frameNumber);
    void receiveAreaDescriptor(IModelAreaDescriptor* areaDescr);

private:
    IController* m_TrackerController;
    IController* m_ComponentController;
    IController* m_AreaDescrController;

    IBioTrackerContext* m_PluginContext;
    Config*             _cfg;

public:
    QList<ENUMS::COREPERMISSIONS> m_CorePermissions;
};

#endif // BIOTRACKERPLUGIN_H
