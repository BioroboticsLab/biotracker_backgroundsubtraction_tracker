#pragma once

#include <cv.h>
#include <highgui.h>
#include <chrono>
#include "../TrackedComponents/pose/FishPose.h"
#include "../TrackedComponents/TrackedElement.h"
#include "../TrackedComponents/TrackedTrajectory.h"
#include "imageProcessor/detector/blob/cvBlob/BlobsDetector.h"

class NN2dMapper
{
public:

	/**
	 * The contructor with parameters.
	 */
	NN2dMapper(TrackedTrajectory *tree);

	~NN2dMapper(void) {};
	
	std::tuple<std::vector<FishPose>, std::vector<float>> getNewPoses(TrackedTrajectory* traj, uint frameid, std::vector<BlobPose> blobPoses);
	std::vector<FishPose> convertBlobPosesToFishPoses(std::vector<BlobPose> blobPoses);
	float estimateOrientationRad(int trackid, float *confidence);
	bool correctAngle(int trackid, FishPose &pose);
	
	std::map<int, float> _mapLastConfidentAngle;
	TrackedTrajectory *_tree;

protected:

};

