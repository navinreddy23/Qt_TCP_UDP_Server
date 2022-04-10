#ifndef UDPWIDGET_H
#define UDPWIDGET_H

#include <QHash>
#include <QTimer>
#include <QWidget>
#include <QLineEdit>
#include "Programs/Misc/Logger.h"
#include "Programs/Server/UdpServer.h"

namespace Ui {
class MicUdpWidget;
}

class MicUdpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MicUdpWidget(QWidget *parent = nullptr);
    ~MicUdpWidget();

private slots:
    void on_btnMicUdpStartStop_clicked();

    void on_cmbMicUdpLogLevel_currentIndexChanged(int index);

    void UpdateUiElements(void);

    void logOutput(log_level_t level, const QString& message);

    void on_teMicUdp_textChanged();

signals:
    emit void setStatus(const QString& message);
    emit void logChanged(void);


private:
    Ui::MicUdpWidget *ui;

    QTimer m_timer;
    quint16 m_port = 0;
    bool m_started = false;
    UdpServer* m_server = nullptr;
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

#endif // UDPWIDGET_H
