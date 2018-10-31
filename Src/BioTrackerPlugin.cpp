#include "BioTrackerPlugin.h"
#include "PluginContext.h"

#include "Controller/ControllerTrackingAlgorithm.h"
#include "Controller/ControllerTrackedComponent.h"

#include "Config.h"

#include "View/TrackedElementView.h"
#include "Utility/TrackedComponents/TrackedComponentFactory.h"

BioTrackerPlugin::BioTrackerPlugin() {
}

BioTrackerPlugin::~BioTrackerPlugin() {
    _cfg->save(Config::configLocation, "BackgroundSubtractionConfig.ini");
	delete _cfg;
}

IView* BioTrackerPlugin::getTrackerParameterWidget() {
	return qobject_cast<ControllerTrackingAlgorithm*> (m_TrackerController)->getTrackingParameterWidget();
}
IView *BioTrackerPlugin::getTrackerElementsWidget()
{
	return qobject_cast<ControllerTrackedComponent *> (m_ComponentController)->getTrackingElementsWidget();
}

// forwards tracked component model to coreApp
IModel* BioTrackerPlugin::getTrackerComponentModel() {
	return qobject_cast<ControllerTrackedComponent *> (m_ComponentController)->getModel();
}

IModelTrackedComponentFactory *BioTrackerPlugin::getComponentFactory() {
	return new TrackedComponentFactory();
}

void BioTrackerPlugin::createPlugin() {
	_cfg = new Config();
    _cfg->load(Config::configLocation, "BackgroundSubtractionConfig.ini");
	m_PluginContext = new PluginContext(this, _cfg);
	m_PluginContext->createApplication();

	IController * ctr = m_PluginContext->requestController(ENUMS::CONTROLLERTYPE::COMPONENT);
	m_ComponentController = qobject_cast<ControllerTrackedComponent *>(ctr);

	IController * ctr2 = m_PluginContext->requestController(ENUMS::CONTROLLERTYPE::TRACKING);
	m_TrackerController = qobject_cast<ControllerTrackingAlgorithm *>(ctr2);

	connectInterfaces();
}

void BioTrackerPlugin::connectInterfaces() {
	ControllerTrackingAlgorithm* ctrAlg = qobject_cast<ControllerTrackingAlgorithm*> (m_TrackerController);
	ControllerTrackedComponent* ctrTrC = qobject_cast<ControllerTrackedComponent*> (m_ComponentController);

	//controllertrackingalgorithm
	QObject::connect(ctrAlg, &ControllerTrackingAlgorithm::emitCvMat, this, &BioTrackerPlugin::receiveCvMatFromController);
	QObject::connect(ctrAlg, &ControllerTrackingAlgorithm::emitTrackingDone, this, &BioTrackerPlugin::receiveTrackingDone);
	QObject::connect(ctrAlg, &ControllerTrackingAlgorithm::emitChangeDisplayImage, this, &BioTrackerPlugin::receiveChangeDisplayImage);
	QObject::connect(this, &BioTrackerPlugin::emitAreaDescriptorUpdate, ctrAlg, &ControllerTrackingAlgorithm::receiveAreaDescriptorUpdate);
	//tracking algorithm
	QObject::connect(static_cast<BioTrackerTrackingAlgorithm*>(ctrAlg->getModel()), SIGNAL(emitDimensionUpdate(int, int)), this, SIGNAL(emitDimensionUpdate(int, int)));
	//controllertrackedcomponents
	QObject::connect(this, &BioTrackerPlugin::emitAddTrajectory, ctrTrC, &ControllerTrackedComponent::receiveAddTrajectory, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitRemoveTrajectory, ctrTrC, &ControllerTrackedComponent::receiveRemoveTrajectory, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitRemoveTrajectoryId, ctrTrC, &ControllerTrackedComponent::receiveRemoveTrajectoryId, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitRemoveTrackEntity, ctrTrC, &ControllerTrackedComponent::receiveRemoveTrackEntity, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitValidateTrajectory, ctrTrC, &ControllerTrackedComponent::receiveValidateTrajectory, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitValidateEntity, ctrTrC, &ControllerTrackedComponent::receiveValidateEntity, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitEntityRotation, ctrTrC, &ControllerTrackedComponent::receiveEntityRotation, Qt::DirectConnection);
	//connect this to enable moving of elements -> we need pxtocm() to create new poses
	QObject::connect(this, &BioTrackerPlugin::emitAreaDescriptorUpdate, ctrTrC, &ControllerTrackedComponent::receiveAreaDescriptorUpdate, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitMoveElement, ctrTrC, &ControllerTrackedComponent::receiveMoveElement, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitSwapIds, ctrTrC, &ControllerTrackedComponent::receiveSwapIds, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitToggleFixTrack, ctrTrC, &ControllerTrackedComponent::receiveToggleFixTrack, Qt::DirectConnection);
	QObject::connect(this, &BioTrackerPlugin::emitCurrentFrameNumber, ctrTrC, &ControllerTrackedComponent::receiveCurrentFrameNumber, Qt::DirectConnection);
}


