#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QApplication>
#include <QShowEvent>
#include "Views/mainwindow.h"
#include "Helpers/usersessionhelper.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void setUpConnections();
    void loginUser();
    void setErrorLabelVisible(bool visible);
    void clearLoginField();

private:
    MainWindow mainWindow;
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
