#include "TrackerParameterView.h"
#include "ui_TrackerParameterView.h"

#include <iostream>

TrackerParameterView::TrackerParameterView(QWidget *parent, IController *controller, IModel *model) : IViewWidget(parent, controller, model),
																																		_ui(new Ui::TrackerParameterView)
{
	_ui->setupUi(this);
	getNotified();

	auto parameter = qobject_cast<TrackerParameter *>(getModel());

	connect(_ui->lineEdit_2_binThresh, qOverload<int>(&QSpinBox::valueChanged), parameter, &TrackerParameter::setBinarizationThreshold);
	connect(_ui->lineEdit_2_binThresh, qOverload<int>(&QSpinBox::valueChanged), this, &TrackerParameterView::parametersChanged);

	connect(_ui->lineEdit_3_SizeErode, qOverload<int>(&QSpinBox::valueChanged), parameter, &TrackerParameter::setSizeErode);
	connect(_ui->lineEdit_3_SizeErode, qOverload<int>(&QSpinBox::valueChanged), this, &TrackerParameterView::parametersChanged);

	connect(_ui->lineEdit_4_SizeDilate, qOverload<int>(&QSpinBox::valueChanged), parameter, &TrackerParameter::setSizeDilate);
	connect(_ui->lineEdit_4_SizeDilate, qOverload<int>(&QSpinBox::valueChanged), this, &TrackerParameterView::parametersChanged);

	connect(_ui->lineEdit_8_MinBlob, qOverload<int>(&QSpinBox::valueChanged), parameter, &TrackerParameter::setMinBlobSize);
	connect(_ui->lineEdit_8_MinBlob, qOverload<int>(&QSpinBox::valueChanged), this, &TrackerParameterView::parametersChanged);

	connect(_ui->lineEdit_9MaxBlob, qOverload<int>(&QSpinBox::valueChanged), parameter, &TrackerParameter::setMaxBlobSize);
	connect(_ui->lineEdit_9MaxBlob, qOverload<int>(&QSpinBox::valueChanged), this, &TrackerParameterView::parametersChanged);

	connect(_ui->lineEdit_7_bgRatio, qOverload<double>(&QDoubleSpinBox::valueChanged), parameter, &TrackerParameter::setBackgroundRatio);
	connect(_ui->lineEdit_7_bgRatio, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &TrackerParameterView::parametersChanged);
}

TrackerParameterView::~TrackerParameterView()
{
	delete _ui;
}

void TrackerParameterView::on_pushButtonResetBackground_clicked()
{
	TrackerParameter *parameter = qobject_cast<TrackerParameter *>(getModel());
	parameter->setResetBackground(true);
}

void TrackerParameterView::on_comboBoxSendImage_currentIndexChanged(int v)
{
	TrackerParameter *parameter = qobject_cast<TrackerParameter *>(getModel());
	parameter->setSendImage(v);
	parameter->setNewSelection(_ui->comboBoxSendImage->currentText().toStdString());
}

void TrackerParameterView::getNotified()
{
	TrackerParameter *parameter = qobject_cast<TrackerParameter *>(getModel());

	int val = parameter->getBinarizationThreshold();
	_ui->lineEdit_2_binThresh->setValue(val);

	val = parameter->getSizeErode();
	_ui->lineEdit_3_SizeErode->setValue(val);

	val = parameter->getSizeDilate();
	_ui->lineEdit_4_SizeDilate->setValue(val);

	double dval = parameter->getBackgroundRatio();
	_ui->lineEdit_7_bgRatio->setValue(dval);

	val = parameter->getMinBlobSize();
	_ui->lineEdit_8_MinBlob->setValue(val);

	val = parameter->getMaxBlobSize();
	_ui->lineEdit_9MaxBlob->setValue(val);
}
