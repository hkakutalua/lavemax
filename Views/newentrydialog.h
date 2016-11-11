#ifndef NEWENTRYDIALOG_H
#define NEWENTRYDIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QCompleter>
#include <QProgressDialog>

#include "../Exceptions/splashexception.h"
#include "../Helpers/errorhelper.h"
#include "../Helpers/databasehelper.h"
#include "../Helpers/receiptprinterhelper.h"
#include "../Helpers/datetimehelper.h"
#include "../Helpers/usersessionhelper.h"
#include "selectclientdialog.h"
#include "edittissuesdialog.h"
#include "selectdatetimedialog.h"

namespace Ui {
class NewEntryDialog;
}

class NewEntryDialog : public QDialog
{
    Q_OBJECT

public:
    NewEntryDialog(QWidget *parent = 0);
    ~NewEntryDialog();

private slots:
    void setUpConnections();

    // Open dialogs and bind their signals to
    // the data update slots
    void openSelectClientDialog();
    void openEditTissuesDialog();
    void openSelectDateTimeDialog();

    // Update the selected client
    void updateClientData(DatabaseType::Client client,
                          bool registerClient);

    // Update clothes field(s) data
    void updateClothesData(QList<DatabaseType::Cloth> clothes);

    // Update the delivery field(s) data
    void updateDateTimeData(QDateTime dateTime, QString text);

    // Update urgency's field data
//    void updateUrgencyData();

    // Update payment type field data
    void updatePaymentTypeData();

    // Calculate and update payment data
    // The incoming partial payment amount is only assigned if the current
    // payment type is PartialPayment, otherwise payment amount data will
    // be reseted to 0
    void updatePaymentData();

    // Ui update methods
    void updateAllUi();
    void updateReceiptUi();
    void updateClientUi();
    void updateClothesUi();
    void updateDateUi();
    void updatePaymentUi();

    void saveAndPrintReceipt();

private:
    // Generate a receipt number for this receipt
    // The new receipt will have an incremented by one
    // number of the last inserted receipt
    int getNextReceiptNumber();

    // Check if receipt data are all valid
    bool isReceiptValid();

    QString _classTag;

    // Receipt basic fields
    unsigned int                _receiptNumber   {0};
    DatabaseType::Client        _client;
    QString                     _clientNameStr;
    bool                        _registerClient  {false};
    QList<DatabaseType::Cloth>  _clothList;
    double                      _clothesPriceSum {0};

    // Receipt date fields
    QDateTime                   _deliveryDateTime;
    QString                     _deliveryDateTimeStr;

    // Receipt payment fiels
    DatabaseType::PaymentType   _paymentType       {DatabaseType::CompletePayment};
    bool                        _isPaymentComplete {true};
    DatabaseType::DiscountLevel _discountLevel     {DatabaseType::NoDiscount};
    double                      _totalToPayGross   {0};
    double                      _totalToPayNet     {0};
    double                      _totalPayed        {0};
    double                      _missingPayment    {0};

    double                      _partPaymValue     {0};
    double                      _minPartPaymValue  {0};
    double                      _maxPartPaymValue  {0};

    Ui::NewEntryDialog *ui;
};

#endif // NEWENTRYDIALOG_H
