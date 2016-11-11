#include "databasehelper.h"
#include "usersessionhelper.h"

using namespace DatabaseType;
using namespace ErrorHelperType;
using namespace UserSessionType;

void DatabaseHelper::Transaction()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        throw SplashException(db.lastError().text(),
                              _classTag, CriticalError);
    }
}

void DatabaseHelper::Commit()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.commit()) {
        throw SplashException(db.lastError().text(),
                              _classTag, CriticalError);
    }
}

void DatabaseHelper::Rollback()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.rollback()) {
        throw SplashException(db.lastError().text(),
                              _classTag, CriticalError);
    }
}

unsigned int DatabaseHelper::InsertClient(const Client &client)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Client(Name, PhoneNumber, Address)"
                  " VALUES(:Name, :PhoneNumber, :Address)");
    query.bindValue(":Name", client.Name);
    query.bindValue(":PhoneNumber", client.PhoneNumber);
    query.bindValue(":Address", client.Address);
    if (!query.exec()) {
        throw SplashException(query.lastError().text()
                              + "\n" + query.executedQuery(),
                              _classTag, CriticalError);
    }

    return query.lastInsertId().toUInt();
}

unsigned int DatabaseHelper::InsertReceipt(const Receipt &receipt,
                                   bool registerClient)
{
    if (receipt.Number <= 0){
        throw SplashException(QString("Invalid receipt number (%1)")
                              .arg(receipt.Number), _classTag);
    }

    int clientId = 0;
    if (registerClient)
        clientId = InsertClient(receipt.ClientData);
    else
        clientId = receipt.ClientData.Id;


    // Insert receipt basic data
    QSqlQuery receiptQuery;
    receiptQuery.prepare("INSERT INTO Receipt(ReceiptNumber, UserAccountId, ClientId,"
                         "                    RegistrationDate, DeliveryDate)"
                         "  VALUES(:ReceiptNumber, :UserAccountId, :ClientId,"
                         "         :RegistrationDate, :DeliveryDate)");
    receiptQuery.bindValue(":ReceiptNumber", receipt.Number);
    receiptQuery.bindValue(":UserAccountId", receipt.UserAccountData.Id);
    receiptQuery.bindValue(":ClientId", clientId);
    receiptQuery.bindValue(":RegistrationDate",
                           receipt.RegistrationDateTime.toString("yyyy-MM-dd hh:mm"));
    receiptQuery.bindValue(":DeliveryDate",
                           receipt.DeliveryDateTime.toString("yyyy-MM-dd hh:mm"));
    if (!receiptQuery.exec()) {
        throw SplashException(receiptQuery.lastError().text()
                              + "\n" + receiptQuery.executedQuery(),
                              _classTag, CriticalError);
    }

    // Insert receipt payment data
    QSqlQuery paymentQuery;
    paymentQuery.prepare("INSERT INTO ReceiptPayment(ReceiptNumber, PaymentType,"
                         "                           Discount, TotalToPayGross, "
                         "                           TotalToPayNet, TotalPayed)"
                         " VALUES(:ReceiptNumber, :PaymentType, :Discount,"
                         "        :TotalToPayGross, :TotalToPayNet, :TotalPayed)");
    paymentQuery.bindValue(":ReceiptNumber", receipt.Number);
    paymentQuery.bindValue(":Discount", 0);
    paymentQuery.bindValue(":TotalToPayGross", receipt.TotalToPayGross);
    paymentQuery.bindValue(":TotalToPayNet", receipt.TotalToPayNet);
    paymentQuery.bindValue(":TotalPayed", receipt.TotalPayed);

    switch (receipt.PaymentKind) {
        case CompletePayment:
            paymentQuery.bindValue(":PaymentType", "complete");
            break;

        case PartialPayment:
            paymentQuery.bindValue(":PaymentType", "partial");
            break;
    }

    if (!paymentQuery.exec()) {
        throw SplashException(paymentQuery.lastError().text()
                              + "\n" + paymentQuery.executedQuery(),
                              _classTag, CriticalError);
    }

    // Insert clothes
    QSqlQuery clothQuery;
    for (int i = 0; i < receipt.Clothes.size(); ++i) {
        const Cloth& cloth = receipt.Clothes[i];
        clothQuery.prepare("INSERT INTO Cloth(ReceiptNumber, ItemNumber, ServiceTypeId,"
                           "                  ClothTypeId, TreatmentTypeId, PriceCharged,"
                           "                  Urgency, Color, Brand, Size, Observations,"
                           "                  ChargedPerSqrMeters, SizeInSqrMeters)"
                           " VALUES(:ReceiptNumber, :ItemNumber, :ServiceTypeId,"
                           "        :ClothTypeId, :TreatmentTypeId, :PriceCharged,"
                           "        :Urgency, :Color, :Brand, :Size, :Observations,"
                           "        :ChargedPerSqrMeters, :SizeInSqrMeters)");
        clothQuery.bindValue(":ReceiptNumber", receipt.Number);
        clothQuery.bindValue(":ItemNumber", i + 1);
        clothQuery.bindValue(":ServiceTypeId", cloth.Service.Id);
        clothQuery.bindValue(":ClothTypeId", cloth.ClothKind.Id);
        clothQuery.bindValue(":TreatmentTypeId", cloth.TreatmentKind.Id);
        clothQuery.bindValue(":PriceCharged", cloth.PriceCharged);
        clothQuery.bindValue(":Urgency", (int)cloth.Urgency);
        clothQuery.bindValue(":Color", cloth.Color);
        clothQuery.bindValue(":Brand", cloth.Brand);
        clothQuery.bindValue(":Size", cloth.Size);
        clothQuery.bindValue(":Observations", cloth.Observations);
        clothQuery.bindValue(":ChargedPerSqrMeters", cloth.IsChargedPerSqrMeters);
        clothQuery.bindValue(":SizeInSqrMeters", cloth.SizeInSquareMeters);

        if (!clothQuery.exec()) {
            throw SplashException(clothQuery.lastError().text()
                                  + "\n" + clothQuery.executedQuery(),
                                  _classTag, CriticalError);
        }
    }

    return receipt.Number;
}
