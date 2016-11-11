#ifndef REPORTPRINTHELPER_H
#define REPORTPRINTHELPER_H

#include <QString>
#include <QDebug>
#include "databasehelper.h"
#include "startspprinterhelper.h"

class ReportPrintHelper
{
public:
    static ReportPrintHelper& GetInstance()
    {
        static ReportPrintHelper instance;
        return instance;
    }

    void PrintReport(const QList<DatabaseType::Receipt> &receipts);

private:
    ReportPrintHelper() {}
    ReportPrintHelper(const ReportPrintHelper&);
    const ReportPrintHelper& operator=(const ReportPrintHelper&);

    QString prepareReceiptData(const QList<DatabaseType::Receipt> &receipts);
};

#endif // REPORTPRINTHELPER_H
