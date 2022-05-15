#ifndef TCPHANDLER_H
#define TCPHANDLER_H

#include <QTimer>
#include <QObject>
#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "Programs/Misc/Logger.h"
#include "Programs/IMU/Imu.h"

class TcpHandler : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TcpHandler(QObject *parent = nullptr,
                           qintptr handle = 0 );

    ~TcpHandler();

signals:
    void logOutput(log_level_t level, const QString& message);
    void closeConn(void);

    // QRunnable interface
public:
    void run();

public slots:
    void onTimeout();
    void onQuit();

private slots:
    void close();
    void onReadyRead();

private:
    qintptr handle;
    QTcpSocket* socket = new QTcpSocket(nullptr);

    QString m_sendCommand = "";

    quint64 m_bytesReceived = 0;
    quint64 m_packetsReceived = 0;
    quint64 m_timeout = 0;
    quint32 m_idle = 0;
    bool m_quit = false;
    Logger logger;

    void DebugPrint(const QByteArray& data);

    void LogInputEmitter(log_level_t level, const QString& message);
    void LogOutputEmitter(log_level_t level, const QString &message);
};

#endif // TCPHANDLER_H
