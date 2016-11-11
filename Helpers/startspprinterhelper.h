#ifndef STARTSPPRINTERHELPER_H
#define STARTSPPRINTERHELPER_H

#include <starmicronics/StarIOPort.h>
#include <QDebug>
#include <QApplication>
#include "databasehelper.h"

namespace StarTSPPrinterType {
    enum PrinterErrorType
    {
        PortNotOpened = -1,
        WriteDataFail = -2,
        WriteDataIncomplete = -3
    };

    // Text Alignment Codes
    const QString LeftAlignment       {"\x1b\x1d\x61\x30"};
    const QString CenterAlignment     {"\x1b\x1d\x61\x31"};
    const QString RightAlignment      {"\x1b\x1d\x61\x32"};

    // Emphasized Printing Codes
    const QString SetEmphPrinting     {"\x1b\x45"};
    const QString CancelEmphPrinting  {"\x1b\x46"};
    const QString SetUnderlineMode    {"\x1b\x2d\x48"};
    const QString CancelUnderlineMode {"\x1b\x2d\x0"};

    // Cut command code
    const QString CutCommand          {"\x1b\x64\x02"};

    // Open cash drawer code
    const QString OpenCashDrawer      {"\x07"};
}

class StarTSPPrinterHelper : public QObject
{
    Q_OBJECT

public:
    static StarTSPPrinterHelper& GetInstance()
    {
        static StarTSPPrinterHelper instance;
        return instance;
    }

    void Print(const QString &data);

signals:
    void printSuccess();
    void printError(StarTSPPrinterType::PrinterErrorType);

private:
    StarTSPPrinterHelper(QObject *parent = 0) : QObject(parent) {}
    StarTSPPrinterHelper(const StarTSPPrinterHelper&);
    const StarTSPPrinterHelper& operator=(const StarTSPPrinterHelper&);

    unsigned int printData(const QByteArray &data);

    QString _portName {"usbven:COM1"};
    QString _classTag {"StarTSPPrinterHelper"};
};

#endif // STARTSPPRINTERHELPER_H
