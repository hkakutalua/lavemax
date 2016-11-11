#ifndef RECEIPTPRINTERHELPER_H
#define RECEIPTPRINTERHELPER_H

#include <QString>
#include <QDebug>
#include "databasehelper.h"
#include "startspprinterhelper.h"

class ReceiptPrinterHelper
{
public:
    static ReceiptPrinterHelper& GetInstance()
    {
        static ReceiptPrinterHelper instance;
        return instance;
    }

    void PrintReceipt(const DatabaseType::Receipt &receipt,
                      bool printDuplicate = false);

private:
    ReceiptPrinterHelper() {}
    ReceiptPrinterHelper(const ReceiptPrinterHelper&);
    const ReceiptPrinterHelper& operator=(const ReceiptPrinterHelper&);

    QString prepareReceiptData(const DatabaseType::Receipt &receipt,
                               bool printDuplicate);
};

#endif // RECEIPTPRINTERHELPER_H
