#ifndef TRACKERPARAMETER_H
#define TRACKERPARAMETER_H


#include "Interfaces/IModel/IModel.h"
#include "../Config.h"

class TrackerParameter : public IModel
{
    Q_OBJECT
public:
    TrackerParameter(QObject *parent = 0);

public slots:

	void setBinarizationThreshold(int x);
	int getBinarizationThreshold();

	int getSizeErode() { return _SizeErode; };
	void setSizeErode(int x) {
		_SizeErode = x;
		Q_EMIT notifyView();
	};

	int getSizeDilate() { return _SizeDilate; };
	void setSizeDilate(int x) {
		_SizeDilate = x;
		Q_EMIT notifyView();
	};

	double getBackgroundRatio() { return _LearningRate; };
	void setBackgroundRatio(double x) {
		_LearningRate = x;
		Q_EMIT notifyView();
	};

	double getMinBlobSize() { return _MinBlobSize; };
	void setMinBlobSize(double x) {
		_MinBlobSize = x;
		Q_EMIT notifyView();
	};

	double getMaxBlobSize() { return _MaxBlobSize; };
	void setMaxBlobSize(double x) {
		_MaxBlobSize = x;
		Q_EMIT notifyView();
	};

	bool getDoBackground() { return _doBackground; };
	void setDoBackground(bool x) {
		_doBackground = x;
		Q_EMIT notifyView();
	};

	bool getDoNetwork() { return _doNetwork; };
	void setDoNetwork(bool x) {
		_doNetwork = x;
		Q_EMIT notifyView();
	};

	int getSendImage() { return _sendImage; };
	void setSendImage(int x) {
		_sendImage = x;
		Q_EMIT notifyView();
	};

	bool getResetBackground() { return _resetBackground; };
	void setResetBackground(bool x) {
		_resetBackground = x;
		Q_EMIT notifyView();
	};

	int getNoFish() { return _noFish; };
	void setNoFish(int x) {
		_noFish = x;
		_cfg->NoFish = x;
		Q_EMIT notifyView();
	};

	std::string getNewSelection() { return _newSelection; };
	void setNewSelection(std::string x) {
		_newSelection = x;
	}
	

	void setAll(
		int BinarizationThreshold,
		int SizeErode,
		int SizeDilate,
		double LearningRate,
		int minBlobSize, 
		int maxBlobSize)
	{
		_BinarizationThreshold = BinarizationThreshold;
		_SizeErode = SizeErode;
		_SizeDilate = SizeDilate;
		_LearningRate = LearningRate;
		_MinBlobSize = minBlobSize;
		_MaxBlobSize = maxBlobSize;
		_cfg->BinarizationThreshold = BinarizationThreshold;
		_cfg->SizeErode = SizeErode;
		_cfg->SizeDilate = SizeDilate;
		_cfg->MinBlobSize = minBlobSize;
		_cfg->MaxBlobSize = maxBlobSize;
		_cfg->LearningRate = LearningRate;
		Q_EMIT notifyView();
	};



private:

	int _BinarizationThreshold;
	int _SizeErode;
	int _SizeDilate;
	double _LearningRate;
	int _MinBlobSize;
	int _MaxBlobSize;

	bool _doBackground;
	int _sendImage;
	bool _resetBackground;
	int _noFish;

	int _networkPort;
	bool _doNetwork;

	std::string _newSelection;
	Config *_cfg;
};

#endif // TRACKERPARAMETER_H
