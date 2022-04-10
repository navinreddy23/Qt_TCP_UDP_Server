#ifndef UDPSERVER_H
#define UDPSERVER_H

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

private:
    bool m_started;
    QUdpSocket socket;
    Logger logger;

    void LogOutputEmitter(log_level_t level, const QString& message);

};

#endif // UDPSERVER_H
