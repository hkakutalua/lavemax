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
    const QString companyName = "LAVANDARIA ART LAR";

    //
    const QString separator = "\n" + SetEmphPrinting +
        "------------------------------------------------"
        + CancelEmphPrinting + "\n";

    QString tempStr =
        SetEmphPrinting +
        CenterAlignment + "\n" +
        companyName + "\n\n" +
        CancelEmphPrinting;

    if (receipt.PaymentKind == PartialPayment) {
        tempStr += LeftAlignment + SetEmphPrinting + SetExpansion3x+
            "PP\n" +
            CancelExpansion + CancelEmphPrinting;
    }

    tempStr += QString(
        SetEmphPrinting +
        LeftAlignment +
        "FATURA nº %L1\n"
        "DATA DE REGISTO: %L2\n"
        "DATA DE ENTREGA: %L3\n"
        "CLIENTE: %4\n"
        "TELEMÓVEL: %5\n"
        "FUNCIONÁRIO(a): %6\n" +
        CancelEmphPrinting
    ).arg(receipt.Number)
     .arg(receipt.RegistrationDateTime.toString("dd-MM-yyyy hh:mm"))
     .arg(receipt.DeliveryDateTime.toString("dd-MM-yyyy hh:mm"))
     .arg(receipt.ClientData.Name, receipt.ClientData.PhoneNumber)
     .arg(receipt.UserAccountData.Name);

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
            "Detalhes: %5; %6;\n"
            "Urgência: %7%"
        ).arg(cloth.ClothKind.Name, cloth.TreatmentKind.Name)
         .arg(cloth.PriceCharged)
         .arg(cloth.Brand, cloth.Color, cloth.Observations)
         .arg(cloth.Urgency);
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
        "Valores em Falta: %L3 Kz(s)" +
        separator +
        CancelEmphPrinting
        ).arg(receipt.TotalToPayNet)
         .arg(totalPayed)
         .arg(missingPayment);

    // Print company info
    /*
    const QString address = "Cacuaco - Bairro das Salinas"
                            " - Rua direita da Vila de Cacuaco";
    const QString email   = "lave.max2015@gmail.com";
    const QString nif     = "5417381349";
    const uint    phone1  = 924477500;
    const uint    phone2  = 994477500;
    tempStr += QString(
        CenterAlignment +
        "%1\n"
        "Terminais: %L2 \ %L3\n"
        "Email: %4 - NIF: %5\n\n"
        + SetEmphPrinting +
        "TEMOS REGRAS DA CASA\n"
        + CancelEmphPrinting
        ).arg(address)
         .arg(phone1)
         .arg(phone2)
         .arg(email, nif);
    */

    // In the end...
    tempStr += CutCommand;

    // The receipt will be printed twice in the same paper
    if (printDuplicate)
        tempStr += tempStr + OpenCashDrawer;
    else
        tempStr += OpenCashDrawer;

    return tempStr;
}
