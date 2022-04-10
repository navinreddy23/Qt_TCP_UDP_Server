#include <QTime>
#include <QDebug>
#include <QDialog>
#include <QSettings>
#include <QGroupBox>
#include <QDateTime>
#include <QFontDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QCoreApplication>
#include <QDesktopServices>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Dialogs/AboutDialog.h"

#define KEY_SAVEDIR "Save Dir"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Init();
    LoadSettings();
}

MainWindow::~MainWindow()
{
    SaveSettings();
    delete ui;
}

void MainWindow::on_actionClear_Log_triggered()
{
    QTextEdit* textEditor = GetCurrentTextEditPtr();

    if (textEditor != nullptr)
    {
        textEditor->selectAll();
        textEditor->cut();
    }
}

void MainWindow::on_actionUndo_triggered()
{
    QTextEdit* textEditor = GetCurrentTextEditPtr();

    if (textEditor != nullptr)
        textEditor->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    QTextEdit* textEditor = GetCurrentTextEditPtr();

    if (textEditor != nullptr)
        textEditor->redo();
}

void MainWindow::on_actionFont_triggered()
{
    bool ok;
    QFont font;
    QTextEdit* textEditor = GetCurrentTextEditPtr();

    if (textEditor == nullptr)
        return;

    font = QFontDialog::getFont(&ok, textEditor->currentFont(), this, "Choose a font");
    if(ok) textEditor->setCurrentFont(font);
}

void MainWindow::on_actionSave_triggered()
{
    SaveLog();
}

void MainWindow::on_actionSave_All_triggered()
{
    SaveAllLogs();
}

void MainWindow::on_actionOpen_triggered()
{
    OpenLog();
}


void MainWindow::on_actionSave_At_triggered()
{
    SaveAt();
}

void MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/navinreddy23"));
}

void MainWindow::onLogChanged()
{
    m_logChanged = true;
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog* dlg = new AboutDialog (this);
    dlg->exec();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    this->setWindowTitle(ui->tabWidget->tabText(index));
}

void MainWindow::on_actionExit_triggered()
{
    SaveSettings();
    this->close();
}

void MainWindow::Init()
{
    this->setCentralWidget(ui->tabWidget);

    m_micUdpWidget = new MicUdpWidget(this);
    ui->tabWidget->addTab(m_micUdpWidget, "UDP Server");
    connect(m_micUdpWidget, &MicUdpWidget::setStatus, this, &MainWindow::SetStatus);
    connect(m_micUdpWidget, &MicUdpWidget::logChanged, this, &MainWindow::onLogChanged);

    m_micTcpWidget = new MicTcpWidget(this);
    ui->tabWidget->addTab(m_micTcpWidget, "TCP Server");
    connect(m_micTcpWidget, &MicTcpWidget::setStatus, this, &MainWindow::SetStatus);
    connect(m_micTcpWidget, &MicTcpWidget::logChanged, this, &MainWindow::onLogChanged);

    m_saveDir.clear();
}

void MainWindow::LoadSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup(this->objectName());
    m_saveDir = settings.value(KEY_SAVEDIR).toString();
    settings.endGroup();
}

void MainWindow::SaveSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup(this->objectName());
    settings.setValue(KEY_SAVEDIR, m_saveDir);
    settings.endGroup();
}

void MainWindow::SaveToFile(const QString path, QTextEdit *textEdit)
{
    QFile file(path);

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this,"Error", file.errorString());
        SetStatus("Error could not save file!");
        return;
    }

    QTextStream stream(&file);

    stream << textEdit->toHtml();
    file.close();

    SetStatus("Saved: " + path);
}

void MainWindow::SaveLog()
{
    if(!CheckSaveDirectory())
        return;

    QTextEdit* textEdit = GetCurrentTextEditPtr();

    if (textEdit == nullptr)
        return;

    QString path = m_saveDir + QDir::separator() +
            ui->tabWidget->tabText(ui->tabWidget->currentIndex()) + "_"
            + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH.mm.ss") + ".html";

    qInfo() << "Save Dir" << path;

    SaveToFile(path, textEdit);
}

