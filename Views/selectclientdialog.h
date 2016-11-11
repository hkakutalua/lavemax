#ifndef SELECTCLIENTDIALOG_H
#define SELECTCLIENTDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QIntValidator>
#include "../Helpers/databasehelper.h"
#include "../Exceptions/splashexception.h"

namespace Ui {
class SelectClientDialog;
}

namespace SelectClientDialogType {
    enum SelectionMode
    {
        JustSelect = 0,
        RegisterAndSelect
    };

    enum SearchCriteria
    {
        SearchByPhone = 0,
        SearchByName
    };
}

class SelectClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectClientDialog(QWidget *parent = 0);
    ~SelectClientDialog();

signals:
    void ClientSelectionDone(DatabaseType::Client client,
                             bool registerClient);

private slots:
    void setUpConnections();
    void updateClientSearchCriteria();
    void updateSelectionModeData();
    void updateClientModelData();

    void selectClientAndQuit();

private:
    bool validSelection();

    QString _tag;

    SelectClientDialogType::SelectionMode  _selectionMode  {SelectClientDialogType::JustSelect};
    SelectClientDialogType::SearchCriteria _searchCriteria {SelectClientDialogType::SearchByPhone};
    DatabaseType::Client                   _client;
    QSqlQueryModel                         _clientModel;

    QIntValidator _phoneValidator {0, 999999999};

    Ui::SelectClientDialog *ui;
};

#endif // SELECTCLIENTDIALOG_H
