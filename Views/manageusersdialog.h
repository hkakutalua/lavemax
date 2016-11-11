#ifndef MANAGEUSERSDIALOG_H
#define MANAGEUSERSDIALOG_H

#include <QDialog>

namespace Ui {
class ManageUsersDialog;
}

class ManageUsersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManageUsersDialog(QWidget *parent = 0);
    ~ManageUsersDialog();

private:
    Ui::ManageUsersDialog *ui;
};

#endif // MANAGEUSERSDIALOG_H
