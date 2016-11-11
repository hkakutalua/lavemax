#include "edittissuesdialog.h"
#include "ui_edittissuesdialog.h"

using namespace DatabaseType;

EditTissuesDialog::EditTissuesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTissuesDialog)
{
    ui->setupUi(this);
    setUpConnections();

    updateServiceModel();
}

EditTissuesDialog::EditTissuesDialog(QList<Cloth> clothes, QWidget *parent) :
    EditTissuesDialog(parent)
{
    addTissueListToTable(clothes);
}

EditTissuesDialog::~EditTissuesDialog()
{
    delete ui;
}

void EditTissuesDialog::setUpConnections()
{
    // /////////////////////////////////////////// //
    connect(this, SIGNAL(serviceModelUpdated()),
            this, SLOT(updateServiceData()));

    connect(this, SIGNAL(clothModelUpdated()),
            this, SLOT(updateClothData()));

    connect(this, SIGNAL(treatmentModelUpdated()),
            this, SLOT(updateTreatmentData()));

    // /////////////////////////////////////////// //
    connect(this, SIGNAL(serviceDataUpdated()),
            this, SLOT(updateClothModel()));

    connect(this, SIGNAL(clothDataUpdated()),
            this, SLOT(updateTreatmentModel()));

    connect(this, SIGNAL(treatmentDataUpdated()),
            this, SLOT(updateClothTreatmentData()));

    connect(this, SIGNAL(clothTreatmentDataUpdated()),
            this, SLOT(updatePriceChargedData()));

    connect(this, SIGNAL(urgencyDataUpdated()),
            this, SLOT(updatePriceChargedData()));

    connect(this, SIGNAL(squareMetersDataUpdated()),
            this, SLOT(updatePriceChargedData()));

    // /////////////////////////////////////////// // UI
    connect(this, SIGNAL(clothTreatmentDataUpdated()),
            this, SLOT(updateClothTreatmentUi()));

    connect(this, SIGNAL(priceChargedDataUpdated()),
            this, SLOT(updatePriceChargedUi()));

    // /////////////////////////////////////////// //
    auto serviceComboBox   = ui->ServiceTypeComboBox;
    auto clothComboBox     = ui->ClothTypeComboBox;
    auto treatmentComboBox = ui->TreatmentTypeComboBox;

    connect(serviceComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateServiceData()));
    connect(clothComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateClothData()));
    connect(treatmentComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateTreatmentData()));

    auto urgencyButtonGroup = ui->UrgencyButtonGroup;
    connect(urgencyButtonGroup, SIGNAL(buttonToggled(int,bool)),
            this, SLOT(updateUrgencyData()));
    urgencyButtonGroup->setId(ui->NormalUrgencyRadio, NormalLevel);
    urgencyButtonGroup->setId(ui->MediumUrgencyRadio, MediumLevel);
    urgencyButtonGroup->setId(ui->HighUrgencyRadio,   HighLevel);

    connect(ui->SquareMetersSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(updateSquareMetersData()));

    connect(ui->AddTissueButton, SIGNAL(clicked()),
            this, SLOT(addTissueToTable()));
    connect(ui->RemoveTissueButton, SIGNAL(clicked()),
            this, SLOT(removeTissueFromTable()));
    connect(ui->RemoveAllTissuesButton, SIGNAL(clicked()),
            this, SLOT(removeAllTissuesFromTable()));

    connect(ui->ConfirmButton, SIGNAL(clicked()),
            this, SLOT(retrieveTissuesAndQuit()));
    connect(ui->CancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));
}

void EditTissuesDialog::updateServiceModel()
{
    const int  nameColumn = 1;
    QComboBox* comboBox   = ui->ServiceTypeComboBox;

    try {
        QSqlQuery query = queryServices();
        _serviceModel.setQuery(query);
    }
    catch (const SplashException &e) {
        _serviceModel.clear();
        ErrorHelper::GetInstance()
        .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }

    comboBox->setModel(&_serviceModel);
    comboBox->setModelColumn(nameColumn);
    comboBox->setCurrentIndex(0);

    emit serviceModelUpdated();
}

void EditTissuesDialog::updateClothModel()
{
    const int  nameColumn = 1;
    QComboBox* comboBox   = ui->ClothTypeComboBox;

    try {
        QSqlQuery query = queryClothes(_service.Id);
        _clothModel.setQuery(query);
    }
    catch (const SplashException &e) {
        _clothModel.clear();
        ErrorHelper::GetInstance()
        .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }

    comboBox->setModel(&_clothModel);
    comboBox->setModelColumn(nameColumn);
    comboBox->setCurrentIndex(0);

    emit clothModelUpdated();
}

