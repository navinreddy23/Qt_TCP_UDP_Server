#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTimer>
#include <QObject>
#include <QTcpServer>
#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QTcpSocket>
#include <QThreadPool>
#include "Programs/Misc/Logger.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

    void Start(quint16 port);
    void Quit();
    bool Started() const;
    quint32 ActiveConnections() const;
    quint32 DevicesUpdated() const;

signals:
    emit void quit();
    void logOutput(log_level_t level, const QString& message);
    emit void deviceUpdated(const QString& device, const QString& version);

    // QTcpServer interface
protected:
    void incomingConnection(qintptr handle);

private slots:
    void onCloseConn(void);

private:
    QTimer m_timer;
    bool m_started;
    quint32 m_activeConnections = 0;
    QThreadPool pool;

    Logger logger;

    void LogInputEmitter(log_level_t level, const QString& message);
    void LogOutputEmitter(log_level_t level, const QString &message);
};

#endif // TCPSERVER_H
