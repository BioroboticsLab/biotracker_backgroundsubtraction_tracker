#ifndef TRACKERPARAMETER_H
#define TRACKERPARAMETER_H


#include "Interfaces/IModel/IModel.h"
#include "../Config.h"
#include "TrackingAlgorithm/ParamNames.h"

class TrackerParameter : public IModel
{
    Q_OBJECT
public:
    TrackerParameter(QObject *parent = 0);

    void setThreshold(int x);

    int getThreshold();

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

	int getmog2History() { return _mog2History; };
	void setmog2History(int x) {
		_mog2History = x;
		Q_EMIT notifyView();
	};

	int getmog2VarThresh() { return _mog2VarThresh; };
	void setmog2VarThresh(int x) {
		_mog2VarThresh = x;
		Q_EMIT notifyView();
	};

	double getmog2BackgroundRatio() { return _mog2BackgroundRatio; };
	void setmog2BackgroundRatio(double x) {
		_mog2BackgroundRatio = x;
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
		int Threshold,
		int BinarizationThreshold,
		int SizeErode,
		int SizeDilate,
		int mog2History,
		int mog2VarThresh,
		double mog2BackgroundRatio,
		int minBlobSize, 
		int maxBlobSize)
	{
		_Threshold = Threshold;
		_BinarizationThreshold = BinarizationThreshold;
		_SizeErode = SizeErode;
		_SizeDilate = SizeDilate;
		_mog2History = mog2History;
		_mog2VarThresh = mog2VarThresh;
		_mog2BackgroundRatio = mog2BackgroundRatio;
		_MinBlobSize = minBlobSize;
		_MaxBlobSize = maxBlobSize;
		_cfg->BinarizationThreshold = BinarizationThreshold;
		_cfg->SizeErode = SizeErode;
		_cfg->SizeDilate = SizeDilate;
		_cfg->MinBlobSize = minBlobSize;
		_cfg->MaxBlobSize = maxBlobSize;
		_cfg->Mog2History = mog2History;
		_cfg->Mog2VarThresh = mog2VarThresh;
		_cfg->Mog2BackgroundRatio = mog2BackgroundRatio;
		Q_EMIT notifyView();
	};



private:

	int _Threshold;
	int _BinarizationThreshold;
	int _SizeErode;
	int _SizeDilate;
	int _mog2History;
	int _mog2VarThresh;
	double _mog2BackgroundRatio;
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