void BioTrackerPlugin::receiveAreaDescriptor(IModelAreaDescriptor *areaDescr) {
	Q_EMIT emitAreaDescriptorUpdate(areaDescr);
}

void BioTrackerPlugin::receiveCurrentFrameFromMainApp(std::shared_ptr<cv::Mat> mat, uint frameNumber) {
	qobject_cast<ControllerTrackingAlgorithm*> (m_TrackerController)->doTracking(mat, frameNumber);

	Q_EMIT emitCurrentFrameNumber(frameNumber);
}

void BioTrackerPlugin::receiveCurrentFrameNumberFromMainApp(uint frameNumber) {
	Q_EMIT emitCurrentFrameNumber(frameNumber);
}

void BioTrackerPlugin::receiveCvMatFromController(std::shared_ptr<cv::Mat> mat, QString name) {
	Q_EMIT emitCvMat(mat, name);
}

void BioTrackerPlugin::receiveTrackingDone(uint framenumber) {
	Q_EMIT emitTrackingDone(framenumber);
}

void BioTrackerPlugin::receiveChangeDisplayImage(QString str) {
	Q_EMIT emitChangeDisplayImage(str);
}

void BioTrackerPlugin::receiveRemoveTrajectory(IModelTrackedTrajectory* trajectory) {
	Q_EMIT emitRemoveTrajectory(trajectory);
}

void BioTrackerPlugin::receiveAddTrajectory(QPoint pos) {
	Q_EMIT emitAddTrajectory(pos);
}

void BioTrackerPlugin::receiveSwapIds(IModelTrackedTrajectory * trajectory0, IModelTrackedTrajectory * trajectory1) {
	Q_EMIT emitSwapIds(trajectory0, trajectory1);
}

void BioTrackerPlugin::sendCorePermissions() {
	// get plugin settings

	// signal permissions
	Q_EMIT emitCorePermission(std::pair<ENUMS::COREPERMISSIONS, bool>(ENUMS::COREPERMISSIONS::COMPONENTVIEW, _cfg->EnableView));
	Q_EMIT emitCorePermission(std::pair<ENUMS::COREPERMISSIONS, bool>(ENUMS::COREPERMISSIONS::COMPONENTMOVE, _cfg->EnableMove));
	Q_EMIT emitCorePermission(std::pair<ENUMS::COREPERMISSIONS, bool>(ENUMS::COREPERMISSIONS::COMPONENTREMOVE, _cfg->EnableRemove));
	Q_EMIT emitCorePermission(std::pair<ENUMS::COREPERMISSIONS, bool>(ENUMS::COREPERMISSIONS::COMPONENTSWAP, _cfg->EnableSwap));
	Q_EMIT emitCorePermission(std::pair<ENUMS::COREPERMISSIONS, bool>(ENUMS::COREPERMISSIONS::COMPONENTADD, _cfg->EnableAdd));
	Q_EMIT emitCorePermission(std::pair<ENUMS::COREPERMISSIONS, bool>(ENUMS::COREPERMISSIONS::COMPONENTROTATE, _cfg->EnableRotate));
}

