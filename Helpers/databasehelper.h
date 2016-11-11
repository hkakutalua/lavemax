#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlQuery>
#include <QDateTime>
#include <QSqlError>
#include "../Exceptions/splashexception.h"

namespace DatabaseType
{
    enum UrgencyLevel
    {
        NormalLevel = 0,    // 0%
        MediumLevel = 50,   // 50%
        HighLevel   = 100   // 100%
    };

    enum DiscountLevel
    {
        NoDiscount = 0,
        Discount10 = 10,
        Discount20 = 20
    };

    enum PaymentType
    {
        CompletePayment,
        PartialPayment
    };

    enum PrivilegeLevel
    {
        Administrator,
        Manager,
        User
    };

    struct UserAccount
    {
        unsigned int   Id = 0;
        QString        Name;
        QString        Login;
        QString        Password;
        bool           IsActive = true;
        PrivilegeLevel Level;
    };

    struct Client
    {
        unsigned int Id = 0;
        QString Name;
        QString PhoneNumber;
        QString Address;
    };

    struct ServiceType
    {
        unsigned int Id = 0;
        QString      Name;
    };

    struct ClothType
    {
        unsigned int Id = 0;
        QString      Name;
    };

    struct TreatmentType
    {
        unsigned int Id = 0;
        QString      Name;
    };

    struct Cloth
    {
        unsigned int  ItemNumber = 0;

        ServiceType   Service;
        ClothType     ClothKind;
        TreatmentType TreatmentKind;

        UrgencyLevel  Urgency;
        double        PriceCharged;

        QString       Color;
        QString       Brand;
        QString       Size;
        QString       Observations;

        bool          IsChargedPerSqrMeters = false;
        double        SizeInSquareMeters = 0;
    };

    struct Receipt
    {
        unsigned int  Number = 0;
        UserAccount   UserAccountData;
        Client        ClientData;

        QDateTime     RegistrationDateTime;
        QDateTime     DeliveryDateTime;

        PaymentType   PaymentKind;
        DiscountLevel Discount {NoDiscount};
        double        TotalToPayGross;
        double        TotalToPayNet;
        double        TotalPayed;

        QList<Cloth>  Clothes;
    };
}

class DatabaseHelper
{
public:
    static DatabaseHelper& GetInstance()
    {
        static DatabaseHelper instance;
        return instance;
    }

    void Transaction();
    void Commit();
    void Rollback();

    unsigned int InsertClient(const DatabaseType::Client &client);
    unsigned int InsertReceipt(const DatabaseType::Receipt &receipt,
                       bool registerClient);

private:
    DatabaseHelper() { _classTag = "DatabaseHelper"; }
    DatabaseHelper(const DatabaseHelper&);
    const DatabaseHelper& operator=(const DatabaseHelper&);

    QString _classTag;
};

#endif // DATABASEHELPER_H
