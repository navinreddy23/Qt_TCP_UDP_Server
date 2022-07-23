#include <QFile>
#include <QSettings>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCompleter>

#include "UdpWidget.h"
#include "ui_UdpWidget.h"

#define KEY_FONT            "udpFont"
#define KEY_UDP_PORT        "udpPort"
#define KEY_SERIAL_LIST     "serialInputsList"
#define KEY_SERIAL_IP       "serialInput"

UdpWidget::UdpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MicUdpWidget)
{
    ui->setupUi(this);

    Init();
    LoadSettings();
    SetCompleter();
}

UdpWidget::~UdpWidget()
{
    SaveSettings();
    delete ui;
}

void UdpWidget::on_btnMicUdpStartStop_clicked()
{
    if (!m_started)
        StartServer();
    else
        StopServer();
}

void UdpWidget::on_cmbMicUdpLogLevel_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AssignLogLevel();
}

void UdpWidget::UpdateUiElements()
{
}

void UdpWidget::logOutput(log_level_t level, const QString &message)
{
    if (level > LOG_NONE && level <= m_logLevel)
    {
        ui->teMicUdp->append(QTime::currentTime().toString() + "  -  " + message);
        ui->teMicUdp->verticalScrollBar()->minimum();
    }
}

void UdpWidget::on_teMicUdp_textChanged()
{
    emit logChanged();
}

void UdpWidget::Init()
{
    m_timer.setInterval(1000);
    m_timer.start();

    connect(&m_timer, &QTimer::timeout, this, &UdpWidget::UpdateUiElements);

    m_serial = new Serial(this);
    connect(m_serial, &Serial::logOutput, this, &UdpWidget::logOutput);

    AddSerialPorts(m_serial->Scan());

    m_started = false;
    ui->btnMicUdpStartStop->setStyleSheet("QPushButton {background-color: ForestGreen; color: white;}");
   /*  ui->btnMicUdpStartStop->setIcon(QIcon(":/files/Images/start.png")); */

    AssignPort();
}

void UdpWidget::LoadSettings()
{
    QFont font;
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup(this->objectName());

    font = settings.value(KEY_FONT, QFont()).value<QFont>();
    ui->lePort->setText(settings.value(KEY_UDP_PORT).toString());
    AssignPort();

    m_serialInputs.clear();
    int size = settings.beginReadArray(KEY_SERIAL_LIST);
    for(int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);
        m_serialInputs.append(settings.value(KEY_SERIAL_IP).toString());
    }

    m_serialInputs.removeDuplicates();

    settings.endArray();

    settings.endGroup();

    ui->teMicUdp->setCurrentFont(font);
}

void UdpWidget::SetCompleter()
{
    m_serialInputs.removeDuplicates();
    QCompleter* completer = new QCompleter(m_serialInputs, this);
    ui->leSerial->setCompleter(completer);
}

void UdpWidget::SaveSettings()
{
    QFont font = ui->teMicUdp->currentFont();
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup(this->objectName());

    settings.setValue(KEY_FONT, font);
    settings.setValue(KEY_UDP_PORT, ui->lePort->text());

    settings.beginWriteArray(KEY_SERIAL_LIST);
    int i = 0;
    foreach (QString str, m_serialInputs)
    {
        settings.setArrayIndex(i);
        settings.setValue(KEY_SERIAL_IP, str);
        i++;
    }
    settings.endArray();

    settings.endGroup();
}

void UdpWidget::AddSerialPorts(const QStringList &list)
{
    ui->cmbSerialPort->clear();

    ui->cmbSerialPort->addItem("Select Port");
    ui->cmbSerialPort->addItems(list);
}

void UdpWidget::AssignPort()
{
    m_port = ui->lePort->text().toUShort();
}

void UdpWidget::AssignLogLevel()
{
    m_logLevel = (log_level_t) ui->cmbMicUdpLogLevel->currentIndex();
}

void UdpWidget::DisableUiElementsOnStart()
{
    ui->lePort->setEnabled(false);
}

void UdpWidget::EnableUiElements()
{
    ui->lePort->setEnabled(true);
}

void UdpWidget::SetStatus(const QString &message)
{
    emit setStatus(message);
}

void UdpWidget::StartServer()
{
    if (m_server == nullptr)
    {
        m_server = new UdpServer(this);
        connect(m_server, &UdpServer::logOutput, this, &UdpWidget::logOutput);
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

void UdpWidget::StopServer()
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

void UdpWidget::on_lePort_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    AssignPort();
}

void UdpWidget::on_btnSerial_clicked()
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


void UdpWidget::on_leSerial_returnPressed()
{
    if(m_opened)
    {
        m_serial->SendSerialCmd(ui->leSerial->text() + "\r\n");
        if(!ui->leSerial->text().isEmpty())
        {
            m_serialInputs.append(ui->leSerial->text());
            SetCompleter();
        }

        ui->leSerial->clear();
    }
}

