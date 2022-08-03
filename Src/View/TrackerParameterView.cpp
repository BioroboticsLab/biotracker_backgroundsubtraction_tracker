#include "TrackerParameterView.h"
#include "ui_TrackerParameterView.h"

#include <iostream>

TrackerParameterView::TrackerParameterView(QWidget*     parent,
                                           IController* controller,
                                           IModel*      model)
: IViewWidget(parent, controller, model)
, _ui(new Ui::TrackerParameterView)
, _useAbsDiff(nullptr)
, _binThres(nullptr)
, _maxImg(nullptr)
{
    _ui->setupUi(this);
    getNotified();

    auto parameter = qobject_cast<TrackerParameter*>(getModel());

    connect(_ui->algorithmCB,
            &QComboBox::currentTextChanged,
            parameter,
            &TrackerParameter::setAlgorithm);
    connect(_ui->algorithmCB,
            &QComboBox::currentTextChanged,
            this,
            &TrackerParameterView::initSubtractorSpecificUI);

    connect(_ui->lineEdit_3_OpeningErosionSize,
            qOverload<int>(&QSpinBox::valueChanged),
            parameter,
            &TrackerParameter::setOpeningErosionSize);
    connect(_ui->lineEdit_3_OpeningErosionSize,
            qOverload<int>(&QSpinBox::valueChanged),
            this,
            &TrackerParameterView::parametersChanged);

    connect(_ui->lineEdit_4_OpeningDilationSize,
            qOverload<int>(&QSpinBox::valueChanged),
            parameter,
            &TrackerParameter::setOpeningDilationSize);
    connect(_ui->lineEdit_4_OpeningDilationSize,
            qOverload<int>(&QSpinBox::valueChanged),
            this,
            &TrackerParameterView::parametersChanged);

    connect(_ui->lineEdit_4_ClosingDilationSize,
            qOverload<int>(&QSpinBox::valueChanged),
            parameter,
            &TrackerParameter::setClosingDilationSize);
    connect(_ui->lineEdit_4_ClosingDilationSize,
            qOverload<int>(&QSpinBox::valueChanged),
            this,
            &TrackerParameterView::parametersChanged);

    connect(_ui->lineEdit_3_ClosingErosionSize,
            qOverload<int>(&QSpinBox::valueChanged),
            parameter,
            &TrackerParameter::setClosingErosionSize);
    connect(_ui->lineEdit_3_ClosingErosionSize,
            qOverload<int>(&QSpinBox::valueChanged),
            this,
            &TrackerParameterView::parametersChanged);

    connect(_ui->lineEdit_8_MinBlob,
            qOverload<int>(&QSpinBox::valueChanged),
            parameter,
            &TrackerParameter::setMinBlobSize);
    connect(_ui->lineEdit_8_MinBlob,
            qOverload<int>(&QSpinBox::valueChanged),
            this,
            &TrackerParameterView::parametersChanged);

    connect(_ui->lineEdit_9MaxBlob,
            qOverload<int>(&QSpinBox::valueChanged),
            parameter,
            &TrackerParameter::setMaxBlobSize);
    connect(_ui->lineEdit_9MaxBlob,
            qOverload<int>(&QSpinBox::valueChanged),
            this,
            &TrackerParameterView::parametersChanged);

    connect(_ui->lineEdit_7_learningRate,
            qOverload<double>(&QDoubleSpinBox::valueChanged),
            parameter,
            &TrackerParameter::setLearningRate);
    connect(_ui->lineEdit_7_learningRate,
            qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,
            &TrackerParameterView::parametersChanged);

    initSubtractorSpecificUI(_ui->algorithmCB->currentText());

    _ui->algorithmCB->setEnabled(false);
}

TrackerParameterView::~TrackerParameterView()
{
    delete _ui;
}

