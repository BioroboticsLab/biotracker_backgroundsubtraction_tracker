#include "SimpleBlobsDetector.h"
#include "../../../../../../helper/CvHelper.h"
#include <limits>

SimpleBlobsDetector::SimpleBlobsDetector(void) : _mask(nullptr)
{
	initParams();
}

void SimpleBlobsDetector::initParams()
{
	_params.minThreshold = 15;
	_params.maxThreshold = 50;
	_params.thresholdStep = 5;	
	_params.filterByArea = true;
	_params.minArea = 1;
	_params.maxArea = 999999;
	_params.filterByInertia = false;
	_params.filterByColor = true;
	_params.blobColor = 255;
	_params.filterByCircularity = false;
	_params.minDistBetweenBlobs = 0; 
}


std::vector<BlobPose> SimpleBlobsDetector::findBlobs(const cv::Mat& binImage, const cv::Mat& oriImage)
{
	std::vector<BlobPose> blobPoses;

	std::vector<cv::KeyPoint> keyPoints;

	cv::Ptr<cv::SimpleBlobDetector> blobDetector = cv::SimpleBlobDetector::create(_params);

	blobDetector->detect( binImage, keyPoints);

	for (int i = 0; i < keyPoints.size(); i++)
	{
		// gets blob center
		int x = keyPoints.at(i).pt.x;
		int y = keyPoints.at(i).pt.y;
		cv::Point blobPose_px = cv::Point(x, y);		

		// apply homography
		cv::Point2f blobPose_cm = _areaInfo->pxToCm(blobPose_px);

		if (!_areaInfo->inTrackingArea(blobPose_px))
			continue;

		float blobPose_angleDegree = keyPoints.at(i).angle; // as degree (0..360)
		float blobPose_angleRadian = keyPoints.at(i).angle * float(CV_PI) / float(180.0);
		float blobPose_width =  keyPoints.at(i).size;
		float blobPose_height = keyPoints.at(i).size;

		blobPoses.push_back(BlobPose(blobPose_cm, blobPose_px, blobPose_angleDegree, blobPose_angleRadian, blobPose_width, blobPose_height));
	}


	return blobPoses;
}


std::vector<BlobPose> SimpleBlobsDetector::getPoses(cv::Mat& binarized_image_mat, cv::Mat& original_image_mat)
{	
	return findBlobs(binarized_image_mat,original_image_mat);
}

void SimpleBlobsDetector::setDouble(std::string spec_param, double value)
{	
	if(spec_param.compare("1") == 0) { //TRACKERPARAM::MIN_BLOB_SIZE
		_params.minArea = value;
	} else {
		std::cout << "SimpleBlobsDetector::Warning - Parameter: " << spec_param << " not found!" << std::endl;
	}

	if(spec_param.compare("999999") == 0) { //TRACKERPARAM::MAX_BLOB_SIZE
		_params.maxArea = value;
	} else {
		std::cout << "SimpleBlobsDetector::Warning - Parameter: " << spec_param << " not found!" << std::endl;
	}
}
