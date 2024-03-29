#pragma once

#include <QSettings>
#include <QMap>

#include <opencv2/opencv.hpp>

#include "../../../TrackerParameter.h"

class ImagePreProcessor
{
public:
    /**
     * The standard constructor.
     */
    ImagePreProcessor(TrackerParameter* TrackingParameter);

    /**
     * The standard destructor.
     */
    ~ImagePreProcessor();

    /**
     * Init function. Sets the property for the imge pre-processing.
     * @return void.
     */
    void init();

    /**
     * A mathematical morphology operation using in computer vision to erode an
     * image. Erode image with 3x3 4-connectivity.
     * @param: image, image to erode,
     * @param: kernelSize, size of erosion kernel,
     * @return: an eroded image.
     */
    cv::Mat erode(cv::Mat image, int kernelSize);

    /**
     * A mathematical morphology operation using in computer vision to dilate
     * an image. Dilate image with 6x6 8-connectivity.
     * @param: image, image to dilate,
     * @param: kernelSize, size of dilation kernel,
     * @return: a dilated image.
     */
    cv::Mat dilate(cv::Mat image, int kernelSize);

    /**
     * A computer vision methode to calculate the image difference.
     * Background image subtracts the foreground image.
     * @param: image, image to background subtract,
     * @return: the background subtracted image.
     */
    cv::Mat backgroundSubtraction(cv::Mat& image);

    /**
     * Pre-process an image, if all methods enabled, this function:
     * - does the background subtraction
     * - erodes the image
     * - dilates the image
     * @param: image, image to process,
     * @return: a pre-process image.
     */
    QMap<QString, cv::Mat> preProcess(cv::Mat p_image);

    /**
     * The method updates the image background.
     * @return: void.
     */
    void              resetBackgroundImage();
    TrackerParameter* m_TrackingParameter;

private:
    cv::Mat _outputImage;

    cv::Mat m_backgroundImage;
    cv::Mat m_foregroundMask;

    // parameters for image pre-processing
    bool _backgroundSubtractionEnabled;
    bool _backgroundEnabled;
    bool _binarizeEnabled;
    bool _erodeEnabled;
    bool _dilateEnabled;
    bool _gaussianBlurEnabled;
    bool _resetBackgroundImageEnabled;

    cv::Ptr<cv::BackgroundSubtractor> m_subtractor;
};
