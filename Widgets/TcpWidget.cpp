#include <QFile>
#include <QSettings>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>

#include "TcpWidget.h"
#include "ui_MicTcpWidget.h"

MicTcpWidget::MicTcpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MicTcpWidget)
{
    ui->setupUi(this);

    Init();
}

MicTcpWidget::~MicTcpWidget()
{
    delete ui;
}

void MicTcpWidget::on_btnMicTcpStartStop_clicked()
{
    if (!m_started)
        StartServer();
    else
        StopServer();
}

void MicTcpWidget::on_cmbMicTcpPort_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AssignPort();
}

void MicTcpWidget::on_cmbMicTcpLogLevel_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AssignLogLevel();
}

void MicTcpWidget::UpdateUiElements()
{
    if (m_server != nullptr)
    {
        ui->lblMicTcpActiveConn->setText(QString("%1").arg(m_server->ActiveConnections()));
    }
}

void MicTcpWidget::logOutput(log_level_t level, const QString &message)
{
    if (level > LOG_NONE && level <= m_logLevel)
    {
        ui->teMicTcp->append(QTime::currentTime().toString() + "  -  " + message);
        ui->teMicTcp->verticalScrollBar()->minimum();
    }
}

void MicTcpWidget::on_teMicTcp_textChanged()
{
    emit logChanged();
}

void MicTcpWidget::UpdateDeviceList(const QString &device, const QString &version)
{
    SetStatus("Mic (TCP) Updated: " + device + "\t to \t" + version);
}

void MicTcpWidget::Init()
{
    m_timer.setInterval(1000);
    m_timer.start();

    connect(&m_timer, &QTimer::timeout, this, &MicTcpWidget::UpdateUiElements);

    m_started = false;
    /* ui->btnMicTcpStartStop->setIcon(QIcon(":/files/Images/start.png")); */
    ui->btnMicTcpStartStop->setStyleSheet("QPushButton {background-color: ForestGreen; color: white;}");

    AssignOtaType();
    AssignPort();
}

void MicTcpWidget::AssignOtaType()
{

}

void MicTcpWidget::AssignPort()
{
    m_port = ui->cmbMicTcpPort->currentText().toUShort();
}

void MicTcpWidget::AssignLogLevel()
{
    m_logLevel = (log_level_t) ui->cmbMicTcpLogLevel->currentIndex();
}

void MicTcpWidget::DisableUiElementsOnStart()
{
    ui->cmbMicTcpPort->setEnabled(false);
}

void MicTcpWidget::EnableUiElements()
{
    ui->cmbMicTcpPort->setEnabled(true);
}

void MicTcpWidget::SetStatus(const QString &message)
{
    emit setStatus(message);
}

void MicTcpWidget::StartServer()
{
    if (m_server == nullptr)
    {
        m_server = new TcpServer(this);
        connect(m_server, &TcpServer::logOutput, this, &MicTcpWidget::logOutput);
    }

    m_server->Start(m_port);

    m_started = m_server->Started();

    if (m_started)
    {
        ui->btnMicTcpStartStop->setStyleSheet("QPushButton {background-color: DarkRed; color: white;}");
        ui->btnMicTcpStartStop->setText("Stop");
        DisableUiElementsOnStart();
        SetStatus(QString("Started Mic Server On Port: %1").arg(m_port));
    }
    else
    {
        QMessageBox::critical(this, "Mic Server Start Failed", QString("Starting server on port %1 failed").arg(m_port));
        SetStatus(QString("Starting server on port %1 failed").arg(m_port));
    }
}

void MicTcpWidget::StopServer()
{
    if (m_server != nullptr)
    {
        m_server->Quit();
        m_started = m_server->Started();
        delete m_server;
        m_server = nullptr;
        EnableUiElements();
    }

    SetStatus(QString("Stopped Mic Server On Port: %1").arg(m_port));

    if (!m_started)
    {
        ui->btnMicTcpStartStop->setText("Start");
        ui->btnMicTcpStartStop->setStyleSheet("QPushButton {background-color: ForestGreen; color: white;}");
    }
}



