#ifndef VISUALIZERECEIPTDIALOG_H
#define VISUALIZERECEIPTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include "../Helpers/receiptprinterhelper.h"
#include "../Helpers/databasehelper.h"

namespace Ui {
class VisualizeReceiptDialog;
}

class VisualizeReceiptDialog : public QDialog
{
    Q_OBJECT

public:
    VisualizeReceiptDialog(unsigned int receiptNumber,
                           QWidget *parent = 0);
    ~VisualizeReceiptDialog();

private slots:
    void setUpConnections();

    void updateReceiptData();
    void updateReceiptPaymData();
    void updateClothData();
    void updateClientData();
    void updateUserData();

    void updateReceiptUi();
    void updateReceiptPaymUi();
    void updateClothUi(QAbstractItemModel *model);
    void updateClientUi();
    void updateUserUi();

    void reprintReceipt();

private:
    QString               _tag;
    unsigned int          _receiptNumber {0};
    unsigned int          _clientId      {0};
    unsigned int          _userId        {0};
    DatabaseType::Receipt _receipt;

    Ui::VisualizeReceiptDialog *ui;
};

#endif // VISUALIZERECEIPTDIALOG_H
