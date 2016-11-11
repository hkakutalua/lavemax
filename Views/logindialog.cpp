#include "logindialog.h"
#include "ui_logindialog.h"

using namespace UserSessionType;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setUpConnections();
    setErrorLabelVisible(false);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setUpConnections()
{
    connect(ui->LoginButton, SIGNAL(clicked()),
            this, SLOT(loginUser()));
    connect(ui->ExitButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(&mainWindow, SIGNAL(WindowClosed()),
            this, SLOT(show()));
}

void LoginDialog::loginUser()
{
    QString login    = ui->UserLoginEntry->text();
    QString password = ui->PasswordEntry->text();

    UserSessionHelper& session = UserSessionHelper::GetInstance();
    if (session.LoginUser(login, password)) {
        setErrorLabelVisible(false);
        clearLoginField();

        mainWindow.showMaximized();
        this->hide();
    }
    else {
        setErrorLabelVisible(true);
    }
}

void LoginDialog::setErrorLabelVisible(bool visible)
{
    ui->InvalidLoginErrorLabel->setVisible(visible);
}

void LoginDialog::clearLoginField()
{
    ui->UserLoginEntry->clear();
    ui->PasswordEntry->clear();
    ui->UserLoginEntry->setFocus();
}