void EditTissuesDialog::updateTreatmentModel()
{
    const int  nameColumn = 1;
    QComboBox* comboBox   = ui->TreatmentTypeComboBox;

    try {
        QSqlQuery query = queryTreatments(_cloth.Id);
        _treatmentModel.setQuery(query);
    }
    catch (const SplashException &e) {
        ErrorHelper::GetInstance()
        .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }

    comboBox->setModel(&_treatmentModel);
    comboBox->setModelColumn(nameColumn);
    comboBox->setCurrentIndex(0);

    emit treatmentModelUpdated();
}

void EditTissuesDialog::updateServiceData()
{
    const int  idColumn   = 0;
    const int  nameColumn = 1;
    QComboBox* comboBox   = ui->ServiceTypeComboBox;
    auto       model      = &_serviceModel;

    if (model->rowCount() == 0) {
        comboBox->clear();
        _service = ServiceType();
    }
    else {
        const int curRow = comboBox->currentIndex();
        uint serviceId   = model->index(curRow, idColumn).data().toUInt();
        QString name     = model->index(curRow, nameColumn).data().toString();

        _service.Id      = serviceId;
        _service.Name    = name;
    }

    emit serviceDataUpdated();
}

void EditTissuesDialog::updateClothData()
{
    const int  idColumn   = 0;
    const int  nameColumn = 1;
    QComboBox* comboBox   = ui->ClothTypeComboBox;
    auto       model      = &_clothModel;

    if (model->rowCount() == 0) {
        comboBox->clear();
        _cloth = ClothType();
    }
    else {
        const int curRow = comboBox->currentIndex();
        uint clothId     = model->index(curRow, idColumn).data().toUInt();
        QString name     = model->index(curRow, nameColumn).data().toString();

        _cloth.Id        = clothId;
        _cloth.Name      = name;
    }

    emit clothDataUpdated();
}

void EditTissuesDialog::updateTreatmentData()
{
    const int  idColumn   = 0;
    const int  nameColumn = 1;
    QComboBox* comboBox   = ui->TreatmentTypeComboBox;
    auto       model      = &_treatmentModel;

    if (model->rowCount() == 0) {
        comboBox->clear();
        _treatment = TreatmentType();
    }
    else {
        const int curRow = comboBox->currentIndex();
        uint treatmentId = model->index(curRow, idColumn).data().toUInt();
        QString name     = model->index(curRow, nameColumn).data().toString();

        _treatment.Id    = treatmentId;
        _treatment.Name  = name;
    }

    emit treatmentDataUpdated();
}

void EditTissuesDialog::updateClothTreatmentData()
{
    const int   priceColumn           = 0;
    const int   isChargedMetersColumn = 1;

    try {
        QSqlQuery query = queryClothTreatment(_cloth.Id,
                                              _treatment.Id);
        QSqlQueryModel model;
        model.setQuery(query);

        if (model.rowCount() == 0) {
            _basePrice = 0;
            _isChargedPerSqrMeters = false;
        }
        else {
            _basePrice             = model.index(0, priceColumn)
                                          .data()
                                          .toDouble();
            _isChargedPerSqrMeters = model.index(0, isChargedMetersColumn)
                                          .data()
                                          .toBool();
        }
    }
    catch (const SplashException &e) {
        _basePrice = 0;
        _isChargedPerSqrMeters = false;
        ErrorHelper::GetInstance()
        .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }

    emit clothTreatmentDataUpdated();
}

void EditTissuesDialog::updateSquareMetersData()
{
    double value = ui->SquareMetersSpinBox->value();

    // Square Meters Size can't be 0 or negative
    if (_isChargedPerSqrMeters)
        _sizeInSqrMeters = (value > 0) ? value : 1;
    else
        _sizeInSqrMeters = 1;

    emit squareMetersDataUpdated();
}

void EditTissuesDialog::updateUrgencyData()
{
    int checkedButtonId = ui->UrgencyButtonGroup->checkedId();
    switch (checkedButtonId) {
        case NormalLevel:
            _urgency = NormalLevel;
            break;

        case MediumLevel:
            _urgency = MediumLevel;
            break;

        case HighLevel:
            _urgency = HighLevel;
            break;

        default: _urgency = NormalLevel;
    }

    emit urgencyDataUpdated();
}

