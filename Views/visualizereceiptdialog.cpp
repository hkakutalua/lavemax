#include "visualizereceiptdialog.h"
#include "ui_visualizereceiptdialog.h"

using namespace DatabaseType;
using namespace ErrorHelperType;

VisualizeReceiptDialog::VisualizeReceiptDialog(unsigned int receiptNumber,
                                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VisualizeReceiptDialog)
{
    ui->setupUi(this);

    _tag            = metaObject()->className();
    _receiptNumber  = receiptNumber;

    setUpConnections();

    updateReceiptData();
    updateReceiptPaymData();
    updateClothData();
    updateClientData();
    updateUserData();
}

VisualizeReceiptDialog::~VisualizeReceiptDialog()
{
    delete ui;
}

void VisualizeReceiptDialog::setUpConnections()
{
    connect(ui->ReprintButton, SIGNAL(clicked()),
            this, SLOT(reprintReceipt()));
    connect(ui->ExitButton, SIGNAL(clicked()),
            this, SLOT(accept()));
}

void VisualizeReceiptDialog::updateReceiptData()
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT ReceiptNumber, UserAccountId, ClientId,"
        "       RegistrationDate, DeliveryDate"
        " FROM Receipt"
        " WHERE ReceiptNumber = :ReceiptNumber";

    try {
        query.prepare(queryTxt);
        query.bindValue(":ReceiptNumber", _receiptNumber);

        if (!query.exec()) {
            throw SplashException(query.lastError().text()
                                  + "\n" + queryTxt,
                                  _tag);
        }

        if (query.next()){
            _receipt.Number               = query.value(0).toUInt();
            _userId                       = query.value(1).toUInt();
            _clientId                     = query.value(2).toUInt();
            _receipt.RegistrationDateTime = query.value(3).toDateTime();
            _receipt.DeliveryDateTime     = query.value(4).toDateTime();

            updateReceiptUi();
        }
    }
    catch(SplashException &e) {
        ErrorHelper::GetInstance()
         .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

void VisualizeReceiptDialog::updateReceiptPaymData()
{
    QSqlQuery query;
    QString queryTxt =
        "SELECT PaymentType, Discount, TotalToPayGross,"
        "       TotalToPayNet, TotalPayed"
        " FROM ReceiptPayment"
        " WHERE ReceiptNumber = :ReceiptNumber";

    try {
        query.prepare(queryTxt);
        query.bindValue(":ReceiptNumber", _receipt.Number);

        if (!query.exec()) {
            throw SplashException(query.lastError().text()
                                  + "\n" + queryTxt,
                                  _tag);
        }

        if (query.next()) {
            QString paymentType = query.value(0).toString();
            if (paymentType == "complete")
                _receipt.PaymentKind = CompletePayment;
            else if (paymentType == "partial")
                _receipt.PaymentKind = PartialPayment;

            _receipt.Discount        = (DiscountLevel)query.value(1).toInt();
            _receipt.TotalToPayGross = query.value(2).toDouble();
            _receipt.TotalToPayNet   = query.value(3).toDouble();
            _receipt.TotalPayed      = query.value(4).toDouble();

            updateReceiptPaymUi();
        }
    }
    catch (SplashException &e) {
        ErrorHelper::GetInstance()
         .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

void VisualizeReceiptDialog::updateClothData()
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

    try {
        query.prepare(queryTxt);
        query.bindValue(":ReceiptNumber", _receipt.Number);
        if (!query.exec()) {
            throw SplashException(query.lastError().text()
                                  + "\n" + queryTxt,
                                  _tag);
        }

        while (query.next()) {
            Cloth cloth;
            cloth.ItemNumber           = query.value(0).toUInt();
            cloth.Service.Name         = query.value(1).toString();
            cloth.ClothKind.Name       = query.value(2).toString();
            cloth.TreatmentKind.Name   = query.value(3).toString();
            cloth.PriceCharged         = query.value(4).toDouble();
            cloth.Urgency              = (UrgencyLevel)query.value(5).toInt();
            cloth.Size                 = query.value(6).toString();
            cloth.SizeInSquareMeters   = query.value(7).toDouble();
            cloth.Brand                = query.value(8).toString();
            cloth.Color                = query.value(9).toString();
            cloth.Observations         = query.value(10).toString();
            _receipt.Clothes          << cloth;
        }

        QSqlQueryModel *model = new QSqlQueryModel;
        model->setQuery(query);
        updateClothUi(model);
    }
    catch (SplashException &e) {
        ErrorHelper::GetInstance()
         .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

void VisualizeReceiptDialog::updateClientData()
{
    QString queryTxt =
        "SELECT Name, PhoneNumber, Address FROM Client"
        " WHERE ClientId = :ClientId";
    QSqlQuery query;

    try {
        query.prepare(queryTxt);
        query.bindValue(":ClientId", _clientId);
        if (!query.exec()){
            throw SplashException(query.lastError().text()
                                  + "\n" + queryTxt,
                                  _tag);
        }

        if (query.next()) {
            Client client;
            client.Id           = _clientId;
            client.Name         = query.value(0).toString();
            client.PhoneNumber  = query.value(1).toString();
            client.Address      = query.value(2).toString();
            _receipt.ClientData = client;

            updateClientUi();
        }
    }
    catch (SplashException &e) {
        ErrorHelper::GetInstance()
         .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

void VisualizeReceiptDialog::updateUserData()
{
    QString queryTxt =
        "SELECT Name FROM UserAccount"
        " WHERE UserAccountId = :UserAccountId";
    QSqlQuery query;

    try {
        query.prepare(queryTxt);
        query.bindValue(":UserAccountId", _userId);
        if (!query.exec()){
            throw SplashException(query.lastError().text()
                                  + "\n" + queryTxt,
                                  _tag);
        }

        if (query.next()) {
            UserAccount user;
            user.Id                  = _userId;
            user.Name                = query.value(0).toString();
            _receipt.UserAccountData = user;

            updateUserUi();
        }
    }
    catch (SplashException &e) {
        ErrorHelper::GetInstance()
         .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

void VisualizeReceiptDialog::updateReceiptUi()
{
    ui->ReceiptNumberLabel->setText(QString("Fatura Número %L1")
                                    .arg(_receipt.Number));
    ui->RegistrationDateLabel->setText(
                _receipt.RegistrationDateTime.toString("dd-MM-yyyy hh:mm"));
    ui->DeliveryDateLabel->setText(
                _receipt.DeliveryDateTime.toString("dd-MM-yyyy hh:mm"));
}

void VisualizeReceiptDialog::updateReceiptPaymUi()
{
    QString paymTypeTxt;
    double  totalToPay       = _receipt.TotalToPayNet;
    double  totalPayed       = _receipt.TotalPayed;
    double  missingPaymValue = totalToPay - totalPayed;

    switch (_receipt.PaymentKind) {
        case CompletePayment:
            paymTypeTxt = "Pagamento Completo";
            ui->CompletePaymButton->setEnabled(false);
            break;

        case PartialPayment:
            paymTypeTxt = "Pagamento Parcial";
            ui->CompletePaymButton->setEnabled(true);
            break;
    }

    ui->PaymentTypeLabel->setText(paymTypeTxt);
    ui->TotalToPayLabel->setText(QString("%L1 Kz(s)").arg(totalToPay));
    ui->TotalPayedLabel->setText(QString("%L1 Kz(s)").arg(totalPayed));
    ui->MissingPaymLabel->setText(QString("%L1 Kz(s)").arg(missingPaymValue));
}

void VisualizeReceiptDialog::updateClothUi(QAbstractItemModel *model)
{
    int itemNumberCol      = 0;
    int serviceCol         = 1;
    int clothCol           = 2;
    int treatmentCol       = 3;
    int priceCol           = 4;
    int urgencyCol         = 5;
    int sizeCol            = 6;
    int sizeInSqrMetersCol = 7;
    int brandCol           = 8;
    int colorCol           = 9;
    int observationsCol    = 10;

    model->setHeaderData(serviceCol, Qt::Horizontal, "Serviço");
    model->setHeaderData(clothCol, Qt::Horizontal, "Tecido");
    model->setHeaderData(treatmentCol, Qt::Horizontal, "Tratamento");
    model->setHeaderData(priceCol, Qt::Horizontal, "Preço Cobrado");
    model->setHeaderData(urgencyCol, Qt::Horizontal, "Urgência");
    model->setHeaderData(sizeCol, Qt::Horizontal, "Tamanho");
    model->setHeaderData(sizeInSqrMetersCol, Qt::Horizontal, "Metro(s)²");
    model->setHeaderData(brandCol, Qt::Horizontal, "Marca");
    model->setHeaderData(colorCol, Qt::Horizontal, "Cor");
    model->setHeaderData(observationsCol, Qt::Horizontal, "Observações");

    QTableView *table = ui->ClothTableView;
    table->setModel(model);
    model->setParent(table);
    table->setColumnHidden(itemNumberCol, true);
    table->horizontalHeader()
         ->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void VisualizeReceiptDialog::updateClientUi()
{
    ui->ClientNameLabel->setText(_receipt.ClientData.Name);
    ui->ClientPhoneLabel->setText(_receipt.ClientData.PhoneNumber);
    ui->ClientAddressLabel->setText(_receipt.ClientData.Address);
}

void VisualizeReceiptDialog::updateUserUi()
{
    ui->UserNameLabel->setText(_receipt.UserAccountData.Name);
}

void VisualizeReceiptDialog::reprintReceipt()
{
    ReceiptPrinterHelper::GetInstance()
     .PrintReceipt(_receipt);
}
