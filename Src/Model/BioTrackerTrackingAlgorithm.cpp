#include "BioTrackerTrackingAlgorithm.h"
#include <future>
#include "TrackedComponents/TrackedComponentFactory.h"
#include <chrono>

#include <tuple>
#include <optional>

BioTrackerTrackingAlgorithm::BioTrackerTrackingAlgorithm(IController* parent,
                                                         IModel* parameter,
                                                         IModel* trajectory)
: IModelTrackingAlgorithm(parent)
, _ipp((TrackerParameter*) parameter)
, _lastImage(std::nullopt)
{
    _cfg = static_cast<ControllerTrackingAlgorithm*>(parent)->getConfig();
    _TrackingParameter      = (TrackerParameter*) parameter;
    _TrackedTrajectoryMajor = (BST::TrackedTrajectory*) trajectory;
    _nn2d = std::make_shared<NN2dMapper>(_TrackedTrajectoryMajor);

    _bd = BlobsDetector();

    _noFish = -1;

    if (_cfg->DoNetwork) {
        _listener = new TcpListener(this);
        _listener->listen(QHostAddress::Any, _cfg->NetworkPort);
        QObject::connect(_listener,
                         SIGNAL(newConnection()),
                         _listener,
                         SLOT(acceptConnection()));
    }

    _lastFramenumber = -1;
}

void BioTrackerTrackingAlgorithm::receiveAreaDescriptorUpdate(
    IModelAreaDescriptor* areaDescr)
{
    _AreaInfo = areaDescr;
    _bd.setAreaInfo(_AreaInfo);
}

BioTrackerTrackingAlgorithm::~BioTrackerTrackingAlgorithm()
{
}

std::vector<FishPose> BioTrackerTrackingAlgorithm::getLastPositionsAsPose()
{
    // TODO: This seems kinda fragile: I just assume that the tree has this
    // very certain structure:
    // Trajectory -> M Trajectories -> N TrackedElements
    // For every of M Trajectories grab the last (highest index) of
    // TrackedElements.
    // TODO: If we are tracking somewhere in the middle, this is bad. Do it by
    // id!
    std::vector<FishPose> last;
    for (int i = 0; i < _TrackedTrajectoryMajor->size(); i++) {
        BST::TrackedTrajectory* t = dynamic_cast<BST::TrackedTrajectory*>(
            _TrackedTrajectoryMajor->getChild(i));
        if (t && t->getValid() && !t->getFixed()) {
            BST::TrackedElement* e = (BST::TrackedElement*) t->getLastChild();
            last.push_back(e->getFishPose());
        }
    }
    return last;
}

void BioTrackerTrackingAlgorithm::refreshPolygon()
{
}

void BioTrackerTrackingAlgorithm::receiveParametersChanged()
{
    if (_lastFramenumber >= 0 && _lastImage && !_lastImage->empty()) {
        doTracking(*_lastImage, _lastFramenumber);
    }
}

std::vector<BlobPose> BioTrackerTrackingAlgorithm::getContourCentroids(
    cv::Mat image)
{

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i>              hierarchy;
    std::vector<BlobPose>               centroids;

    cv::findContours(image,
                     contours,
                     hierarchy,
                     cv::RETR_TREE,
                     cv::CHAIN_APPROX_SIMPLE,
                     cv::Point(0, 0));

    for (auto x : contours) {
        cv::Point2f c(0, 0);
        float       i = 0;
        for (auto y : x) {
            c += cv::Point2f(y);
            i++;
        }
        c.x = c.x / i;
        c.y = c.y / i;

        // cv::RotatedRect minEllipse;
        cv::RotatedRect bb = minAreaRect(x);

        // check if blob is in tracking area --> this can be optimized by
        // checking earlier (only search blobs in tracking area)
        if (!_AreaInfo->inTrackingArea(c)) {
            continue;
        }

        BlobPose bc(_AreaInfo->pxToCm(c),
                    c,
                    bb.angle,
                    bb.size.width,
                    bb.size.height);

        centroids.push_back(bc);
    }

    return centroids;
}

void BioTrackerTrackingAlgorithm::doTracking(cv::Mat image, uint framenumber)
{
    _ipp.m_TrackingParameter = _TrackingParameter;
    _lastImage               = image;
    _lastFramenumber         = framenumber;

    // dont do nothing if we ain't got an image
    if (image.empty()) {
        return;
    }

    if (_imageX != image.size().width || _imageY != image.size().height) {
        _imageX = image.size().width;
        _imageY = image.size().height;
        Q_EMIT emitDimensionUpdate(_imageX, _imageY);
    }

    std::chrono::system_clock::time_point start =
        std::chrono::system_clock::now();

    // Refuse to run tracking if we have no area info...
    if (_AreaInfo == nullptr) {
        Q_EMIT emitTrackingDone(framenumber);
        return;
    }

    // The user changed the # of fish. Reset the history and start over!
    if (_noFish != _TrackedTrajectoryMajor->validCount()) {
        _noFish = _TrackedTrajectoryMajor->validCount();
        _nn2d   = std::make_shared<NN2dMapper>(_TrackedTrajectoryMajor);
    }

    if (_TrackingParameter->getResetBackground()) {
        _TrackingParameter->setResetBackground(false);
        _ipp.resetBackgroundImage();
    }

    // Do the preprocessing
    auto    images  = _ipp.preProcess(image);
    cv::Mat mask    = *images.find("Closed Mask");
    cv::Mat greyMat = *images.find("Greyscale");

    // Find blobs via ellipsefitting
    _bd.setMaxBlobSize(_TrackingParameter->getMaxBlobSize());
    _bd.setMinBlobSize(_TrackingParameter->getMinBlobSize());

    std::vector<BlobPose> blobs = getContourCentroids(mask);

    // Never switch the position of the trajectories. The NN2d mapper relies on
    // this! If you mess up the order, add or remove some t, then create a new
    // mapper.
    std::vector<FishPose> fish = getLastPositionsAsPose();

    // Find new positions using 2D nearest neighbor
    std::tuple<std::vector<FishPose>, std::vector<float>> poses =
        _nn2d->getNewPoses(_TrackedTrajectoryMajor, framenumber, blobs);

    // Insert new poses into data structure
    int trajNumber = 0;
    for (int i = 0; i < _TrackedTrajectoryMajor->size(); i++) {
        BST::TrackedTrajectory* t = dynamic_cast<BST::TrackedTrajectory*>(
            _TrackedTrajectoryMajor->getChild(i));
        if (t && t->getValid() && !t->getFixed()) {
            BST::TrackedElement* e = new BST::TrackedElement(t,
                                                             "n.a.",
                                                             t->getId());

            e->setFishPose(std::get<0>(poses)[trajNumber]);
            e->setTime(start);
            t->add(e, framenumber);
            trajNumber++;
        }
    }

    // Send forth new positions to the robotracker, if networking is enabled
    if (_TrackingParameter->getDoNetwork()) {
        std::vector<FishPose> ps = std::get<0>(poses);
        _listener->sendPositions(framenumber,
                                 ps,
                                 std::vector<cv::Point2f>(),
                                 start);
    }

    emit trackingImagesChanged(images);

    std::string newSel = _TrackingParameter->getNewSelection();

    Q_EMIT emitTrackingDone(framenumber);
}
