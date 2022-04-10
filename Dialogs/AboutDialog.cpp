#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    Init();
    DisplayLogo();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_buttonBox_accepted()
{
    this->accept();
}

void AboutDialog::Init()
{
    this->setWindowTitle("About");
    ui->lblAppName->setText(QApplication::applicationName());
    ui->lblOrgName->setText(QApplication::organizationName());
    ui->lblURL->setText(QApplication::organizationDomain());
    ui->lblVersion->setText("v" + QString(GIT_VERSION));
    ui->lblGitHash->setText("Git Hash: " + QString(GIT_HASH));
    ui->lblGitBranch->setText("Git Branch: " + QString(GIT_BRANCH));
    ui->lblBuildDate->setText(BUILD_TIMESTAMP);
}

void AboutDialog::DisplayLogo()
{
    ui->lblLogo->setStyleSheet("image: url(:/files/Images/app-icon.png);");
    ui->lblAppIcon->setStyleSheet("image: url(:/files/Images/server.png);");
}

