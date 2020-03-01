#pragma once

#include <unordered_set>
#include <chrono>

#include <QThread>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>

#include "../TrackedComponents/pose/FishPose.h"

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
	std::unordered_set<QTcpSocket *> _sockets;
};
