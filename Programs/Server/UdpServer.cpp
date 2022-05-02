#include "UdpServer.h"
#include <QDebug>
#include <QNetworkDatagram>
#include <QtEndian>

#define MODULE_NAME " [UdpServer] "

UdpServer::UdpServer(QObject *parent) : QObject(parent)
{
    connect(&socket, &QUdpSocket::readyRead, this, &UdpServer::onReadReady);
    connect(&m_timer, &QTimer::timeout, this, &UdpServer::OnTimeout);

    m_timer.setInterval(1000);
    m_timer.start();
}

UdpServer::~UdpServer()
{
    socket.close();
}

void UdpServer::Start(quint16 port)
{
    if(!socket.bind(QHostAddress::AnyIPv4, port))
    {
        qInfo() << "Failed to start UDP server: "
                << socket.errorString();

        LogOutputEmitter(LOG_ERROR, QString("Failed to start UDP server: %1").arg(socket.errorString()));
        m_started = false;

        return;
    }

    m_started = true;

    qInfo() << "Started Mic UDP on "
            << socket.localAddress()
            << " : " << socket.localPort();

    LogOutputEmitter(LOG_INFO, QString("Started UDP server for on %1:%2").arg(socket.localAddress().toString()).arg(socket.localPort()));
}

void UdpServer::Quit()
{
    LogOutputEmitter(LOG_INFO, QString("Stopped UDP server"));
    m_started = false;
    socket.close();
}


void UdpServer::onReadReady()
{
    while(socket.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket.receiveDatagram();

        QByteArray request(datagram.data());
        QByteArray response;

        Q_UNUSED(request);

        m_packetsReceived++;
        m_bytesReceived += request.size();

        LogOutputEmitter(LOG_DEBUG, QString("Received data: %1").arg(m_packetsReceived));
        LogOutputEmitter(LOG_VERBOSE, QString("Packet Size: %1").arg(request.size()));

        socket.writeDatagram(datagram.makeReply(response));
    }
}

void UdpServer::LogInputEmitter(log_level_t level, const QString &message)
{
    emit logOutput(level, message);
}

void UdpServer::OnTimeout()
{
    LogOutputEmitter(LOG_INFO, "------------------------------");

    m_timeout++;

    quint64 dataRate = m_bytesReceived / m_timeout;
    quint64 packetRate = m_packetsReceived / m_timeout;

    LogOutputEmitter(LOG_INFO, QString("Data rate: %1 kB/sec").arg(dataRate/1024));
    LogOutputEmitter(LOG_INFO, QString("Packet rate: %1/sec").arg(packetRate));
}

bool UdpServer::Started() const
{
    return m_started;
}

void UdpServer::LogOutputEmitter(log_level_t level, const QString &message)
{
    QString debugMessage(message);
    logger.GetLogMessage(level, MODULE_NAME, debugMessage);
    emit logOutput(level, debugMessage);
}

