#include "usersessionhelper.h"
#include "reportprinthelper.h"

using namespace UserSessionType;
using namespace DatabaseType;
using namespace ErrorHelperType;

bool UserSessionHelper::LoginUser(const QString &login, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT UserAccountId, Name, Login,"
                  "       Password, IsActive, PrivilegeLevel"
                  " FROM UserAccount"
                  " WHERE Login = :Login AND Password = :Password");
    query.bindValue(":Login", login);
    query.bindValue(":Password", password);

    if (!query.exec()) {
        throw SplashException(query.lastError().text(),
                              _classTag, ErrorHelperType::CriticalError);
    }

    if (!query.next())
        return false;

    _userAccount.Id       = query.value(0).toUInt();
    _userAccount.Name     = query.value(1).toString();
    _userAccount.Login    = query.value(2).toString();
    _userAccount.Password = query.value(3).toString();
    _userAccount.IsActive = query.value(4).toBool();

    const QString privilegeLevelTxt = query.value(5).toUInt();
    if (privilegeLevelTxt == "admin")
        _userAccount.Level = Administrator;
    else if (privilegeLevelTxt == "manager")
        _userAccount.Level = Manager;
    else if(privilegeLevelTxt == "user")
        _userAccount.Level = User;

    try {
        insertLoginRegistry();
    }
    catch (const SplashException &e) {
        ErrorHelper::GetInstance()
            .SaveAndShowError(e.Tag(), e.Level(), e.What());
        LogoffUser();
        return false;
    }

    _loginState = UserLogged;
    return true;
}

void UserSessionHelper::LogoffUser()
{
    if (_loginState == UserLogged) {
        _userAccount = UserAccount();
        _loginState  = UserNotLogged;
    }
}

UserAccount UserSessionHelper::GetLoggedUser() const
{
    return _userAccount;
}

LoginState UserSessionHelper::GetLoginState() const
{
    return _loginState;
}

