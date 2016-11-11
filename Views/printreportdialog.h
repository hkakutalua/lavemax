#ifndef PRINTREPORTDIALOG_H
#define PRINTREPORTDIALOG_H

#include <QDialog>
#include "../Helpers/databasehelper.h"
#include "../Helpers/receiptprinterhelper.h"
#include "../Helpers/errorhelper.h"

namespace Ui {
class PrintReportDialog;
}

class PrintReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintReportDialog(QWidget *parent = 0);
    ~PrintReportDialog();

private slots:
    void printReport();

private:
    void loadReceipts();

    QList<DatabaseType::Receipt> receipts;
    Ui::PrintReportDialog *ui;
};

#endif // PRINTREPORTDIALOG_H
