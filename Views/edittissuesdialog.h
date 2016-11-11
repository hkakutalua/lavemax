#ifndef EDITTISSUESDIALOG_H
#define EDITTISSUESDIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>

#include "../Exceptions/splashexception.h"
#include "../Helpers/databasehelper.h"

namespace Ui {
class EditTissuesDialog;
}

class EditTissuesDialog : public QDialog
{
    Q_OBJECT

public:
    EditTissuesDialog(QWidget *parent = 0);
    EditTissuesDialog(QList<DatabaseType::Cloth> clothes,
                      QWidget *parent = 0);
    ~EditTissuesDialog();

signals:
    void TissuesListUpdated(QList<DatabaseType::Cloth> clothes);

    // ////////////////////////////// //
    void serviceModelUpdated();
    void clothModelUpdated();
    void treatmentModelUpdated();

    // ////////////////////////////// //
    void serviceDataUpdated();
    void clothDataUpdated();
    void treatmentDataUpdated();
    void clothTreatmentDataUpdated();
    void squareMetersDataUpdated();
    void urgencyDataUpdated();
    void priceChargedDataUpdated();

private slots:
    void setUpConnections();

    // ////////////////////////////// //
    void updateServiceModel();
    void updateClothModel();
    void updateTreatmentModel();

    // ////////////////////////////// //
    void updateServiceData();
    void updateClothData();
    void updateTreatmentData();
    void updateClothTreatmentData();
    void updateSquareMetersData();
    void updateUrgencyData();
    void updatePriceChargedData();

    // ////////////////////////////// //
    void updateClothTreatmentUi();
    void updatePriceChargedUi();
    void updateClothesPriceSumUi();
    void updateAddedTissuesGroupBoxTitle();

    void addTissueToTable(DatabaseType::Cloth cloth = {});
    void addTissueListToTable(QList<DatabaseType::Cloth> list);
    void removeTissueFromTable();
    void removeAllTissuesFromTable();

    // ////////////////////////////// //
    void clearTissueDialog();

    void retrieveTissuesAndQuit();

private:
    QSqlQuery queryServices();
    QSqlQuery queryClothes       (unsigned int serviceId);
    QSqlQuery queryTreatments    (unsigned int clothId);
    QSqlQuery queryClothTreatment(unsigned int clothId,
                                  unsigned int treatmentId);

    void setSizeFieldVisible(bool visible);
    void setSizeInSqrMetersFieldVisible(bool visible);

    QString                     _tag;

    QSqlQueryModel              _serviceModel;
    QSqlQueryModel              _clothModel;
    QSqlQueryModel              _treatmentModel;

    DatabaseType::ServiceType   _service;
    DatabaseType::ClothType     _cloth;
    DatabaseType::TreatmentType _treatment;

    // ClothTreatment fields
    bool                        _isChargedPerSqrMeters {false};
    double                      _sizeInSqrMeters       {1};
    double                      _basePrice             {0};

    DatabaseType::UrgencyLevel  _urgency               {DatabaseType::NormalLevel};
    double                      _totalPriceCharged     {0};

    QList<DatabaseType::Cloth>  _clothList;

    Ui::EditTissuesDialog *ui;
};

#endif // EDITTISSUESDIALOG_H
