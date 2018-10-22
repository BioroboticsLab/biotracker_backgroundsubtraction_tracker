##############################################################
#### Biotracker: LukasKanade
##############################################################

include(BT_Utility/VSSugar)
VSSourceGroups(${CMAKE_CURRENT_LIST_DIR})

set(TN bt_backgroundsubtraction.tracker)
include(BT_Utility/TrackerPlugin)

find_package(BT_Interfaces 0.1.1 REQUIRED)
target_link_libraries(${TN} BT_Interfaces)
set_target_properties(${TN} PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)

install(TARGETS ${TN} OPTIONAL DESTINATION .)

target_sources(${TN}
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/PluginContext.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BioTrackerPlugin.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/View/TrackedElementView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/View/TrackerParameterView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/null_Model.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/BioTrackerTrackingAlgorithm.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackerParameter.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/Network/TcpListener.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackedComponents/TrackedComponentFactory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackedComponents/TrackedElement.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackedComponents/TrackedTrajectory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackedComponents/pose/FishPose.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/NN2dMapper.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/preprocessor/ImagePreProcessor.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/cvblobs/blob.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/cvblobs/BlobContour.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/cvblobs/BlobOperators.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/cvblobs/BlobProperties.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/cvblobs/BlobResult.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/cvblobs/ComponentLabeling.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/detector/IDetector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/detector/blob/BlobPose.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/detector/blob/BlobPose.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/detector/blob/cvBlob/BlobsDetector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/detector/blob/simpleBlob/SimpleBlobsDetector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/detector/contour/ContourPose.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Model/TrackingAlgorithm/imageProcessor/detector/contour/ContoursDetector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/helper/CvHelper.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/helper/StringHelper.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Controller/ControllerTrackedComponent.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Controller/ControllerTrackingAlgorithm.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Controller/null_Controller.cpp"
)











