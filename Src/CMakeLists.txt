##############################################################
#### Biotracker: backgroundSubtraction
##############################################################

include(biotracker-utility/TrackerPlugin)

set(target biotracker-backgroundSubtraction.bio_tracker)

add_behavior_plugin(${target}
    "PluginContext.cpp"
    "Config.cpp"
    "BioTrackerPlugin.cpp"
    "View/TrackedElementView.cpp"
    "View/TrackerParameterView.cpp"
    "Model/null_Model.cpp"
    "Model/BioTrackerTrackingAlgorithm.cpp"
    "Model/TrackerParameter.cpp"
    "Model/Network/TcpListener.cpp"
    "Model/TrackedComponents/TrackedComponentFactory.cpp"
    "Model/TrackedComponents/TrackedElement.cpp"
    "Model/TrackedComponents/TrackedTrajectory.cpp"
    "Model/TrackedComponents/pose/FishPose.cpp"
    "Model/TrackingAlgorithm/NN2dMapper.cpp"
    "Model/TrackingAlgorithm/imageProcessor/preprocessor/CustomBackgroundSubtractor.h"
    "Model/TrackingAlgorithm/imageProcessor/preprocessor/CustomBackgroundSubtractor.cpp"
    "Model/TrackingAlgorithm/imageProcessor/preprocessor/ImagePreProcessor.cpp"
    "Model/TrackingAlgorithm/imageProcessor/cvblobs/blob.cpp"
    "Model/TrackingAlgorithm/imageProcessor/cvblobs/BlobContour.cpp"
    "Model/TrackingAlgorithm/imageProcessor/cvblobs/BlobOperators.cpp"
    "Model/TrackingAlgorithm/imageProcessor/cvblobs/BlobProperties.cpp"
    "Model/TrackingAlgorithm/imageProcessor/cvblobs/BlobResult.cpp"
    "Model/TrackingAlgorithm/imageProcessor/cvblobs/ComponentLabeling.cpp"
    "Model/TrackingAlgorithm/imageProcessor/detector/IDetector.cpp"
    "Model/TrackingAlgorithm/imageProcessor/detector/blob/BlobPose.cpp"
    "Model/TrackingAlgorithm/imageProcessor/detector/blob/BlobPose.cpp"
    "Model/TrackingAlgorithm/imageProcessor/detector/blob/cvBlob/BlobsDetector.cpp"
    "Model/TrackingAlgorithm/imageProcessor/detector/blob/simpleBlob/SimpleBlobsDetector.cpp"
    "Model/TrackingAlgorithm/imageProcessor/detector/contour/ContourPose.cpp"
    "Model/TrackingAlgorithm/imageProcessor/detector/contour/ContoursDetector.cpp"
    "helper/CvHelper.cpp"
    "helper/StringHelper.cpp"
    "Controller/ControllerTrackedComponent.cpp"
    "Controller/ControllerTrackingAlgorithm.cpp"
    "Controller/null_Controller.cpp"
)

install(TARGETS ${target} OPTIONAL DESTINATION .)
find_package(Qt5 REQUIRED COMPONENTS Xml Network)
target_link_libraries (${target} Qt5::Xml Qt5::Network)

target_link_libraries(${target} ${OpenCV_LIBS})
