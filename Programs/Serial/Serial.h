#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "Programs/Misc/Logger.h"

class Serial : public QObject
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = nullptr);
    ~Serial();

    void Init();
    void Open(const QString& port, const QString& baudRate);
    QStringList Scan(void);
    bool IsInitialized(void);
    void Close();

signals:
    emit void logOutput(log_level_t level, const QString& message);
    emit void stringRxd(const QString &rxStr);

public slots:
    void SendSerialCmd(const QString& cmd);

private slots:
    void onReadyRead(void);

private:

    Logger logger;
    bool m_isInit = false;
    QSerialPort m_serial;
    QByteArray m_serialData;

    void ProcessLine(QByteArray& data);
    void LogOutputEmitter(log_level_t level, const QString &message);

};

#endif // SERIAL_H
