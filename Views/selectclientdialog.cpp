#include "selectclientdialog.h"
#include "ui_selectclientdialog.h"

using namespace SelectClientDialogType;
using namespace DatabaseType;

SelectClientDialog::SelectClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectClientDialog)
{
    ui->setupUi(this);
    setUpConnections();
    _tag = this->metaObject()->className();

    ui->ClientTableView->setModel(&_clientModel);
    ui->ClientTableView->horizontalHeader()
                       ->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ClientTableView->horizontalHeader()
                       ->setStretchLastSection(true);

    ui->ClientPhoneEntry->setValidator(&_phoneValidator);
    updateClientSearchCriteria();
}

SelectClientDialog::~SelectClientDialog()
{
    delete ui;
}

void SelectClientDialog::setUpConnections()
{
    connect(ui->SelectionTabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(updateSelectionModeData()));

    connect(ui->SearchCriteriaComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateClientSearchCriteria()));

    connect(ui->SearchEntry, SIGNAL(textEdited(QString)),
            this, SLOT(updateClientModelData()));

    connect(ui->SelectButton, SIGNAL(clicked()), this, SLOT(selectClientAndQuit()));
    connect(ui->CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void SelectClientDialog::updateClientSearchCriteria()
{
    _searchCriteria = (SearchCriteria)ui->SearchCriteriaComboBox
                                        ->currentIndex();
    switch (_searchCriteria) {
        case SearchByName:
            ui->SearchEntry->setValidator(nullptr);
            break;

        case SearchByPhone:
            ui->SearchEntry->setValidator(&_phoneValidator);
            break;
    }
}

void SelectClientDialog::updateSelectionModeData()
{
    _selectionMode = (SelectionMode)ui->SelectionTabWidget
                                      ->currentIndex();
}

void SelectClientDialog::updateClientModelData()
{
    if (ui->SearchEntry->text().isEmpty()) {
        _clientModel.setQuery(QSqlQuery());
        return;
    }

    const int idColumn    = 0;
    const int nameColumn  = 1;
    const int phoneColumn = 2;
    const int addrColumn  = 3;
    QString searchText    = ui->SearchEntry->text() + "%";

    QSqlQuery query;
    try {
        QString columnWhereClause;
        switch(_searchCriteria) {
            case SearchByName:
                columnWhereClause = " WHERE lower(Name) LIKE lower(:SearchText)";
            break;

            case SearchByPhone:
                columnWhereClause = " WHERE PhoneNumber LIKE :SearchText";
            break;
        }

        query.prepare("SELECT ClientId, Name, PhoneNumber, Address"
                      " FROM Client"
                      + columnWhereClause +
                      " LIMIT 20");
        query.bindValue(":ColumnName", columnWhereClause);
        query.bindValue(":SearchText", searchText);
        if (!query.exec())
            throw SplashException(query.lastError().text(),
                                  _tag);

        _clientModel.setQuery(query);
        _clientModel.setHeaderData(nameColumn, Qt::Horizontal, "Nome");
        _clientModel.setHeaderData(phoneColumn, Qt::Horizontal, "Telemóvel");
        _clientModel.setHeaderData(addrColumn, Qt::Horizontal, "Endereço");
        ui->ClientTableView->setColumnHidden(idColumn, true);
    }
    catch (SplashException &e) {
        ErrorHelper::GetInstance()
            .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

void SelectClientDialog::selectClientAndQuit()
{
    if (!validSelection())
        return;

    const int selectedRow        = ui->ClientTableView->currentIndex().row();
    const QModelIndex idIndex    = _clientModel.index(selectedRow, 0);
    const QModelIndex nameIndex  = _clientModel.index(selectedRow, 1);
    const QModelIndex phoneIndex = _clientModel.index(selectedRow, 2);
    const QModelIndex addrIndex  = _clientModel.index(selectedRow, 3);

    Client client;
    bool   registerClient = false;
    switch (_selectionMode) {
        case JustSelect:
            client.Id          = _clientModel.data(idIndex).toUInt();
            client.Name        = _clientModel.data(nameIndex).toString();
            client.PhoneNumber = _clientModel.data(phoneIndex).toString();
            client.Address     = _clientModel.data(addrIndex).toString();
            registerClient     = false;
            break;

        case RegisterAndSelect:
            client.Id          = 0;
            client.Name        = ui->ClientNameEntry->text();
            client.PhoneNumber = ui->ClientPhoneEntry->text();
            client.Address     = ui->ClientAddressEntry->text();
            registerClient     = true;
            break;
    }

    emit ClientSelectionDone(client, registerClient);
    done(0);
}

bool SelectClientDialog::validSelection()
{
    bool    isValid = true;
    QString alertTxt;

    if (_selectionMode == JustSelect) {
        QModelIndex currentIndex = ui->ClientTableView->currentIndex();
        if (!currentIndex.isValid()) {
            alertTxt = "Nenhum cliente foi selecionado.";
            isValid  = false;
        }
    }
    else if (_selectionMode == RegisterAndSelect){
        QString clientName  = ui->ClientNameEntry->text();
        QString clientPhone = ui->ClientPhoneEntry->text();

        if (clientName.isEmpty() || clientPhone.isEmpty()) {
            alertTxt = "O nome do cliente e o número de telemóvel "
                       "não podem estar vazios.";
            isValid  = false;
        }
    }

    if (!isValid) {
        QMessageBox::warning(this, windowTitle(),
                             alertTxt);
    }

    return isValid;
}
