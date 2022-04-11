#include <QFile>
#include <QSettings>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>

#include "TcpWidget.h"
#include "ui_TcpWidget.h"

#define KEY_FONT            "tcpFont"
#define KEY_TCP_PORT        "tcpPort"

TcpWidget::TcpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MicTcpWidget)
{
    ui->setupUi(this);

    Init();
    LoadSettings();
}

TcpWidget::~TcpWidget()
{
    SaveSettings();
    delete ui;
}

void TcpWidget::on_btnMicTcpStartStop_clicked()
{
    if (!m_started)
        StartServer();
    else
        StopServer();
}

void TcpWidget::on_cmbMicTcpLogLevel_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AssignLogLevel();
}

void TcpWidget::UpdateUiElements()
{
    if (m_server != nullptr)
    {
        ui->lblMicTcpActiveConn->setText(QString("%1").arg(m_server->ActiveConnections()));
    }
    else
    {
        ui->lblMicTcpActiveConn->setText(QString("0"));
    }
}

void TcpWidget::logOutput(log_level_t level, const QString &message)
{
    if (level > LOG_NONE && level <= m_logLevel)
    {
        ui->teMicTcp->append(QTime::currentTime().toString() + "  -  " + message);
        ui->teMicTcp->verticalScrollBar()->minimum();
    }
}

void TcpWidget::on_teMicTcp_textChanged()
{
    emit logChanged();
}

void TcpWidget::UpdateDeviceList(const QString &device, const QString &version)
{
    SetStatus("Mic (TCP) Updated: " + device + "\t to \t" + version);
}

void TcpWidget::Init()
{
    m_timer.setInterval(1000);
    m_timer.start();

    connect(&m_timer, &QTimer::timeout, this, &TcpWidget::UpdateUiElements);

    m_serial = new Serial(this);
    connect(m_serial, &Serial::logOutput, this, &TcpWidget::logOutput);

    AddSerialPorts(m_serial->Scan());

    m_started = false;
    /* ui->btnMicTcpStartStop->setIcon(QIcon(":/files/Images/start.png")); */
    ui->btnMicTcpStartStop->setStyleSheet("QPushButton {background-color: ForestGreen; color: white;}");

    AssignPort();
}

void TcpWidget::LoadSettings()
{
    QFont font;
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup(this->objectName());

    font = settings.value(KEY_FONT, QFont()).value<QFont>();
    ui->lePort->setText(settings.value(KEY_TCP_PORT).toString());
    AssignPort();

    settings.endGroup();

    ui->teMicTcp->setCurrentFont(font);
}

void TcpWidget::SaveSettings()
{
    QFont font = ui->teMicTcp->currentFont();
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup(this->objectName());

    settings.setValue(KEY_FONT, font);
    settings.setValue(KEY_TCP_PORT, ui->lePort->text());

    settings.endGroup();
}

void TcpWidget::AddSerialPorts(const QStringList &list)
{
    ui->cmbSerialPort->clear();

    ui->cmbSerialPort->addItem("Select Port");
    ui->cmbSerialPort->addItems(list);
}

void TcpWidget::AssignPort()
{
    m_port = ui->lePort->text().toUShort();
}

void TcpWidget::AssignLogLevel()
{
    m_logLevel = (log_level_t) ui->cmbMicTcpLogLevel->currentIndex();
}

void TcpWidget::DisableUiElementsOnStart()
{
    ui->lePort->setEnabled(false);
}

void TcpWidget::EnableUiElements()
{
    ui->lePort->setEnabled(true);
}

void TcpWidget::SetStatus(const QString &message)
{
    emit setStatus(message);
}

void TcpWidget::StartServer()
{
    if (m_server == nullptr)
    {
        m_server = new TcpServer(this);
        connect(m_server, &TcpServer::logOutput, this, &TcpWidget::logOutput);
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

void TcpWidget::StopServer()
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

void TcpWidget::on_lePort_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    AssignPort();
}


void TcpWidget::on_btnSerial_clicked()
{
    if(m_opened)
    {
        m_serial->Close();

        ui->cmbSerialPort->setDisabled(false);
        ui->cmbBaud->setDisabled(false);

        m_opened = false;
        ui->btnSerial->setText("Open");

        AddSerialPorts(m_serial->Scan());
    }
    else
    {
        m_serial->Open(ui->cmbSerialPort->currentText(), ui->cmbBaud->currentText());

        if(m_serial->IsInitialized())
        {
            m_opened = true;

            ui->cmbSerialPort->setDisabled(true);
            ui->cmbBaud->setDisabled(true);

            ui->btnSerial->setText("Close");
        }
        else
        {
            AddSerialPorts(m_serial->Scan());
        }
    }
}


void TcpWidget::on_leSerial_returnPressed()
{
    if(m_opened)
    {
        m_serial->SendSerialCmd(ui->leSerial->text() + "\r\n");
        ui->leSerial->clear();
    }
}

