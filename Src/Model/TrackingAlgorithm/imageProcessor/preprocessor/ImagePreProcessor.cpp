#include "ImagePreProcessor.h"

#include <opencv2/highgui.hpp>

#include <QMutex>

#include "CustomBackgroundSubtractor.h"

QMutex bgsMutex;
QMutex oriImageMutex;
QMutex initBgkFrameNumMutex;

ImagePreProcessor::ImagePreProcessor(TrackerParameter* p_TrackingParameter) :
	_maxBackgroundImageInitTime(0)
	, m_subtractor(new CustomBackgroundSubtractor())
{
	_TrackingParameter = p_TrackingParameter;
	init();
}

ImagePreProcessor::~ImagePreProcessor(void)
{
}

void ImagePreProcessor::setBkgFrameNum(int frameNum)
{
	QMutexLocker locker(&initBgkFrameNumMutex);
	_maxBackgroundImageInitTime = frameNum;
}

int ImagePreProcessor::getBkgFrameNum()
{
	QMutexLocker locker(&initBgkFrameNumMutex);
	return _maxBackgroundImageInitTime;
}


void ImagePreProcessor::init()
{
	QMutexLocker locker(&bgsMutex);

	m_backgroundImage = std::make_shared<cv::Mat>();
	m_foregroundImage = std::make_shared<cv::Mat>();

	_backgroundSubtractionEnabled = true;
	_backgroundEnabled = true;
	_erodeEnabled = true;
	_dilateEnabled = true;
	_gaussianBlurEnabled = false;
	_binarizeEnabled = true;
	_resetBackgroundImageEnabled = false;
}

cv::Mat ImagePreProcessor::binarize(cv::Mat& image)
{
	cv::Mat binarizedImage;

	if (image.channels() >= 3)
		cv::cvtColor(image, binarizedImage, cv::COLOR_BGR2GRAY);
	else
		image.copyTo(binarizedImage);

	if (binarizedImage.data)
		cv::threshold(binarizedImage, binarizedImage, m_TrackingParameter->getBinarizationThreshold(), 255, cv::THRESH_BINARY);

	return binarizedImage;
}

cv::Mat ImagePreProcessor::erode(cv::Mat& image)
{
	cv::Mat erodedImage;
	cv::Mat erodeKernel;
	int sizeErode = m_TrackingParameter->getSizeErode();
	if (sizeErode > 0)
	{
		erodeKernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(sizeErode, sizeErode));
	}
	else {
		return image;
	}
	if (image.data)
		cv::erode(image, erodedImage, erodeKernel);
	return erodedImage;
}

cv::Mat ImagePreProcessor::dilate(cv::Mat& image)
{
	cv::Mat dilatedImage;
	cv::Mat dilateKernel;
	int sizeDilate = m_TrackingParameter->getSizeDilate();
	if (sizeDilate > 0)
	{
		dilateKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(sizeDilate, sizeDilate));
	}
	else {
		return image;
	}
	if (image.data)
		cv::dilate(image, dilatedImage, dilateKernel);
	return dilatedImage;
}

cv::Mat ImagePreProcessor::backgroundSubtraction(cv::Mat& image)
{
	cv::Mat foreground;
	m_subtractor->apply(image, foreground, m_TrackingParameter->getBackgroundRatio());
	m_subtractor->getBackgroundImage(*m_backgroundImage);
	return foreground;
}

std::map<std::string, std::shared_ptr<cv::Mat>> ImagePreProcessor::preProcess(std::shared_ptr<cv::Mat> p_image)
{
	std::shared_ptr<cv::Mat> greyMat		= std::make_shared<cv::Mat>();
	std::shared_ptr<cv::Mat> binarizedImage = std::make_shared<cv::Mat>();
	std::shared_ptr<cv::Mat> erodedImage	= std::make_shared<cv::Mat>();
	std::shared_ptr<cv::Mat> dilatedImage	= std::make_shared<cv::Mat>();

	cv::cvtColor(*p_image, *greyMat, cv::COLOR_BGR2GRAY);
	
	// 1. step: do the background subtraction
	*m_foregroundImage = backgroundSubtraction(*greyMat);

	// 2. step: binarize the image 
	*binarizedImage = binarize(*m_foregroundImage);

	// 3. step: erode the image
	*erodedImage = erode(*binarizedImage);

	// 4. step: dilate the image
	*dilatedImage = dilate(*erodedImage);

	std::map<std::string, std::shared_ptr<cv::Mat>> all;
	all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(std::string("Greyscale"), greyMat));
	all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(std::string("Background"), m_backgroundImage));
	all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(std::string("Foreground"), m_foregroundImage));
	all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(std::string("Binarized"), binarizedImage));
	all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(std::string("Eroded"), erodedImage));
	all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(std::string("Dilated"), dilatedImage));

	return all;
}

void ImagePreProcessor::resetBackgroundImage()
{
	// this will reset the background at the next opportunity
	init();
}
