#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QCloseEvent>
#include "Widgets/UdpWidget.h"
#include "Widgets/TcpWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    emit void logUpdatedSerialNumbers(const QString dirPath, const QString email, const QString pwd);

private slots:
    void on_actionClear_Log_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionFont_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_All_triggered();

    void on_actionSave_At_triggered();

    void on_actionHelp_triggered();

    void onLogChanged(void);

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    QString m_saveDir;
    QString m_email = "";
    QString m_pwd = "";
    bool m_initFailed = false;
    bool m_logChanged = false;

    UdpWidget* m_micUdpWidget = nullptr;
    TcpWidget* m_micTcpWidget = nullptr;

    void Init();
    void LoadSettings();
    void SaveSettings();
    void SaveToFile(const QString path, QTextEdit* textEdit);
    void SaveLog();
    void SaveAllLogs();
    void SaveAt();
    void OpenLog();
    void SetStatus(const QString& status);
    bool CheckSaveDirectory(void);

    /* This function assumes that there is only one textEdit Widget per Tab */
    QTextEdit* GetCurrentTextEditPtr(void);
    QTextEdit* GetTextEditPtr(quint8 tabIndex);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
