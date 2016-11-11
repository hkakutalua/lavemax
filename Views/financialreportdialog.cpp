#include "financialreportdialog.h"
#include "ui_financialreportdialog.h"

#include <QSettings>

using namespace DatabaseType;
using namespace FinancialReportType;

FinancialReportDialog::FinancialReportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinancialReportDialog)
{
    ui->setupUi(this);

    QSettings settings;
    _morningShiftBegin   = settings.value("shifts/mornBeginShift").toTime();
    _morningShiftEnd     = settings.value("shifts/mornEndShift").toTime();

    _afternoonShiftBegin = settings.value("shifts/aftBeginShift").toTime();
    _afternoonShiftEnd   = settings.value("shifts/aftEndShift").toTime();

    _nightShiftBegin     = settings.value("shifts/nightBeginShift").toTime();
    _nightShiftEnd       = settings.value("shifts/nightEndShift").toTime();

    _shiftBegin          = _morningShiftBegin;
    _shiftEnd            = _morningShiftEnd;

    setUpConnections();
}

FinancialReportDialog::~FinancialReportDialog()
{
    delete ui;
}

void FinancialReportDialog::setUpConnections()
{
    connect(ui->PrintAndExitButton, SIGNAL(clicked()),
            this, SLOT(printReport()));
    connect(ui->CancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    ui->ShiftButtonGroup->setId(ui->MorningShiftRadio,   Morning);
    ui->ShiftButtonGroup->setId(ui->AfternoonShiftRadio, Afternoon);
    connect(ui->ShiftButtonGroup, SIGNAL(buttonToggled(int,bool)),
            this, SLOT(updateShiftData()));
}

void FinancialReportDialog::updateShiftData()
{
    _shift = (Shift)ui->ShiftButtonGroup->checkedId();
    switch (_shift) {
        case Morning:
            _shiftBegin = _morningShiftBegin;
            _shiftEnd   = _morningShiftEnd;
            break;

        case Afternoon:
            _shiftBegin = _afternoonShiftBegin;
            _shiftEnd   = _afternoonShiftEnd;
            break;

        case Night:
            _shiftBegin = _nightShiftBegin;
            _shiftEnd   = _nightShiftEnd;
            break;
    }
}

void FinancialReportDialog::printReport()
{
    QDateTime beginDate = QDateTime(QDate::currentDate(), _shiftBegin);
    QDateTime endDate   = QDateTime(QDate::currentDate(), _shiftEnd);

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

            receipt.Clothes         = queryReceiptClothes(receipt.Number),
            receipt.ClientData      = queryReceiptClient(clientId);
            receipt.UserAccountData = queryReceiptUser(userId);

            receiptList.append(receipt);
        }

        //
        ReportPrintHelper::GetInstance()
                          .PrintReport(receiptList);
        done(0);
    }
    catch (const SplashException &e) {
        ErrorHelper::GetInstance()
                    .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

QSqlQuery FinancialReportDialog::queryReceiptsBetween(QDateTime beginDate, QDateTime endDate)
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT ReceiptNumber, UserAccountId, ClientId,"
        "       RegistrationDate, DeliveryDate"
        " FROM Receipt"
        " WHERE RegistrationDate BETWEEN :BeginDate AND :EndDate"
        " ORDER BY ReceiptNumber";
    query.prepare(queryTxt);
    query.bindValue(":BeginDate", beginDate.toString("dd-MM-yyyy hh:mm"));
    query.bindValue(":EndDate", endDate.toString("dd-MM-yyyy hh:mm"));

    if (!query.exec()) {
        throw SplashException(query.lastError().text()
                              + "\n" + queryTxt,
                              _classTag);
    }

    qDebug() << beginDate.toString() << endDate.toString();
    qDebug() << query.size();

    return query;
}

QSqlQuery FinancialReportDialog::queryPaymentsBetween(QDateTime beginDate, QDateTime endDate)
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT PaymentType, Discount, TotalToPayGross,"
        "       TotalToPayNet, TotalPayed"
        " FROM ReceiptPayment AS Payment"
        " JOIN Receipt"
        " ON   Receipt.ReceiptNumber = Payment.ReceiptNumber"
        " WHERE Receipt.RegistrationDate BETWEEN :BeginDate AND :EndDate"
        " ORDER BY Payment.ReceiptNumber";
    query.prepare(queryTxt);
    query.bindValue(":BeginDate", beginDate.toString("dd-MM-yyyy hh:mm"));
    query.bindValue(":EndDate", endDate.toString("dd-MM-yyyy hh:mm"));

    if (!query.exec()) {
        throw SplashException(query.lastError().text()
                              + "\n" + queryTxt,
                              _classTag);
    }

    qDebug() << query.size();

    return query;
}

QList<Cloth> FinancialReportDialog::queryReceiptClothes(uint receiptNumber)
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT c.ItemNumber, st.Name, ct.Name, tt.Name, c.PriceCharged, c.Urgency,"
        "       c.Size, c.SizeInSqrMeters, c.Brand, c.Color, c.Observations"
        " FROM Cloth AS c"
        " INNER JOIN ServiceType AS st"
        "  ON st.ServiceTypeId = c.ServiceTypeId"
        " INNER JOIN ClothType AS ct"
        "  ON ct.ClothTypeId = c.ClothTypeId"
        " INNER JOIN TreatmentType AS tt"
        "  ON tt.TreatmentTypeId = c.TreatmentTypeId"
        " WHERE ReceiptNumber = :ReceiptNumber";

    query.prepare(queryTxt);
    query.bindValue(":ReceiptNumber", receiptNumber);
    if (!query.exec()) {
        throw SplashException(query.lastError().text()
                              + "\n" + queryTxt,
                              _classTag);
    }

    QList<Cloth> clothes;
    while (query.next()) {
        Cloth cloth;
        cloth.ItemNumber         = query.value(0).toUInt();
        cloth.Service.Name       = query.value(1).toString();
        cloth.ClothKind.Name     = query.value(2).toString();
        cloth.TreatmentKind.Name = query.value(3).toString();
        cloth.PriceCharged       = query.value(4).toDouble();
        cloth.Urgency            = (UrgencyLevel)query.value(5).toInt();
        cloth.Size               = query.value(6).toString();
        cloth.SizeInSquareMeters = query.value(7).toDouble();
        cloth.Brand              = query.value(8).toString();
        cloth.Color              = query.value(9).toString();
        cloth.Observations       = query.value(10).toString();

        clothes.append(cloth);
    }

    return clothes;
}

Client FinancialReportDialog::queryReceiptClient(uint clientId)
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

UserAccount FinancialReportDialog::queryReceiptUser(uint userId)
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
