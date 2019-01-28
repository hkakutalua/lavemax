#include "reportprinthelper.h"
#include "usersessionhelper.h"

using namespace DatabaseType;
using namespace StarTSPPrinterType;

void ReportPrintHelper::PrintReport(const QList<Receipt> &receipts)
{
    QString data = prepareReceiptData(receipts);
    StarTSPPrinterHelper::GetInstance()
                         .Print(data);
}

QString ReportPrintHelper::prepareReceiptData(const QList<Receipt> &receipts)
{
    const QString companyName = "LAVANDARIA ART LAR";

    const QString separator = "\n"
        + SetEmphPrinting +
        "------------------------------------------------"
        + CancelEmphPrinting + "\n";

    const QString header = QString(
        SetEmphPrinting + CenterAlignment + "\n" + companyName +
        "\nFECHO DE CAIXA\n\n" +
        LeftAlignment +
        "DATA E HORA: %L1\n"
        "FUNCIONÁRIO(A): %2\n\n" +
        CancelEmphPrinting
        ).arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm"))
         .arg(UserSessionHelper::GetInstance().GetLoggedUser().Name);

    QString tempStr = header;
    for (int i=0; i<receipts.size(); ++i) {
        QString paymentTypeStr;
        paymentTypeStr = (receipts[i].PaymentKind == CompletePayment)
                         ? "Pagamento Completo" : "Pagamento Parcial";

        tempStr += QString(
            LeftAlignment + separator +
            SetEmphPrinting + "Fatura nº %L1\n" + CancelEmphPrinting +
            "Data de Entrega: %L2\n"
            "Cliente: %3\n" +
            SetEmphPrinting + "%5\n" + CancelEmphPrinting +
            "A Pagar: %6 Kz(s)\n"
            "Pagou: %7 Kz(s)\n"
            "Valores em Falta: %8"
        ).arg(receipts[i].Number)
         .arg(receipts[i].DeliveryDateTime.toString("dd-MM-yyyy hh:mm"))
         .arg(receipts[i].ClientData.Name)
         .arg(paymentTypeStr)
         .arg(receipts[i].TotalToPayNet)
         .arg(receipts[i].TotalPayed)
         .arg(receipts[i].TotalToPayNet - receipts[i].TotalPayed);

    }
    tempStr += separator;

    double totalToPay = 0;
    double totalPayed = 0;
    double missingPayment = 0;

    for (Receipt receipt : receipts) {
        totalToPay += receipt.TotalToPayNet;
        totalPayed += receipt.TotalPayed;
    }
    missingPayment = totalToPay - totalPayed;

    tempStr += QString(
        SetEmphPrinting + RightAlignment +
        "\nTotal a Pagar: %L1 Kz(s)\n" +
        "Total Pago: %L2 Kz(s)\n" +
        "Total em Falta: %L3 Kz(s)\n" +
        CancelEmphPrinting
        ).arg(totalToPay)
         .arg(totalPayed)
         .arg(missingPayment);

    // In the end...
    tempStr += CutCommand + OpenCashDrawer;
    return tempStr;
}
