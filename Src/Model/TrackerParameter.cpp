#include "TrackerParameter.h"
#include "../Controller/ControllerTrackingAlgorithm.h"

TrackerParameter::TrackerParameter(QObject* parent)
: IModel(parent)
{

    _cfg = static_cast<ControllerTrackingAlgorithm*>(parent)->getConfig();

    _UseAbsoluteDifference = _cfg->UseAbsoluteDifference;
    _BinarizationThreshold = _cfg->BinarizationThreshold;
    _OpeningErosionSize    = _cfg->OpeningErosionSize;
    _OpeningDilationSize   = _cfg->OpeningDilationSize;
    _ClosingDilationSize   = _cfg->ClosingDilationSize;
    _ClosingErosionSize    = _cfg->ClosingErosionSize;
    _MinBlobSize           = _cfg->MinBlobSize;
    _MaxBlobSize           = _cfg->MaxBlobSize;

    _LearningRate = _cfg->LearningRate;

    _doNetwork   = _cfg->DoNetwork;
    _networkPort = _cfg->NetworkPort;

    _doBackground    = true;
    _sendImage       = 0; // Send no image
    _resetBackground = false;

    _algorithm = "Custom";

    Q_EMIT notifyView();
}

void TrackerParameter::setBinarizationThreshold(int x)
{
    _BinarizationThreshold      = x;
    _cfg->BinarizationThreshold = x;
    Q_EMIT notifyView();
}

int TrackerParameter::getBinarizationThreshold()
{
    return _BinarizationThreshold;
}

void TrackerParameter::setMaximumImageValue(int x)
{
    _MaximumImageValue      = x;
    _cfg->MaximumImageValue = x;
    Q_EMIT notifyView();
}

int TrackerParameter::getMaximumImageValue()
{
    return _MaximumImageValue;
}

void TrackerParameter::setUseAbsoluteDifference(bool value)
{
    _UseAbsoluteDifference      = value;
    _cfg->UseAbsoluteDifference = value;
    Q_EMIT notifyView();
}

bool TrackerParameter::getUseAbsoluteDifference()
{
    return _UseAbsoluteDifference;
}