void EditTissuesDialog::updatePriceChargedData()
{
    double clothPrice      = _basePrice;
    double sizeInSqrMeters = _sizeInSqrMeters;
    double urgencyPrice    = 0;

    if (_isChargedPerSqrMeters)
        clothPrice *= sizeInSqrMeters;

    switch (_urgency) {
        case NormalLevel:
            urgencyPrice = 0;
            break;

        case MediumLevel:
            urgencyPrice = clothPrice / 2;
            break;

        case HighLevel:
            urgencyPrice = clothPrice;
            break;

        default:
            urgencyPrice = 0;
    }

    _totalPriceCharged = clothPrice + urgencyPrice;
    emit priceChargedDataUpdated();
}

void EditTissuesDialog::updateClothTreatmentUi()
{
    if (_isChargedPerSqrMeters) {
        setSizeInSqrMetersFieldVisible(true);
        setSizeFieldVisible(false);
    }
    else {
        setSizeInSqrMetersFieldVisible(false);
        setSizeFieldVisible(true);
    }
}

void EditTissuesDialog::updatePriceChargedUi()
{
    QString totalPriceTxt = QString("%L1 Kz(s)")
                             .arg(_totalPriceCharged);
    ui->TissuePriceChargedLabel->setText(totalPriceTxt);
}

void EditTissuesDialog::updateClothesPriceSumUi()
{
    double totalClothesPrice = 0;
    for (int i = 0; i < _clothList.size(); ++i) {
        totalClothesPrice += _clothList[i].PriceCharged;
    }

    ui->TotalPriceLabel->setText(QString("Preço Total: %L1 Kz(s)")
                                 .arg(totalClothesPrice));
}

void EditTissuesDialog::updateAddedTissuesGroupBoxTitle()
{
    QString title = QString("%1 Tecido(s) Adicionado(s)")
                     .arg(_clothList.size());
    ui->AddedTissuesGroupBox->setTitle(title);
}

void EditTissuesDialog::addTissueToTable(Cloth cloth)
{
    // Build entirelly a cloth object if cloth parameter is empty
    if (cloth.Service.Id == 0) {
        cloth.Service               = _service;
        cloth.ClothKind             = _cloth;
        cloth.TreatmentKind         = _treatment;
        cloth.Size                  = ui->TissueSizeEntry->text();
        cloth.Brand                 = ui->TissueBrandEntry->text();
        cloth.Color                 = ui->TissueColorComboBox->currentText();
        cloth.Observations          = ui->ObservationsEntry->text();
        cloth.Urgency               = _urgency;
        cloth.PriceCharged          = _totalPriceCharged;
        cloth.IsChargedPerSqrMeters = _isChargedPerSqrMeters;

        if (cloth.IsChargedPerSqrMeters)
            cloth.SizeInSquareMeters = _sizeInSqrMeters;
    }
    _clothList.append(cloth);

    // //////////////////////////////////////////////////////// //
    QList<QTableWidgetItem*> tableItems;
    QTableWidgetItem *squareMetersItem = new QTableWidgetItem;
    if (cloth.IsChargedPerSqrMeters){
        squareMetersItem->setText(QString("%L1 Metros²")
                            .arg(cloth.SizeInSquareMeters));
    }

    tableItems << new QTableWidgetItem(cloth.Service.Name)
               << new QTableWidgetItem(cloth.ClothKind.Name)
               << new QTableWidgetItem(cloth.TreatmentKind.Name)
               << new QTableWidgetItem(QString("%1%").arg(cloth.Urgency))
               << new QTableWidgetItem(QString("%L1 Kz(s)").arg(cloth.PriceCharged))
               << new QTableWidgetItem(cloth.Size)
               << squareMetersItem
               << new QTableWidgetItem(cloth.Brand)
               << new QTableWidgetItem(cloth.Color)
               << new QTableWidgetItem(cloth.Observations);

    auto table = ui->TissuesTableWidget;
    table->setRowCount(table->rowCount() + 1);
    const int lastInsertedRow = table->rowCount() - 1;

    for(int column=0; column<tableItems.size(); ++column)
        table->setItem(lastInsertedRow, column, tableItems.at(column));

    clearTissueDialog();
    updateClothesPriceSumUi();
    updateAddedTissuesGroupBoxTitle();
}

void EditTissuesDialog::addTissueListToTable(QList<Cloth> list)
{
    for (Cloth item : list)
        addTissueToTable(item);
}

