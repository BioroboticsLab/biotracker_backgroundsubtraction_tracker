#ifndef CONTROLLERTRACKINGALGORITHM_H
#define CONTROLLERTRACKINGALGORITHM_H

#include "Interfaces/IController/IController.h"
#include "../Model/BioTrackerTrackingAlgorithm.h"
#include "Interfaces/IBioTrackerContext.h"
#include "Interfaces/IModel/IModelDataExporter.h"
#include "../Config.h"


class ControllerTrackingAlgorithm : public IController
{
    Q_OBJECT
public:
    ControllerTrackingAlgorithm(QObject* parent = 0, IBioTrackerContext* context = 0, ENUMS::CONTROLLERTYPE ctr = ENUMS::CONTROLLERTYPE::NO_CTR);

    Config* getConfig() { return _cfg;};
    void setConfig(Config *cfg) { _cfg = cfg;};
    // IController interface
public:
    void connectControllerToController() override;

    void doTracking(std::shared_ptr<cv::Mat> mat, uint number);

    IView* getTrackingParameterWidget();

public Q_SLOTS:
	void receiveAreaDescriptorUpdate(IModelAreaDescriptor *areaDescr);

protected:
    void createModel() override;
    void createView() override;
    void connectModelToController() override;

Q_SIGNALS:
    void emitCvMat(std::shared_ptr<cv::Mat> mat, QString name);
    void emitTrackingDone(uint framenumber);
	void emitChangeDisplayImage(QString str);
	void emitAreaDescriptorUpdate(IModelAreaDescriptor *areaDescr);

private:
    IModel* m_TrackingParameter;
	IModel* m_TrackedTrajectoryMajor;
    Config* _cfg;
};

#endif // CONTROLLERTRACKINGALGORITHM_H
