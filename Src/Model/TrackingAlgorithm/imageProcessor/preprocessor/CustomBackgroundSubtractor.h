#pragma once

#include <opencv2/video/background_segm.hpp>

class CustomBackgroundSubtractor : public cv::BackgroundSubtractor
{
public:
    CustomBackgroundSubtractor();

    void apply(cv::InputArray  image,
               cv::OutputArray fgmask,
               double          learningRate = -1) override;

    void getBackgroundImage(cv::OutputArray backgroundImage) const override;

    void setUseAbsoluteDifference(bool value);
    void setBinarizationThreshold(int value);

private:
    cv::Mat m_background;

    bool m_useAbsoluteDifference;

    int m_binarizationThreshold;
};
