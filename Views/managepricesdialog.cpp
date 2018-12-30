#include "managepricesdialog.h"
#include <QDebug>
#include "Views/changesinglepricedialog.h"
#include "ui_managepricesdialog.h"

using namespace DatabaseType;

ManagePricesDialog::ManagePricesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManagePricesDialog)
{
    ui->setupUi(this);

    pricesModel = new QSqlTableModel(this);

    connect(ui->CloseButton, SIGNAL(clicked(bool)),
            this, SLOT(close()));
    connect(ui->AlterSelectedClothPriceButton, SIGNAL(clicked(bool)),
            this, SLOT(openChangeSinglePriceDialog()));

    updatePricesTable();
}

ManagePricesDialog::~ManagePricesDialog()
{
    delete ui;
}

void ManagePricesDialog::openChangeSinglePriceDialog()
{
    QTableView *table = ui->PricesTableView;
    int currentRow = table->currentIndex().row();
    if (currentRow == -1) {
        QMessageBox::information(this, "Alter Preço",
                                 "Não foi selecionado nenhum serviço");
        return;
    }

    QModelIndex clothTypeIndex   = table->model()->index(currentRow, 0);
    QModelIndex treatmentTypeIndex   = table->model()->index(currentRow, 1);
    QModelIndex clothNameIndex   = table->model()->index(currentRow, 2);
    QModelIndex treatmentNameIndex   = table->model()->index(currentRow, 3);
    QModelIndex priceIndex = table->model()->index(currentRow, 4);

    ClothType clothType;
    clothType.Id = clothTypeIndex.data().toUInt();
    clothType.Name = clothNameIndex.data().toString();

    TreatmentType treatmentType;
    treatmentType.Id = treatmentTypeIndex.data().toUInt();
    treatmentType.Name = treatmentNameIndex.data().toString();

    ChangeSinglePriceDialog *dialog = new ChangeSinglePriceDialog(clothType, treatmentType,
                                            priceIndex.data().toDouble(), this);
    dialog->exec();
    updatePricesTable();
}

void ManagePricesDialog::updatePricesTable()
{
    QString queryTxt = "SELECT ClothTreatment.ClothTypeId, ClothTreatment.TreatmentTypeId,"
                       "  ClothType.Name, TreatmentType.Name, Price"
                       " FROM ClothTreatment AS ClothTreatment"
                       " JOIN ClothType AS ClothType"
                       "  ON ClothType.ClothTypeId = ClothTreatment.ClothTypeId"
                       " JOIN TreatmentType AS TreatmentType"
                       "  ON TreatmentType.TreatmentTypeId = ClothTreatment.TreatmentTypeId"
                       " WHERE ClothType.IsActive = true"
                       " ORDER BY ClothType.Name";
    QSqlQuery query;
    if (!query.exec(queryTxt))
    {
        qDebug() << query.lastError().text();
        this->close();
    }

    pricesModel->setQuery(query);
    pricesModel->setHeaderData(2, Qt::Horizontal, "Tecido");
    pricesModel->setHeaderData(3, Qt::Horizontal, "Tratamento");
    pricesModel->setHeaderData(4, Qt::Horizontal, "Preço");

    ui->PricesTableView->verticalHeader()->setVisible(false);
    ui->PricesTableView->setModel(pricesModel);
    ui->PricesTableView->setColumnHidden(0, true);
    ui->PricesTableView->setColumnHidden(1, true);
    ui->PricesTableView->horizontalHeader()
                       ->setSectionResizeMode(QHeaderView::Stretch);
}
