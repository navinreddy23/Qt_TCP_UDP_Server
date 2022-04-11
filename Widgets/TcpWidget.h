#ifndef TCPWIDGET_H
#define TCPWIDGET_H

#include <QHash>
#include <QWidget>
#include <QTimer>
#include <QLineEdit>
#include "Programs/Misc/Logger.h"
#include "Programs/Serial/Serial.h"
#include "Programs/Server/TcpServer.h"

namespace Ui {
class MicTcpWidget;
}

class TcpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpWidget(QWidget *parent = nullptr);
    ~TcpWidget();

signals:
    emit void setStatus(const QString& message);
    emit void logChanged(void);

private slots:
    void on_btnMicTcpStartStop_clicked();

    void on_cmbMicTcpLogLevel_currentIndexChanged(int index);

    void UpdateUiElements(void);

    void logOutput(log_level_t level, const QString& message);

    void UpdateDeviceList(const QString& device, const QString& version);

    void on_teMicTcp_textChanged();

    void on_lePort_textChanged(const QString &arg1);

    void on_btnSerial_clicked();

    void on_leSerial_returnPressed();

private:
    Ui::MicTcpWidget *ui;

    QTimer m_timer;
    quint16 m_port = 0;
    bool m_started = false;
    TcpServer* m_server = nullptr;
    log_level_t m_logLevel = LOG_INFO;

    Serial* m_serial = nullptr;
    bool m_opened = false;

    void Init(void);
    void LoadSettings();
    void SaveSettings();

    void AddSerialPorts(const QStringList &list);
    void AssignPort(void);
    void AssignLogLevel(void);

    void DisableUiElementsOnStart();
    void EnableUiElements();
    void SetStatus(const QString& message);

    void StartServer(void);
    void StopServer(void);

};

#endif // TCPWIDGET_H
