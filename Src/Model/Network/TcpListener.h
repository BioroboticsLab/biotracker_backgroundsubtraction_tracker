#pragma once

#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>

#include "../TrackedComponents/pose/FishPose.h"

#include <vector>
#include <chrono>

class TcpListener : public QTcpServer
{
	Q_OBJECT

public:
	TcpListener(QObject *parent = 0);

public slots:
	void acceptConnection();
	void sendPositionsToSocket(std::string packet);
	std::string sendPositions(int frameNo,
		const std::vector<FishPose>& poses,
		const std::vector<cv::Point2f>& polygon,
		std::chrono::system_clock::time_point ts);

private:
	std::vector<QTcpSocket *> _sockets;
};
