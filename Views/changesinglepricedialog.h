#ifndef CHANGESINGLEPRICEDIALOG_H
#define CHANGESINGLEPRICEDIALOG_H

#include <QDialog>
#include <QDoubleValidator>
#include "Helpers/databasehelper.h"

namespace Ui {
class ChangeSinglePriceDialog;
}

class ChangeSinglePriceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeSinglePriceDialog(DatabaseType::ClothType clothType,
                                     DatabaseType::TreatmentType treatmentType,
                                     double oldPrice,
                                     QWidget *parent = 0);
    ~ChangeSinglePriceDialog();

private slots:
    void saveNewPriceAndClose();

private:
    Ui::ChangeSinglePriceDialog *ui;
    DatabaseType::ClothType clothType;
    DatabaseType::TreatmentType treatmentType;
    double oldPrice;
};

#endif // CHANGESINGLEPRICEDIALOG_H
