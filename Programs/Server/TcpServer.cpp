#include "TcpServer.h"
#include "TcpHandler.h"

#define MODULE_NAME " [TcpServer] "

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
    pool.setMaxThreadCount(10);

    qInfo() << "Max thread count: " << pool.maxThreadCount();
    LogOutputEmitter(LOG_DEBUG, QString("Max thread count: %1").arg(pool.maxThreadCount()));

    m_timer.setInterval(1000);
    m_timer.start();
}

TcpServer::~TcpServer()
{
    pool.deleteLater();
    this->close();
}

void TcpServer::Start(quint16 port)
{
    qInfo() << this << " Start " << QThread::currentThread();

    if (this->listen(QHostAddress::AnyIPv4, port))
    {
        qInfo() << "Mic Server started on " << port;
        LogOutputEmitter(LOG_INFO, QString("Mic server started on port: %1").arg(port));
        m_started = true;
    }
    else
    {
        LogOutputEmitter(LOG_ERROR, QString("Failed to start server on: %1").arg(this->errorString()));
        qCritical() << this->errorString();
        m_started = false;
    }
}

void TcpServer::Quit()
{
    m_activeConnections = 0;
    emit quit();;
    LogOutputEmitter(LOG_INFO, QString("Stopped TCP server"));
    pool.clear();
    m_started = false;
    this->close();
}

void TcpServer::incomingConnection(qintptr handle)
{
    qInfo() << "Incoming connection " << handle << " on " << QThread::currentThread();
    m_activeConnections++;

    LogOutputEmitter(LOG_INFO, QString("Incoming TCP connection.."));

    TcpHandler* client = new TcpHandler(nullptr,
                                          handle);
    client->setAutoDelete(true);

    connect(client, &TcpHandler::logOutput, this, &TcpServer::LogInputEmitter);
    connect(client, &TcpHandler::closeConn, this, &TcpServer::onCloseConn);
    connect(&m_timer, &QTimer::timeout, client, &TcpHandler::onTimeout);
    connect(this, &TcpServer::quit, client, &TcpHandler::onQuit);

    pool.start(client);
}

void TcpServer::onCloseConn()
{
    m_activeConnections--;
}

quint32 TcpServer::ActiveConnections() const
{
    return m_activeConnections;
}

bool TcpServer::Started() const
{
    return m_started;
}

void TcpServer::LogInputEmitter(log_level_t level, const QString &message)
{
    emit logOutput(level, message);
}

void TcpServer::LogOutputEmitter(log_level_t level, const QString &message)
{
    QString debugMessage(message);
    logger.GetLogMessage(level, MODULE_NAME, debugMessage);
    emit logOutput(level, debugMessage);
}
