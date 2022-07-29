#include "ImagePreProcessor.h"

#include <opencv2/highgui.hpp>

#include <QMutex>

#include <opencv2/bgsegm.hpp>

#include "CustomBackgroundSubtractor.h"

QMutex bgsMutex;

ImagePreProcessor::ImagePreProcessor(TrackerParameter* TrackingParameter)
: m_subtractor(nullptr)
, m_TrackingParameter{TrackingParameter}
{
    init();
}

ImagePreProcessor::~ImagePreProcessor(void)
{
}

void ImagePreProcessor::init()
{
    QMutexLocker locker(&bgsMutex);

    auto algorithm = m_TrackingParameter->getAlgorithm();
    if (algorithm == QString("Custom")) {
        m_subtractor = new CustomBackgroundSubtractor();
    } else {
        qFatal("Unsupported background subtraction algorithm");
    }

    m_backgroundImage = std::make_shared<cv::Mat>();
    m_foregroundMask  = std::make_shared<cv::Mat>();

    _backgroundSubtractionEnabled = true;
    _backgroundEnabled            = true;
    _erodeEnabled                 = true;
    _dilateEnabled                = true;
    _gaussianBlurEnabled          = false;
    _binarizeEnabled              = true;
    _resetBackgroundImageEnabled  = false;
}

cv::Mat ImagePreProcessor::erode(cv::Mat image, int kernelSize)
{
    if (kernelSize > 0 && image.data) {
        auto kernel = cv::getStructuringElement(
            cv::MORPH_CROSS,
            cv::Size(kernelSize, kernelSize));

        cv::Mat morphed;
        cv::erode(image, morphed, kernel);

        return morphed;
    } else {
        return image;
    }
}

cv::Mat ImagePreProcessor::dilate(cv::Mat image, int kernelSize)
{
    if (kernelSize > 0 && image.data) {
        auto kernel = cv::getStructuringElement(
            cv::MORPH_RECT,
            cv::Size(kernelSize, kernelSize));

        cv::Mat morphed;
        cv::dilate(image, morphed, kernel);

        return morphed;
    } else {
        return image;
    }
}

cv::Mat ImagePreProcessor::backgroundSubtraction(cv::Mat& image)
{
    if (auto subtractor =
            m_subtractor.dynamicCast<CustomBackgroundSubtractor>();
        subtractor) {
        if (m_TrackingParameter->getAlgorithm() != QString("Custom")) {
            init();
        }
        subtractor->setUseAbsoluteDifference(
            m_TrackingParameter->getUseAbsoluteDifference());
        subtractor->setBinarizationThreshold(
            m_TrackingParameter->getBinarizationThreshold());
    } else {
        qFatal("Unsupported background subtraction algorithm");
    }

    cv::Mat fgmask;
    m_subtractor->apply(image, fgmask, m_TrackingParameter->getLearningRate());
    m_subtractor->getBackgroundImage(*m_backgroundImage);
    return fgmask;
}

std::map<std::string, std::shared_ptr<cv::Mat>> ImagePreProcessor::preProcess(
    std::shared_ptr<cv::Mat> p_image)
{
    std::shared_ptr<cv::Mat> greyMat    = std::make_shared<cv::Mat>();
    std::shared_ptr<cv::Mat> openedMask = std::make_shared<cv::Mat>();
    std::shared_ptr<cv::Mat> closedMask = std::make_shared<cv::Mat>();

    cv::cvtColor(*p_image, *greyMat, cv::COLOR_BGR2GRAY);

    // 1. step: do the background subtraction
    *m_foregroundMask = backgroundSubtraction(*greyMat);

    // 2. step: open the mask
    *openedMask = dilate(
        erode(*m_foregroundMask, m_TrackingParameter->getOpeningErosionSize()),
        m_TrackingParameter->getOpeningDilationSize());

    // 3. step: close the image
    *closedMask = erode(
        dilate(*openedMask, m_TrackingParameter->getClosingDilationSize()),
        m_TrackingParameter->getClosingErosionSize());

    std::map<std::string, std::shared_ptr<cv::Mat>> all;
    all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(
        std::string("Greyscale"),
        greyMat));
    all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(
        std::string("Background"),
        m_backgroundImage));
    all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(
        std::string("Foreground Mask"),
        m_foregroundMask));
    all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(
        std::string("Opened Mask"),
        openedMask));
    all.insert(std::pair<std::string, std::shared_ptr<cv::Mat>>(
        std::string("Closed Mask"),
        closedMask));

    return all;
}

void ImagePreProcessor::resetBackgroundImage()
{
    // this will reset the background at the next opportunity
    init();
}
