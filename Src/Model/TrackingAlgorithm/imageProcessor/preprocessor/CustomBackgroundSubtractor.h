#pragma once

#include <opencv2/video/background_segm.hpp>

class CustomBackgroundSubtractor : public cv::BackgroundSubtractor
{
public:
	void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate = -1) override;

	void getBackgroundImage(cv::OutputArray backgroundImage) const override;

private:
	cv::Mat m_background;
};