void MainWindow::SaveAllLogs()
{
    if(!CheckSaveDirectory())
        return;

    for (int i = 0; i < ui->tabWidget->count(); i++)
    {
        QTextEdit* textEdit = GetTextEditPtr(i);
        if (textEdit != nullptr)
        {
            QString path = m_saveDir + QDir::separator() + ui->tabWidget->tabText(i) + "_" +
                     QDateTime::currentDateTime().toString("yyyy-MM-dd_HH.mm.ss") + ".html";
            path = QDir::toNativeSeparators(path);
            SaveToFile(path, textEdit);
        }
    }

    m_logChanged = false;

    SetStatus("Saved logs from all the tabs");
}

void MainWindow::SaveAt()
{
    QString path;
    path = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!path.isEmpty())
        m_saveDir = path;
}

void MainWindow::OpenLog()
{
    QString path;

    if (m_saveDir.isEmpty())
    {
        path = QFileDialog::getOpenFileName(this, tr("Open Saved Log (*.html)"), QDir::homePath(), "HTML files (*.html)");
    }
    else
    {
        path = QFileDialog::getOpenFileName(this, tr("Open Saved Log (*.html)"), m_saveDir, "HTML files (*.html)");
    }

    if (!path.isEmpty())
    {
        SetStatus("Opened: " + path);
    }
    else
    {
        SetStatus("Open Failed: File not specified");
        return;
    }

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this,"Error", file.errorString());
        return;
    }

    QTextStream stream(&file);

    QTextEdit* textEdit = GetCurrentTextEditPtr();
    if (textEdit == nullptr)
        return;

    //Read in HTML format.
    textEdit->setHtml(stream.readAll());
        file.close();
}

void MainWindow::SetStatus(const QString &status)
{
    ui->statusbar->showMessage(QTime::currentTime().toString() + "  -  " +  status);
}

bool MainWindow::CheckSaveDirectory()
{
    if (m_saveDir.isEmpty())
    {
        m_saveDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if(m_saveDir.isEmpty())
        {
            SetStatus("Save Failed: Direcotry not specified");
            return false;
        }
    }

    return true;
}

QTextEdit *MainWindow::GetCurrentTextEditPtr()
{
    QTextEdit* ptr = nullptr;

    QWidget* widget = qobject_cast<QWidget*>(ui->tabWidget->currentWidget());

    if (widget == nullptr)
    {
        return ptr;
    }

    foreach(QObject* obj, widget->children())
    {
        QGroupBox* tempPtr = qobject_cast<QGroupBox*>(obj);

        if(tempPtr != nullptr)
        {
            foreach(QObject* obj, tempPtr->children())
            {
                ptr = qobject_cast<QTextEdit*>(obj);

                if(ptr != nullptr)
                {
                    break;
                }
            }

            if (ptr != nullptr)
            {
                break;
            }
        }
    }

    return ptr;
}

QTextEdit *MainWindow::GetTextEditPtr(quint8 tabIndex)
{
    QTextEdit* ptr = nullptr;

    QWidget* widget = qobject_cast<QWidget*>(ui->tabWidget->widget(tabIndex));

    if (widget == nullptr)
    {
        return ptr;
    }

    foreach(QObject* obj, widget->children())
    {
        QGroupBox* tempPtr = qobject_cast<QGroupBox*>(obj);

        if(tempPtr != nullptr)
        {
            foreach(QObject* obj, tempPtr->children())
            {
                ptr = qobject_cast<QTextEdit*>(obj);

                if(ptr != nullptr)
                {
                    break;
                }
            }
        }
    }

    return ptr;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    SaveSettings();

    if (m_logChanged)
    {
        QMessageBox msgBox;
        msgBox.setText("The logs have been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");

        msgBox.setStandardButtons(QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.addButton(tr("Save At"), QMessageBox::ActionRole);
        msgBox.setDefaultButton(QMessageBox::SaveAll);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::SaveAll:
            SaveAllLogs();
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            event->ignore();
            return;
            break;
        default:
            SaveAt();
            break;
        }
    }

    event->accept();
}
