#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QShowEvent>
#include "Views/newentrydialog.h"
#include "Views/manageclientsdialog.h"
#include "Views/manageusersdialog.h"
#include "Views/financialreportdialog.h"
#include "Views/visualizereceiptdialog.h"
#include "Views/settingsdialog.h"
#include "Helpers/databasehelper.h"
#include "Helpers/usersessionhelper.h"
#include <QDebug>

namespace MainWindowType {
    enum SearchCriteria
    {
        SearchByPhone = 0,
        SearchByReceiptNumber,
        SearchByClientName
    };
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void WindowClosed();

private slots:
    void openNewEntryDialog();
    void openVisualizeEntryDialog();
    void openManageClientsDialog();
    void openManageUsersDialog();
    void openFinancialReportDialog();
    void updateSearchCriteria();
    void updateReceiptTableView();

    void openSettingsDialog();

private:
    void showEvent(QShowEvent *e);
    void closeEvent(QCloseEvent *e);

    MainWindowType::SearchCriteria _searchCriteria {MainWindowType::SearchByPhone};
    QSqlQueryModel receiptTableModel;
    QIntValidator _phoneValidator {0, 999999999};
    QIntValidator _receiptValidator {1, 999999999};

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
