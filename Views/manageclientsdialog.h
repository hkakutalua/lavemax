#ifndef MANAGECLIENTSDIALOG_H
#define MANAGECLIENTSDIALOG_H

#include <QDialog>

namespace Ui {
class ManageClientsDialog;
}

class ManageClientsDialog : public QDialog
{
    Q_OBJECT

public:
    ManageClientsDialog(QWidget *parent = 0);
    ~ManageClientsDialog();

private:
    Ui::ManageClientsDialog *ui;
};

#endif // MANAGECLIENTSDIALOG_H
