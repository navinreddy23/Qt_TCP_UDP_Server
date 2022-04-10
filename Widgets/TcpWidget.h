#ifndef TCPWIDGET_H
#define TCPWIDGET_H

#include <QHash>
#include <QWidget>
#include <QTimer>
#include <QLineEdit>
#include "Programs/Misc/Logger.h"
#include "Programs/Server/TcpServer.h"

namespace Ui {
class MicTcpWidget;
}

class MicTcpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MicTcpWidget(QWidget *parent = nullptr);
    ~MicTcpWidget();

signals:
    emit void setStatus(const QString& message);
    emit void logChanged(void);

private slots:
    void on_btnMicTcpStartStop_clicked();

    void on_cmbMicTcpPort_currentIndexChanged(int index);

    void on_cmbMicTcpLogLevel_currentIndexChanged(int index);

    void UpdateUiElements(void);

    void logOutput(log_level_t level, const QString& message);

    void UpdateDeviceList(const QString& device, const QString& version);

    void on_teMicTcp_textChanged();

private:
    Ui::MicTcpWidget *ui;

    QTimer m_timer;
    quint16 m_port = 0;
    bool m_started = false;
    TcpServer* m_server = nullptr;
    log_level_t m_logLevel = LOG_INFO;

    void Init(void);
    void AssignOtaType(void);
    void AssignPort(void);
    void AssignLogLevel(void);

    void DisableUiElementsOnStart();
    void EnableUiElements();
    void SetStatus(const QString& message);

    void StartServer(void);
    void StopServer(void);

};

#endif // TCPWIDGET_H
