#include "CustomBackgroundSubtractor.h"

#include <vector>
#include <future>

#include <opencv2/imgproc.hpp>

void CustomBackgroundSubtractor::setBinarizationThreshold(int value)
{
	m_binarizationThreshold = value;
}

void CustomBackgroundSubtractor::getBackgroundImage(cv::OutputArray backgroundImage) const
{
	m_background.copyTo(backgroundImage);
}

void CustomBackgroundSubtractor::apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate)
{
	if (!m_background.data) {
		image.copyTo(m_background);
	}

	const int imageWidth = m_background.cols;
	const int imageHeight = m_background.rows;
	const int totalRegionsX = 4;
	const int totalRegionsY = 4;
	const int totalRegions = totalRegionsX * totalRegionsY;
	const int regionWidth = imageWidth / totalRegionsX;
	const int regionHeight = imageHeight / totalRegionsY;

	fgmask.create(imageHeight, imageWidth, image.type());

	auto fgmaskmat = fgmask.getMat();

	auto workOnRegion = [&](int x, int y) {
		const int startingX = x * regionWidth;
		const int startingY = y * regionHeight;
		const cv::Rect subArea = cv::Rect(startingX, startingY, regionWidth, regionHeight);
		cv::Mat subBackground = m_background(subArea);
		cv::Mat subImage = image.getMat()(subArea);
		cv::Mat subResults = fgmaskmat(subArea);

		subResults = (subBackground - subImage);
		subBackground = (1.0 - learningRate) * subBackground + learningRate * subImage;
	};

	std::vector<std::future<void>> merger;
	merger.reserve(totalRegions);

	for (int x = 0; x < totalRegionsX; ++x)
	{
		for (int y = 0; y < totalRegionsY; ++y)
		{
			merger.push_back(std::async([&, x, y] { workOnRegion(x, y); }));
		}
	}

	for (const auto & asyncResult : merger) {
		asyncResult.wait();
	}

	if (fgmaskmat.data) {
		cv::threshold(fgmaskmat, fgmaskmat, m_binarizationThreshold, 255, cv::THRESH_BINARY);
	}
}