void EditTissuesDialog::removeTissueFromTable()
{
    auto table = ui->TissuesTableWidget;
    _clothList.removeAt(table->currentRow());
    table->removeRow(table->currentRow());

    updateClothesPriceSumUi();
    updateAddedTissuesGroupBoxTitle();
}

void EditTissuesDialog::removeAllTissuesFromTable()
{
    _clothList.clear();
    ui->TissuesTableWidget->setRowCount(0);

    updateClothesPriceSumUi();
    updateAddedTissuesGroupBoxTitle();
}

void EditTissuesDialog::clearTissueDialog()
{
    ui->TissueSizeEntry->clear();
    ui->TissueBrandEntry->clear();
    ui->TissueColorComboBox->setCurrentIndex(0);
    ui->ObservationsEntry->clear();
    ui->NormalUrgencyRadio->setChecked(true);
}

void EditTissuesDialog::retrieveTissuesAndQuit()
{
    emit TissuesListUpdated(_clothList);
    done(0);
}

QSqlQuery EditTissuesDialog::queryServices()
{
    QSqlQuery query("SELECT ServiceTypeId, Name FROM ServiceType");
    QSqlError error = query.lastError();
    if(error.isValid())
        throw SplashException(error.text() + "\n" +
                              query.lastQuery(),
                              _tag);

    return query;
}

QSqlQuery EditTissuesDialog::queryClothes(unsigned int serviceId)
{
    QSqlQuery query;
    query.prepare("SELECT ClothTypeId, Name FROM ClothType"
                  " WHERE ServiceTypeId = :ServiceTypeId");
    query.bindValue(":ServiceTypeId", serviceId);
    if (!query.exec())
        throw SplashException(query.lastError().text() + "\n" +
                              query.lastQuery(), _tag);

    return query;
}

QSqlQuery EditTissuesDialog::queryTreatments(unsigned int clothId)
{
    QSqlQuery query;
    query.prepare("SELECT  CT.TreatmentTypeId, T.Name FROM ClothTreatment CT"
                  " JOIN TreatmentType T"
                  "  ON T.TreatmentTypeId = CT.TreatmentTypeId"
                  " WHERE  CT.ClothTypeId = :ClothTypeId");
    query.bindValue(":ClothTypeId", clothId);
    if (!query.exec())
        throw SplashException(query.lastError().text() + "\n" +
                              query.lastQuery(), _tag);

    return query;
}

QSqlQuery EditTissuesDialog::queryClothTreatment(unsigned int clothId,
                                                 unsigned int treatmentId)
{
    QSqlQuery query;
    query.prepare("SELECT ClothTreatment.Price, ClothTreatment.ChargedPerSqrMeters"
                  "  FROM  ClothTreatment"
                  " WHERE  ClothTypeId = :ClothTypeId"
                  "  AND   TreatmentTypeId = :TreatmentTypeId");
    query.bindValue(":ClothTypeId", clothId);
    query.bindValue(":TreatmentTypeId", treatmentId);
    if (!query.exec())
        throw SplashException(query.lastError().text() + "\n" +
                              query.lastQuery(), _tag);

    return query;
}

void EditTissuesDialog::setSizeFieldVisible(bool visible)
{
    const int sizeFieldRow = 3;
    if (visible) {
        ui->AddTissueFormLayout->insertRow(sizeFieldRow,
                                    ui->TissueSizeLabel,
                                    ui->TissueSizeEntry);
        ui->TissueSizeLabel->show();
        ui->TissueSizeEntry->show();
    }
    else {
        ui->AddTissueFormLayout->removeWidget(ui->TissueSizeLabel);
        ui->AddTissueFormLayout->removeWidget(ui->TissueSizeEntry);
        ui->TissueSizeLabel->hide();
        ui->TissueSizeEntry->hide();
    }
}

void EditTissuesDialog::setSizeInSqrMetersFieldVisible(bool visible)
{
    const int sizeFieldRow = 3;
    if (visible) {
        ui->AddTissueFormLayout->insertRow(sizeFieldRow,
                                    ui->SquareMetersLabel,
                                    ui->SquareMetersSpinBox);
        ui->SquareMetersLabel->show();
        ui->SquareMetersSpinBox->show();
    }
    else {
        ui->AddTissueFormLayout->removeWidget(ui->SquareMetersLabel);
        ui->AddTissueFormLayout->removeWidget(ui->SquareMetersSpinBox);
        ui->SquareMetersLabel->hide();
        ui->SquareMetersSpinBox->hide();
    }
}
