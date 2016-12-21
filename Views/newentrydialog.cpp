#include "newentrydialog.h"
#include "ui_newentrydialog.h"

using namespace DatabaseType;

NewEntryDialog::NewEntryDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::NewEntryDialog)
{
    ui->setupUi(this);

    _classTag            = this->metaObject()->className();
    _receiptNumber       = getNextReceiptNumber();
    configureDefaultDeliveryDate();

    ui->PartPaymAmountSpinBox->setRange(_minPartPaymValue, _maxPartPaymValue);
    updateAllUi();

    setUpConnections();
}

NewEntryDialog::~NewEntryDialog()
{
    delete ui;
}

void NewEntryDialog::setUpConnections()
{
    connect(ui->SaveButton, SIGNAL(clicked()),
            this, SLOT(saveAndPrintReceipt()));
    connect(ui->CancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    connect(ui->ChangeClientButton, SIGNAL(clicked()),
            this, SLOT(openSelectClientDialog()));
    connect(ui->ChangeTissuesButton, SIGNAL(clicked()),
            this, SLOT(openEditTissuesDialog()));
    connect(ui->ChangeDeliveryDateButton, SIGNAL(clicked()),
            this, SLOT(openSelectDateTimeDialog()));

    connect(ui->PaymentTypeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updatePaymentTypeData()));
    connect(ui->PartPaymAmountSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(updatePaymentData()));
}

void NewEntryDialog::openSelectClientDialog()
{
    SelectClientDialog dialog = SelectClientDialog(this);
    connect(&dialog, SIGNAL(ClientSelectionDone(DatabaseType::Client,bool)),
            this, SLOT(updateClientData(DatabaseType::Client,bool)));
    dialog.exec();
}

void NewEntryDialog::openEditTissuesDialog()
{
    EditTissuesDialog *dialog = new EditTissuesDialog(_clothList, this);
    connect(dialog, SIGNAL(TissuesListUpdated(QList<DatabaseType::Cloth>)),
            this, SLOT(updateClothesData(QList<DatabaseType::Cloth>)));
    dialog->exec();
}

void NewEntryDialog::openSelectDateTimeDialog()
{
    SelectDateTimeDialog dialog = SelectDateTimeDialog(this);
    connect(&dialog, SIGNAL(DateTimeSelectionDone(QDateTime,QString)),
            this, SLOT(updateDateTimeData(QDateTime,QString)));
    dialog.exec();
}

void NewEntryDialog::updateClientData(Client client, bool registerClient)
{
    _client         = client;
    _registerClient = registerClient;

    updateClientUi();
}

void NewEntryDialog::updateClothesData(QList<Cloth> clothes)
{
    _clothList       = clothes;
    _clothesPriceSum = 0;
    for (auto cloth : _clothList)
        _clothesPriceSum += cloth.PriceCharged;

    configureDefaultDeliveryDate();
    updateClothesUi();
    updatePaymentData();
}

void NewEntryDialog::configureDefaultDeliveryDate()
{
    UrgencyLevel deliveryUrgency = NormalLevel;
    for (Cloth cloth : _clothList) {
        if (cloth.Urgency == MediumLevel) {
            deliveryUrgency = MediumLevel;
        }
        else if (cloth.Urgency == HighLevel) {
            deliveryUrgency = HighLevel;
            break;
        }
    }

    switch (deliveryUrgency) {
        case NormalLevel:
            _deliveryDateTime = QDateTime(QDate::currentDate(), QTime(17,0)).addDays(2);
            break;

        case MediumLevel:
            _deliveryDateTime = QDateTime(QDate::currentDate(), QTime(17,0)).addDays(1);
            break;

        case HighLevel:
            _deliveryDateTime = QDateTime(QDate::currentDate(), QTime(17,0));
            break;
    }

    if (_deliveryDateTime.date().dayOfWeek() == Qt::Sunday)
        _deliveryDateTime.addDays(1);

    _deliveryDateTimeStr = DateTimeHelper::
                           GetDescriptiveTextFor(_deliveryDateTime);
    updateDateUi();
}

void NewEntryDialog::updateDateTimeData(QDateTime dateTime, QString text)
{
    _deliveryDateTime    = dateTime;
    _deliveryDateTimeStr = text;

    updateDateUi();
}

//void NewEntryDialog::updateUrgencyData()
//{
//    bool urgency0Checked   = ui->Urgency0RadioButton->isChecked();
//    bool urgency50Checked  = ui->Urgency50RadioButton->isChecked();
//    bool urgency100Checked = ui->Urgency100RadioButton->isChecked();

//    if (urgency0Checked)
//        _urgencyLevel = NormalLevel;
//    else if (urgency50Checked)
//        _urgencyLevel = MediumLevel;
//    else if (urgency100Checked)
//        _urgencyLevel = HighLevel;

//    updatePaymentData();
//}

void NewEntryDialog::updatePaymentTypeData()
{
    const int currentRow      = ui->PaymentTypeComboBox->currentIndex();
    const int completePaymRow = 0;
    const int partialPaymRow  = 1;

    switch (currentRow) {
        case completePaymRow:
            _paymentType = CompletePayment;
            break;

        case partialPaymRow:
            _paymentType = PartialPayment;
            break;
    }

    updatePaymentData();
}

void NewEntryDialog::updatePaymentData()
{
    _totalToPayGross = _clothesPriceSum;
    _totalToPayNet   = _totalToPayGross;

    //
    switch (_paymentType) {
        case CompletePayment:
            _partPaymValue     = 0;
            _totalPayed        = _totalToPayNet;
            _missingPayment    = 0;
            break;

        case PartialPayment:
            _partPaymValue     = ui->PartPaymAmountSpinBox->value();
            _totalPayed        = _partPaymValue;
            _missingPayment    = _totalToPayNet - _totalPayed;
            break;
    }

    _minPartPaymValue = 0;
    _maxPartPaymValue = _totalToPayNet - 1;

    updatePaymentUi();
}

