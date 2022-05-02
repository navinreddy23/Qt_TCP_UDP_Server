#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QTimer>
#include <QObject>
#include <QUdpSocket>
#include "Programs/Misc/Logger.h"

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);
    ~UdpServer();

    void Start(quint16 port);
    void Quit();
    bool Started() const;

signals:
    emit void logOutput(log_level_t level, const QString& message);
    emit void deviceUpdated(const QString& device, const QString& version);

private slots:
    void onReadReady();
    void LogInputEmitter(log_level_t level, const QString& message);
    void OnTimeout(void);

private:
    quint64 m_bytesReceived = 0;
    quint64 m_packetsReceived = 0;
    quint64 m_timeout = 0;
    QTimer m_timer;
    bool m_started;
    QUdpSocket socket;
    Logger logger;

    void LogOutputEmitter(log_level_t level, const QString& message);

};

#endif // UDPSERVER_H
