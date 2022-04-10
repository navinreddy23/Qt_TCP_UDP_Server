#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "Serial.h"

#define MODULE_NAME " [Serial] "

Serial::Serial(QObject *parent) : QObject{parent}
{
    connect(&m_serial, &QSerialPort::readyRead, this, &Serial::onReadyRead);
}

Serial::~Serial()
{
    if (m_isInit)
    {
        m_serial.clear();
        m_serial.close();
    }
}

void Serial::onReadyRead()
{
    quint32 size = 0;
    m_serialData.append(m_serial.readAll());

    size = m_serialData.size();

    if((size > 0) && (m_serialData.at(size - 1) == '\n'))
    {
        ProcessLine(m_serialData);
        m_serialData.clear();
    }
}

void Serial::Init()
{
    QList<QSerialPortInfo> list;
    QString port = "";

    m_serialData.clear();

    list = QSerialPortInfo::availablePorts();

    foreach(QSerialPortInfo info, list)
    {
        if(info.description() == "FT230X Basic UART" || info.manufacturer() == "FTDI" || info.description() == "USB Serial Port" )
        {
            port = info.portName();
            break;
        }
    }

    if(port.isEmpty())
    {
        LogOutputEmitter(LOG_ERROR, "Serial device not connected");
        return;
    }

    qInfo() << "port name: " << port;

    m_serial.setBaudRate(QSerialPort::Baud38400);
    m_serial.setPortName(port);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setStopBits(QSerialPort::OneStop);

    if (!m_serial.open(QIODevice::ReadWrite))
    {
        qInfo() << m_serial.errorString();
        LogOutputEmitter(LOG_ERROR, m_serial.errorString());
    }
    else
    {
        m_isInit = true;
        qInfo() << "Serial port opened: " << m_serial.portName();
        LogOutputEmitter(LOG_INFO, "Serial port opened: " + m_serial.portName());
    }
}

bool Serial::IsInitialized()
{
    return  m_isInit;
}

void Serial::Open()
{
    Init();
}

void Serial::Close()
{
    m_serial.clear();
    m_serial.close();
    m_isInit = false;
}

void Serial::SendSerialCmd(const QString &cmd)
{
    QByteArray data = cmd.toUtf8();

    qInfo() << data;
    m_serial.write(data);
    m_serial.waitForBytesWritten();
}

void Serial::ProcessLine(QByteArray &data)
{
    QString dataStr(data);

    QStringList list = dataStr.split('\n');

    foreach(QString str, list)
    {
        if(!str.isEmpty())
        {
            str.remove('\r');
            str.remove('\n');
            str.remove(QChar(0x1B));
            str.remove("[33m");
            str.remove("[31m");
            str.remove("[0m");
            LogOutputEmitter(LOG_VERBOSE, str);
            emit stringRxd(str);
        }
    }
}

void Serial::LogOutputEmitter(log_level_t level, const QString &message)
{
    QString debugMessage(message);
    logger.GetLogMessage(level, MODULE_NAME, debugMessage);
    emit logOutput(level, debugMessage);
}
