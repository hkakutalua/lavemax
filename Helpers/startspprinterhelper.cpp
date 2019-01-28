#include "startspprinterhelper.h"

using namespace StarTSPPrinterType;

void StarTSPPrinterHelper::Print(const QString &data)
{
    QByteArray byteArray;
    byteArray.append(data.toLatin1());

    unsigned int totalSizeCommunicated = printData(byteArray);
    if (totalSizeCommunicated == PortNotOpened) {
        qDebug() << "Failure: OpenPort";
        emit printError(PortNotOpened);
    }
    if (totalSizeCommunicated == WriteDataFail) {
        qDebug() << "Failure: WritePort";
        emit printError(WriteDataFail);
    }
    else if (totalSizeCommunicated != byteArray.size()) {
        qDebug() << "Print Error: Not all data could be written out!";
        emit printError(WriteDataIncomplete);
    }
    else {
        qDebug() << "Successful print!!";
        emit printSuccess();
    }
}

unsigned int StarTSPPrinterHelper::printData(const QByteArray &data)
{
    void *port = OpenPortA(_portName.toStdString().c_str(), "", 2000);
    if(port == NULL)
    {
        qDebug() << "Failed to open port!!";
        return PortNotOpened;
    }

    unsigned int noProgressCount  = 0;
    unsigned int dataWrittenSize  = 0;
    const unsigned char* dataRaw  = (const UCHAR*)data.constData();
    const unsigned int   dataSize = data.size();

    while ((dataWrittenSize < dataSize) && (noProgressCount < 2)) {
        unsigned int sizeCommunicated = 0;
        if (WritePort(port, dataRaw + dataWrittenSize, dataSize - dataWrittenSize, &sizeCommunicated) == 0) {
            ClosePort(port);
            return WriteDataFail;
        }

        if (sizeCommunicated == 0) {
            noProgressCount++;
        }
        else {
            dataWrittenSize += sizeCommunicated;
            noProgressCount = 0;
        }
    }

    ClosePort(port);
    return dataWrittenSize;
}
