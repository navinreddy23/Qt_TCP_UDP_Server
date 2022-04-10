#include <QFile>
#include <QSettings>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>

#include "UdpWidget.h"
#include "ui_UdpWidget.h"

MicUdpWidget::MicUdpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MicUdpWidget)
{
    ui->setupUi(this);

    Init();
}

MicUdpWidget::~MicUdpWidget()
{
    delete ui;
}

void MicUdpWidget::on_btnMicUdpStartStop_clicked()
{
    if (!m_started)
        StartServer();
    else
        StopServer();
}

void MicUdpWidget::on_cmbMicUdpLogLevel_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AssignLogLevel();
}

void MicUdpWidget::UpdateUiElements()
{
}

void MicUdpWidget::logOutput(log_level_t level, const QString &message)
{
    if (level > LOG_NONE && level <= m_logLevel)
    {
        ui->teMicUdp->append(QTime::currentTime().toString() + "  -  " + message);
        ui->teMicUdp->verticalScrollBar()->minimum();
    }
}

void MicUdpWidget::on_teMicUdp_textChanged()
{
    emit logChanged();
}

void MicUdpWidget::Init()
{
    m_timer.setInterval(1000);
    m_timer.start();

    connect(&m_timer, &QTimer::timeout, this, &MicUdpWidget::UpdateUiElements);

    m_started = false;
    ui->btnMicUdpStartStop->setStyleSheet("QPushButton {background-color: ForestGreen; color: white;}");
   /*  ui->btnMicUdpStartStop->setIcon(QIcon(":/files/Images/start.png")); */

    AssignPort();
}

void MicUdpWidget::AssignPort()
{
    m_port = ui->lePort->text().toUShort();
}

void MicUdpWidget::AssignLogLevel()
{
    m_logLevel = (log_level_t) ui->cmbMicUdpLogLevel->currentIndex();
}

void MicUdpWidget::DisableUiElementsOnStart()
{
    ui->lePort->setEnabled(false);
}

void MicUdpWidget::EnableUiElements()
{
    ui->lePort->setEnabled(true);
}

void MicUdpWidget::SetStatus(const QString &message)
{
    emit setStatus(message);
}

void MicUdpWidget::StartServer()
{
    if (m_server == nullptr)
    {
        m_server = new UdpServer(this);
        connect(m_server, &UdpServer::logOutput, this, &MicUdpWidget::logOutput);
    }

    m_server->Start(m_port);

    m_started = m_server->Started();

    if (m_started)
    {
        ui->btnMicUdpStartStop->setStyleSheet("QPushButton {background-color: DarkRed; color: white;}");
        ui->btnMicUdpStartStop->setText("Stop");
        DisableUiElementsOnStart();
        SetStatus(QString("Started Mic Server On Port: %1").arg(m_port));
    }
    else
    {
        QMessageBox::critical(this, "Mic Server Start Failed", QString("Starting server on port %1 failed").arg(m_port));
        SetStatus(QString("Starting server on port %1 failed").arg(m_port));
    }
}

void MicUdpWidget::StopServer()
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
        ui->btnMicUdpStartStop->setText("Start");
        ui->btnMicUdpStartStop->setStyleSheet("QPushButton {background-color: ForestGreen; color: white;}");
    }
}



void MicUdpWidget::on_lePort_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    AssignPort();
}

