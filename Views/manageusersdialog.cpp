#include "manageusersdialog.h"
#include "ui_manageusersdialog.h"

ManageUsersDialog::ManageUsersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageUsersDialog)
{
    ui->setupUi(this);
}

ManageUsersDialog::~ManageUsersDialog()
{
    delete ui;
}
