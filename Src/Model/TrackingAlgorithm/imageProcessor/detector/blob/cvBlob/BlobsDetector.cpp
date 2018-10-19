#include "BlobsDetector.h"
#include "../../../../../../helper/CvHelper.h"

#include <assert.h>
BlobsDetector::BlobsDetector(void) :	
	_mask(nullptr),
	_minBlobSize(1), //TODO Min blob size
	_maxBlobSize(99999) //TODO Max blob size
{}


void BlobsDetector::filterBlobsBySize(CBlobResult& blobs)
{
	//std::cout << "start blobs: " << blobs.GetNumBlobs() << std::endl;
	// blobs smaller than the provided blob size	
	blobs.Filter( blobs, B_INCLUDE, CBlobGetArea(), B_GREATER_OR_EQUAL, minBlobSize());
	//std::cout << "min filtered blobs: " << blobs.GetNumBlobs() << " min: " << minBlobSize() << std::endl;
	// blobs bigger than the provided blob size	
	blobs.Filter( blobs, B_INCLUDE, CBlobGetArea(), B_LESS_OR_EQUAL, maxBlobSize());
	//std::cout << "max filtered blobs: " << blobs.GetNumBlobs() << " max: " << maxBlobSize() << std::endl;
}

bool isLeft(CvPoint a, CvPoint b, CvPoint c) {
    return ((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
}

std::vector<BlobPose> BlobsDetector::findBlobs(const cv::Mat& processedImage, const cv::Mat& oriImage)
{
	std::vector<BlobPose> blobPoses;

	IplImage iplBinImage(processedImage);
	IplImage *img = 0;
	if (_mask)
		img = new IplImage(*_mask);

	CBlob *currentBlob;
	CBlobResult blobs(&iplBinImage, img, 0);
	delete img;

	// filter the blobs by size criteria
	filterBlobsBySize(blobs);	

	for (int i = 0; i < blobs.GetNumBlobs(); i++)
    {
		currentBlob = blobs.GetBlob(i);

		// gets blob center
		int x = currentBlob->GetEllipse().center.x;
		int y = currentBlob->GetEllipse().center.y;
		cv::Point blobPose_px = cv::Point(x, y);		

		// apply homography
		cv::Point2f blobPose_cm =_areaInfo->pxToCm(blobPose_px);

		// ignore blobs outside the tracking area
		if (!_areaInfo->inTrackingArea(blobPose_px))
			continue;
        /*
        CBlobContour * contour = currentBlob->GetExternalContour();
        t_PointList externContour = contour->GetContourPoints();
        CvMemStorage storage;
        CvPoint actualPoint, previousPoint;
        CvSeqReader reader;
        cvStartReadSeq(externContour, &reader);

        // which contour pixels touch border?
        for (int j = 0; j < externContour->total; j++)
        {
            CV_READ_SEQ_ELEM(actualPoint, reader);
            if (j == 0) previousPoint = actualPoint;
            //std::cout << actualPoint.x << "/" << actualPoint.y << std::endl;
            //std::cout << isLeft(CvPoint a, CvPoint b, actualPoint) << std::endl;
           
            previousPoint = actualPoint;
        }*/
        
		float blobPose_angle_deg = currentBlob->GetEllipse().angle;
		float blobPose_angle_rad = currentBlob->GetEllipse().angle * CV_PI / float(180.0);
		assert(blobPose_angle_deg >= 0.0f && blobPose_angle_deg <= 360.0f);
		//std::cout << "angle: " << blobPose_angle_deg << std::endl;
		float blobPose_width =  currentBlob->GetEllipse().size.width;
		float blobPose_height = currentBlob->GetEllipse().size.height;

		blobPoses.push_back(BlobPose(blobPose_cm, blobPose_px, blobPose_angle_deg, blobPose_angle_rad, blobPose_width, blobPose_height));
	}

	return blobPoses;
}

std::vector<BlobPose> BlobsDetector::getPoses(cv::Mat& processedImage, cv::Mat& oriImage)
{	
	return findBlobs(processedImage, oriImage);
}

void BlobsDetector::setDouble(std::string spec_param, double value)
{	
	if(spec_param.compare("1") == 0) {
		this->setMinBlobSize(value);
	}
	else if (spec_param.compare("999999") == 0) {
		this->setMaxBlobSize(value);
	}
	else {
		std::cout << "BlobsDetector::Warning - Parameter: " << spec_param << " not found!" << std::endl;
	}
}
