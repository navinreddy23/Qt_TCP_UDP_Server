#include <QHostAddress>
#include "TcpHandler.h"

#define IDLE_TIMEOUT_SEC    10
#define MODULE_NAME         " [TcpHandler] "

Q_DECLARE_METATYPE(log_level_t)

TcpHandler::TcpHandler(QObject *parent, qintptr handle) : QObject(parent), QRunnable()
{
    this->handle = handle;

    if(!socket->setSocketDescriptor(this->handle))
    {
        qCritical() << socket->errorString();
        LogOutputEmitter(LOG_ERROR, "Socket error: " + socket->errorString() );
        delete socket;
        return;
    }

    qRegisterMetaType<log_level_t>();

    /* Connect relevant signals and slots */
    connect(socket,     &QTcpSocket::readyRead,     this, &TcpHandler::onReadyRead,      Qt::QueuedConnection);
    connect(socket,     &QTcpSocket::disconnected,  this, &TcpHandler::close,            Qt::QueuedConnection);
}

TcpHandler::~TcpHandler()
{
}

void TcpHandler::run()
{
    qInfo() << this << " run " << QThread::currentThread();
    LogOutputEmitter(LOG_VERBOSE, QString("Thread started for TCP client"));

    while(QThread::currentThread()->isRunning())
    {
        QThread::currentThread()->sleep(1);

        if (m_quit)
        {
            return;
        }

        if (m_idle >= IDLE_TIMEOUT_SEC)
        {
            LogOutputEmitter(LOG_DEBUG, "Thread quitting: Idle for long time.");
            emit closeConn();
            return;
        }
    }
}

void TcpHandler::close()
{
    qInfo() << "Closing TCP socket: " << socket;
    emit closeConn();
    LogOutputEmitter(LOG_DEBUG, QString("Closing TCP connection: %1").arg((qlonglong)socket));

    socket->close();
    socket->deleteLater();

    m_quit = true;
}

void TcpHandler::onReadyRead()
{
    QByteArray request;
    QByteArray response;

    request = socket->readAll();

    socket->write(response);
    socket->waitForBytesWritten();

    m_idle = 0;
}

void TcpHandler::onTimeout()
{
    m_idle++;
}

void TcpHandler::onQuit()
{
    socket->close();
    m_quit = true;
}

void TcpHandler::LogInputEmitter(log_level_t level, const QString &message)
{
    emit logOutput(level, message);
}

void TcpHandler::LogOutputEmitter(log_level_t level, const QString &message)
{
    QString debugMessage(message);
    logger.GetLogMessage(level, MODULE_NAME, debugMessage);
    emit logOutput(level, debugMessage);

}
