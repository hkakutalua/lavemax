#ifndef FINANCIALREPORTDIALOG_H
#define FINANCIALREPORTDIALOG_H

#include <QDialog>
#include <QDebug>
#include "Helpers/databasehelper.h"
#include "Helpers/reportprinthelper.h"

namespace Ui {
class FinancialReportDialog;
}

namespace FinancialReportType {
    enum Shift
    {
        Morning,
        Afternoon,
        Night
    };
}

class FinancialReportDialog : public QDialog
{
    Q_OBJECT

public:
    FinancialReportDialog(QWidget *parent = 0);
    ~FinancialReportDialog();

private slots:
    void setUpConnections();
    void updateShiftData();
    void printReport();

private:
    QSqlQuery queryReceiptsBetween(QDateTime beginDate,
                                   QDateTime endDate);
    QSqlQuery queryPaymentsBetween(QDateTime beginDate,
                                   QDateTime endDate);
    QList<DatabaseType::Cloth> queryReceiptClothes(uint receiptNumber);
    DatabaseType::Client       queryReceiptClient (uint clientId);
    DatabaseType::UserAccount  queryReceiptUser   (uint userId);

    void updateReceiptData();
    void updateReceiptPaymData();
    void updateClothData(uint receiptNumber);
    void updateClientData(uint receiptNumber);
    void updateUserData(uint receiptNumber);

    QString _classTag {"FinancialReportDialog"};

    FinancialReportType::Shift _shift{FinancialReportType::Morning};
    QList<DatabaseType::Receipt> _receiptList;

    QTime _shiftBegin;
    QTime _shiftEnd;

    QTime _morningShiftBegin;
    QTime _morningShiftEnd;

    QTime _afternoonShiftBegin;
    QTime _afternoonShiftEnd;

    QTime _nightShiftBegin;
    QTime _nightShiftEnd;

    Ui::FinancialReportDialog *ui;
};

#endif // FINANCIALREPORTDIALOG_H