void TrackerParameterView::initSubtractorSpecificUI(QString algorithm)
{
    auto parameter = qobject_cast<TrackerParameter*>(getModel());

    while (_ui->algorithmSpecificParameterLayout->rowCount() > 0) {
        _ui->algorithmSpecificParameterLayout->removeRow(0);
    }

    _useAbsDiff = nullptr;
    _binThres   = nullptr;
    _maxImg     = nullptr;

    if (algorithm == QString("Custom")) {
        _useAbsDiff = new QCheckBox();
        _useAbsDiff->setText(" ");
        _useAbsDiff->setChecked(parameter->getUseAbsoluteDifference());
        _ui->algorithmSpecificParameterLayout->addRow(
            tr("Use Absolute Difference:"),
            _useAbsDiff);

        connect(_useAbsDiff,
                &QCheckBox::toggled,
                parameter,
                &TrackerParameter::setUseAbsoluteDifference);
        connect(_useAbsDiff,
                &QCheckBox::toggled,
                this,
                &TrackerParameterView::parametersChanged);

        _binThres = new QSpinBox();
        _binThres->setMinimum(1);
        _binThres->setMaximum(255);
        _binThres->setValue(parameter->getBinarizationThreshold());
        _ui->algorithmSpecificParameterLayout->addRow(
            tr("Binarization Threshold:"),
            _binThres);

        connect(_binThres,
                qOverload<int>(&QSpinBox::valueChanged),
                parameter,
                &TrackerParameter::setBinarizationThreshold);
        connect(_binThres,
                qOverload<int>(&QSpinBox::valueChanged),
                this,
                &TrackerParameterView::parametersChanged);

        _maxImg = new QSpinBox();
        _maxImg->setMinimum(1);
        _maxImg->setMaximum(255);
        _maxImg->setValue(parameter->getMaximumImageValue());
        _ui->algorithmSpecificParameterLayout->addRow(
            tr("Maximum Image Value:"),
            _maxImg);

        connect(_maxImg,
                qOverload<int>(&QSpinBox::valueChanged),
                parameter,
                &TrackerParameter::setMaximumImageValue);
        connect(_maxImg,
                qOverload<int>(&QSpinBox::valueChanged),
                this,
                &TrackerParameterView::parametersChanged);
    } else {
        qFatal("Unsupported background subtraction algorithm");
    }

    emit parametersChanged();
}

void TrackerParameterView::on_pushButtonResetBackground_clicked()
{
    TrackerParameter* parameter = qobject_cast<TrackerParameter*>(getModel());
    parameter->setResetBackground(true);
}

void TrackerParameterView::on_comboBoxSendImage_currentIndexChanged(int v)
{
    TrackerParameter* parameter = qobject_cast<TrackerParameter*>(getModel());
    parameter->setSendImage(v);
    parameter->setNewSelection(
        _ui->comboBoxSendImage->currentText().toStdString());
}

void TrackerParameterView::getNotified()
{
    TrackerParameter* parameter = qobject_cast<TrackerParameter*>(getModel());

    _ui->lineEdit_7_learningRate->setValue(parameter->getLearningRate());

    if (_useAbsDiff) {
        _useAbsDiff->setChecked(parameter->getUseAbsoluteDifference());
    }

    if (_binThres) {
        _binThres->setValue(parameter->getBinarizationThreshold());
    }

    if (_maxImg) {
        _maxImg->setValue(parameter->getMaximumImageValue());
    }

    _ui->lineEdit_3_OpeningErosionSize->setValue(
        parameter->getOpeningErosionSize());

    _ui->lineEdit_4_OpeningDilationSize->setValue(
        parameter->getOpeningDilationSize());

    _ui->lineEdit_4_ClosingDilationSize->setValue(
        parameter->getClosingDilationSize());

    _ui->lineEdit_3_ClosingErosionSize->setValue(
        parameter->getClosingErosionSize());

    _ui->lineEdit_8_MinBlob->setValue(parameter->getMinBlobSize());

    _ui->lineEdit_9MaxBlob->setValue(parameter->getMaxBlobSize());
}
