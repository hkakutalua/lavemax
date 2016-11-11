#include "manageclientsdialog.h"
#include "ui_manageclientsdialog.h"

ManageClientsDialog::ManageClientsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageClientsDialog)
{
    ui->setupUi(this);
}

ManageClientsDialog::~ManageClientsDialog()
{
    delete ui;
}