void NewEntryDialog::updateAllUi()
{
    updateReceiptUi();
    updateClientUi();
    updateClothesUi();
    updateDateUi();
    updatePaymentUi();
}

void NewEntryDialog::updateReceiptUi()
{
    ui->ReceiptNumberLabel->setText(QString("Fatura Número %1")
                            .arg(_receiptNumber));
}

void NewEntryDialog::updateClientUi()
{
    if ((_client.Id <= 0) && (!_registerClient))
        _clientNameStr = "Não foi selecionado nenhum cliente";
    else
        _clientNameStr = _client.Name;

    ui->ClientNameLabel->setText(_clientNameStr);
}

void NewEntryDialog::updateClothesUi()
{
    QString countTxt;
    QString priceSumTxt;

    if (_clothList.size() == 0) {
        countTxt    = "Não foram adicionados tecidos";
        priceSumTxt = "Preço Total: 0 Kz(s)";
    }
    else {
        countTxt    = QString("Adicionado(s) %1 tecido(s)")
                      .arg(_clothList.size());
        priceSumTxt = QString("Preço Total: %L1 Kz(s)")
                      .arg(_clothesPriceSum);
    }

    ui->ClothesCountLabel->setText(countTxt);
    ui->ClothesPriceSumLabel->setText(priceSumTxt);
}

void NewEntryDialog::updateDateUi()
{
    ui->DeliveryDateLabel->setText(_deliveryDateTimeStr);
}

void NewEntryDialog::updatePaymentUi()
{
    QString totalToPayTxt  = QString("%L1 Kz(s)").arg(_totalToPayNet);
    QString totalPayedTxt  = QString("%L1 Kz(s)").arg(_totalPayed);
    QString missingPaymTxt = QString("%L1 Kz(s)").arg(_missingPayment);

    ui->PartPaymAmountSpinBox->setRange(_minPartPaymValue, _maxPartPaymValue);
    ui->TotalToPayLabel->setText(totalToPayTxt);
    ui->TotalPayedLabel->setText(totalPayedTxt);
    ui->MissingPaymLabel->setText(missingPaymTxt);

    // Show or hide partial payment field from the layout
    // Depending of the type of payment
    // PartPaymentAmount will be reseted to 0 if payment type
    // is CompletePayment
    QFormLayout* layout = ui->PaymentFormLayout;
    QWidget*     label  = (QWidget*)ui->PartPaymAmountLabel;
    QWidget*     field  = (QWidget*)ui->PartPaymAmountSpinBox;
    int          row    = 1;

    switch (_paymentType) {
        case CompletePayment:
            label->setVisible(false);
            field->setVisible(false);
            layout->removeWidget(label);
            layout->removeWidget(field);
            ui->PartPaymAmountSpinBox->setValue(0);
            break;

        case PartialPayment:
            label->setVisible(true);
            field->setVisible(true);
            layout->insertRow(row, label, field);
            break;
    }
}

void NewEntryDialog::saveAndPrintReceipt()
{
    if (!isReceiptValid()) return;

    Receipt receipt;

    // Receipt basic data
    receipt.Number               = _receiptNumber;
    receipt.ClientData           = _client;
    receipt.UserAccountData      = UserSessionHelper::GetInstance().GetLoggedUser();
    receipt.RegistrationDateTime = QDateTime::currentDateTime();
    receipt.DeliveryDateTime     = _deliveryDateTime;
    receipt.Clothes              = _clothList;

    // Receipt payment data
    receipt.PaymentKind      = _paymentType;
    receipt.Discount         = NoDiscount;       // Change this later please
    receipt.TotalToPayGross  = _totalToPayGross;
    receipt.TotalToPayNet    = _totalToPayNet;
    receipt.TotalPayed       = _totalPayed;

    DatabaseHelper& dbHelper = DatabaseHelper::GetInstance();
    try {
        dbHelper.Transaction();
        dbHelper.InsertReceipt(receipt, _registerClient);
        dbHelper.Commit();

        ReceiptPrinterHelper::GetInstance().PrintReceipt(receipt, true);
        done(0);
    }
    catch(SplashException &e) {
        dbHelper.Rollback();
        ErrorHelper::GetInstance()
            .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

int NewEntryDialog::getNextReceiptNumber()
{
    QString text = "SELECT MAX(ReceiptNumber) FROM Receipt";
    unsigned int nextNum = 0;
    try {
        QSqlQuery query;
        if (!query.exec(text)){
            QString errorTxt = query.lastError().text();
            throw SplashException(errorTxt, _classTag);
        }

        if (!query.next())
            nextNum += 1;
        else
            nextNum = query.value(0).toUInt() + 1;

        return nextNum;
    }
    catch (SplashException &e) {
        ErrorHelper::GetInstance()
            .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }

    return 0;
}

bool NewEntryDialog::isReceiptValid()
{
    bool isValid = true;
    QString errorTxt;

    // Only accept an zero id client,
    // if he's supposed to be registered
    if ((_client.Id <= 0) && (!_registerClient)) {
        isValid = false;
        errorTxt += "Não foi selecionado nenhum cliente.\n";
    }

    if (!_clothList.size()) {
        isValid = false;
        errorTxt += "Não foram adicionados tecidos";
    }

    if (!isValid){
        QMessageBox::warning(this, windowTitle(),
                             errorTxt);
    }

    return isValid;
}
