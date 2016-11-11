#include "receiptprinterhelper.h"

using namespace DatabaseType;
using namespace StarTSPPrinterType;

void ReceiptPrinterHelper::PrintReceipt(const Receipt &receipt, bool printDuplicate)
{
    StarTSPPrinterHelper &printer = StarTSPPrinterHelper::GetInstance();
    printer.Print(prepareReceiptData(receipt, printDuplicate));
}

QString ReceiptPrinterHelper::prepareReceiptData(const Receipt &receipt, bool printDuplicate)
{
    //
    const QString separator = "\n" + SetEmphPrinting +
        "------------------------------------------------"
        + CancelEmphPrinting + "\n";

    QString paymentTypeStr;
    paymentTypeStr = (receipt.PaymentKind == CompletePayment)
                     ? "Pagamento Completo" : "Pagamento Parcial";

    QString tempStr = QString(
        SetEmphPrinting +
        CenterAlignment +
        "\nLAVANDARIA LAVEMAX\n\n" +
        LeftAlignment +
        "Fatura nº %L1\n"
        "Data de Registo: %L2\n"
        "Data de Entrega: %L3\n"
        "Cliente: %4\n"
        "Funcionario(a): %5\n"
        "%6\n" +
        CancelEmphPrinting
    ).arg(receipt.Number)
     .arg(receipt.RegistrationDateTime.toString("dd-MM-yyyy hh:mm"))
     .arg(receipt.DeliveryDateTime.toString("dd-MM-yyyy hh:mm"))
     .arg(receipt.ClientData.Name, receipt.UserAccountData.Name)
     .arg(paymentTypeStr);

    // ---------------------------------------------- //
    //  Peça: Peça XXXX                               //
    // ---------------------------------------------- //
    for (Cloth cloth : receipt.Clothes){
        tempStr += QString(
            LeftAlignment +
            separator +
            "Peça: %1\n"
            "Tratamento: %2\n"
            "Preço: %L3 Kz(s)\n"
            "Marca: %4\n"
            "Detalhes: %5; %6;"
        ).arg(cloth.ClothKind.Name, cloth.TreatmentKind.Name)
         .arg(cloth.PriceCharged)
         .arg(cloth.Brand, cloth.Color, cloth.Observations);
    }
    tempStr += separator + "\n";

    double totalPayed = receipt.TotalToPayNet;
    double missingPayment = 0;
    if (receipt.PaymentKind == PartialPayment) {
        totalPayed = receipt.TotalPayed;
        missingPayment
            = receipt.TotalToPayNet - receipt.TotalPayed;
    }

    tempStr += QString(
        SetEmphPrinting +
        RightAlignment +
        "Total a Pagar: %L1 Kz(s)\n" +
        "Total Pago: %L2 Kz(s)\n" +
        "Valores em Falta: %L3 Kz(s)\n" +
        CancelEmphPrinting
        ).arg(receipt.TotalToPayNet)
         .arg(totalPayed)
         .arg(missingPayment);

    // In the end...
    tempStr += CutCommand;

    // The receipt will be printed twice in the same paper
    if (printDuplicate)
        tempStr += tempStr + OpenCashDrawer;
    else
        tempStr += OpenCashDrawer;

    return tempStr;
}
