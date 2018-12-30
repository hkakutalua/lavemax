#include "changesinglepricedialog.h"
#include <QSqlQuery>
#include "Helpers/errorhelper.h"
#include "ui_changesinglepricedialog.h"

ChangeSinglePriceDialog::ChangeSinglePriceDialog(DatabaseType::ClothType clothType,
        DatabaseType::TreatmentType treatmentType, double oldPrice,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeSinglePriceDialog)
{
    ui->setupUi(this);
    connect(ui->CancelButton, SIGNAL(clicked(bool)),
            this, SLOT(close()));
    connect(ui->AlterPriceButton, SIGNAL(clicked(bool)),
            this, SLOT(saveNewPriceAndClose()));

    this->clothType = clothType;
    this->treatmentType = treatmentType;
    this->oldPrice = oldPrice;

    ui->AlterPriceGroupBox->setTitle(clothType.Name + " (" + treatmentType.Name + ")");
    ui->NewPriceSpinBox->setValue(oldPrice);
}

ChangeSinglePriceDialog::~ChangeSinglePriceDialog()
{
    delete ui;
}

void ChangeSinglePriceDialog::saveNewPriceAndClose()
{
    QSqlQuery query;
    try
    {
        query.prepare("UPDATE ClothTreatment"
                      " SET Price=:Price"
                      " WHERE ClothTypeId = :ClothTypeId"
                      " AND TreatmentTypeId = :TreatmentTypeId");
        query.bindValue(":ClothTypeId", clothType.Id);
        query.bindValue(":TreatmentTypeId", treatmentType.Id);
        query.bindValue(":Price", ui->NewPriceSpinBox->value());
        if (!query.exec())
        {
            throw SplashException(query.lastError().text(),
                                  "ChangeSinglePriceDialog");
        }
    }
    catch (const SplashException &e)
    {
        ErrorHelper::GetInstance().
            SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
    this->close();
}
