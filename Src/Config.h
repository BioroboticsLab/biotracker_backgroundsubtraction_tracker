#pragma once

#include "Utility/IConfig.h"
#include <QString>
#include <map>

class Config : public IConfig
{
public:
    bool UseAbsoluteDifference = true;
    int  BinarizationThreshold = 40;
    int  MaximumImageValue = 255;

    int OpeningErosionSize  = 8;
    int OpeningDilationSize = 8;

    int ClosingDilationSize = 8;
    int ClosingErosionSize  = 8;

    int MinBlobSize = 40;
    int MaxBlobSize = 999999;

    double LearningRate = 0.05;

    int DoNetwork   = true;
    int NetworkPort = 54444;

    int DoBackground    = true;
    int ResetBackground = false;

    int EnableView   = 1;
    int EnableMove   = 1;
    int EnableRemove = 1;
    int EnableSwap   = 1;
    int EnableAdd    = 1;
    int EnableRotate = 1;

    int NoFish = 0;

    void load(QString dir, QString file = "config.ini") override;
    void save(QString dir, QString file) override;

    static const QString DefaultArena;
};
