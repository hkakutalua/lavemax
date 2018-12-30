#ifndef MANAGEPRICESDIALOG_H
#define MANAGEPRICESDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class ManagePricesDialog;
}

class ManagePricesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManagePricesDialog(QWidget *parent = 0);
    ~ManagePricesDialog();

private slots:
    void openChangeSinglePriceDialog();
    void updatePricesTable();

private:
    Ui::ManagePricesDialog *ui;
    QSqlQueryModel *pricesModel;
};

#endif // MANAGEPRICESDIALOG_H
