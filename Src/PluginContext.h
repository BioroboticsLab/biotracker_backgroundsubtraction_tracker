#pragma once
#include "Interfaces/IBioTrackerContext.h"
#include "Interfaces/IController/IController.h"
#include "Config.h"

class PluginContext : public IBioTrackerContext
{
    Q_OBJECT

public:
    PluginContext(QObject* parent, Config* cfg);

    // IBioTrackerContext interface
protected:
    void createAppController() override;
    void connectController() override;

private:
    Config* _cfg;
};
