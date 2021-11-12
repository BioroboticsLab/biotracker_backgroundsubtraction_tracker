#include "TrackerParameter.h"
#include "../Controller/ControllerTrackingAlgorithm.h"

TrackerParameter::TrackerParameter(QObject *parent) :
    IModel(parent)
{
	
	_cfg = static_cast<ControllerTrackingAlgorithm*>(parent)->getConfig();
	

	_BinarizationThreshold = _cfg->BinarizationThreshold;
	_SizeErode = _cfg->SizeErode;
	_SizeDilate = _cfg->SizeDilate;
	_MinBlobSize = _cfg->MinBlobSize;
	_MaxBlobSize = _cfg->MaxBlobSize;

	_BackgroundRatio = _cfg->BackgroundRatio;

	_doNetwork = _cfg->DoNetwork;
	_networkPort = _cfg->NetworkPort;

	_doBackground = true;
	_sendImage = 0; //Send no image
	_resetBackground = false;

    Q_EMIT notifyView();
}

void TrackerParameter::setBinarizationThreshold(int x)
{
	_BinarizationThreshold = x;
	_cfg->BinarizationThreshold = x;
	Q_EMIT notifyView();
}

int TrackerParameter::getBinarizationThreshold()
{
	return _BinarizationThreshold;
}