void UserSessionHelper::PrintUserFinancialReport() const
{
    if (_loginState == UserNotLogged)
        return;

    QDateTime firstLoginDate = queryTodayFirstLoginDate();
    if (firstLoginDate.isNull()) return;
    QDateTime beginDate = firstLoginDate;
    QDateTime endDate   = QDateTime::currentDateTime();

    try {
        QList<Receipt> receiptList;
        QSqlQuery receiptQuery = queryReceiptsBetween(beginDate, endDate);
        QSqlQuery paymQuery    = queryPaymentsBetween(beginDate, endDate);
        uint      clientId     = 0;
        uint      userId       = 0;

        while (receiptQuery.next()) {
            Receipt receipt;

            receipt.Number               = receiptQuery.value(0).toUInt();
            userId                       = receiptQuery.value(1).toUInt();
            clientId                     = receiptQuery.value(2).toUInt();
            receipt.RegistrationDateTime = receiptQuery.value(3).toDateTime();
            receipt.DeliveryDateTime     = receiptQuery.value(4).toDateTime();

            paymQuery.next();
            QString paymStr = paymQuery.value(0).toString();
            if (paymStr == "complete")
                receipt.PaymentKind = CompletePayment;
            else
                receipt.PaymentKind = PartialPayment;

            receipt.Discount        = (DiscountLevel)paymQuery.value(1).toInt();
            receipt.TotalToPayGross = paymQuery.value(2).toDouble();
            receipt.TotalToPayNet   = paymQuery.value(3).toDouble();
            receipt.TotalPayed      = paymQuery.value(4).toDouble();
            receipt.ClientData      = queryReceiptClient(clientId);
            receipt.UserAccountData = queryReceiptUser(userId);

            receiptList.append(receipt);
        }

        //
        ReportPrintHelper::GetInstance()
                          .PrintReport(receiptList);
    }
    catch (const SplashException &e) {
        ErrorHelper::GetInstance()
                    .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

void UserSessionHelper::insertLoginRegistry() const
{
    if (_userAccount.Id <= 0)
        return;

    QSqlQuery query;
    query.prepare("INSERT INTO LoginRegistry(UserAccountId, LoginDate)"
                  " VALUES(:UserAccountId, :LoginDate)");
    query.bindValue(":UserAccountId", _userAccount.Id);
    query.bindValue(":LoginDate", QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss"));
    if (!query.exec()) {
        throw SplashException(query.lastError().text(),
                              _classTag, ErrorHelperType::CriticalError);
    }
}

QDateTime UserSessionHelper::queryTodayFirstLoginDate() const
{
    QSqlQuery query;
    query.prepare("SELECT LoginDate FROM LoginRegistry"
                  " WHERE UserAccountId = :UserAccountId AND"
                  " (LoginDate >= :BeginDate AND LoginDate <= :EndDate)");
    query.bindValue(":UserAccountId", _userAccount.Id);
    query.bindValue(":BeginDate", QDateTime(QDate::currentDate(), QTime(0, 0)));
    query.bindValue(":EndDate", QDateTime(QDate::currentDate(), QTime(23, 59)));

    if (!query.exec()) {
        throw SplashException(query.lastError().text()
                              + "\n" + query.lastQuery(),
                              _classTag);
    }

    QDateTime firstLoginDate;
    if (query.next()) {
        firstLoginDate = query.value(0).toDateTime();
    }

    return firstLoginDate;
}

Client UserSessionHelper::queryReceiptClient(uint clientId) const
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT Name, PhoneNumber, Address FROM Client"
        " WHERE ClientId = :ClientId";

    query.prepare(queryTxt);
    query.bindValue(":ClientId", clientId);
    if (!query.exec()){
        throw SplashException(query.lastError().text()
                              + "\n" + queryTxt,
                              _classTag);
    }

    Client client;
    if (query.next()) {
        client.Id          = clientId;
        client.Name        = query.value(0).toString();
        client.PhoneNumber = query.value(1).toString();
        client.Address     = query.value(2).toString();
    }

    return client;
}

UserAccount UserSessionHelper::queryReceiptUser(uint userId) const
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT Name, Login"
        " FROM UserAccount"
        " WHERE UserAccountId = :UserAccountId";

    query.prepare(queryTxt);
    query.bindValue(":UserAccountId", userId);
    if (!query.exec()){
        throw SplashException(query.lastError().text()
                              + "\n" + queryTxt,
                              _classTag);
    }

    UserAccount user;
    if (query.next()) {
        user.Id    = userId;
        user.Name  = query.value(0).toString();
        user.Login = query.value(1).toString();
    }

    return user;
}

QSqlQuery UserSessionHelper::queryReceiptsBetween(QDateTime beginDate, QDateTime endDate) const
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT ReceiptNumber, UserAccountId, ClientId,"
        "       RegistrationDate, DeliveryDate"
        " FROM Receipt"
        " WHERE RegistrationDate BETWEEN :BeginDate AND :EndDate"
        "  AND UserAccountId = :UserAccountId"
        " ORDER BY ReceiptNumber";
    query.prepare(queryTxt);
    query.bindValue(":BeginDate", beginDate.toString("dd-MM-yyyy hh:mm"));
    query.bindValue(":EndDate", endDate.toString("dd-MM-yyyy hh:mm"));
    query.bindValue(":UserAccountId", _userAccount.Id);

    if (!query.exec()) {
        throw SplashException(query.lastError().text()
                              + "\n" + queryTxt,
                              _classTag);
    }

    return query;
}

QSqlQuery UserSessionHelper::queryPaymentsBetween(QDateTime beginDate, QDateTime endDate) const
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT PaymentType, Discount, TotalToPayGross,"
        "       TotalToPayNet, TotalPayed"
        " FROM ReceiptPayment AS Payment"
        " JOIN Receipt"
        " ON   Receipt.ReceiptNumber = Payment.ReceiptNumber"
        " WHERE Receipt.RegistrationDate BETWEEN :BeginDate AND :EndDate"
        "  AND UserAccountId = :UserAccountId"
        " ORDER BY Payment.ReceiptNumber";
    query.prepare(queryTxt);
    query.bindValue(":BeginDate", beginDate.toString("dd-MM-yyyy hh:mm"));
    query.bindValue(":EndDate", endDate.toString("dd-MM-yyyy hh:mm"));
    query.bindValue(":UserAccountId", _userAccount.Id);

    if (!query.exec()) {
        throw SplashException(query.lastError().text()
                              + "\n" + queryTxt,
                              _classTag);
    }

    return query;
}
