#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace MainWindowType;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->NewEntryAction, SIGNAL(triggered()),
            this, SLOT(openNewEntryDialog()));
    connect(ui->ManageClientsAction, SIGNAL(triggered()),
            this, SLOT(openManageClientsDialog()));
    connect(ui->ManageUsersAction, SIGNAL(triggered()),
            this, SLOT(openManageUsersDialog()));
    connect(ui->VisualizeEntryAction, SIGNAL(triggered()),
            this, SLOT(openVisualizeEntryDialog()));
    connect(ui->TodayFinancialReportAction, SIGNAL(triggered()),
            this, SLOT(openFinancialReportDialog()));

    connect(ui->actionSettings, SIGNAL(triggered()),
            this, SLOT(openSettingsDialog()));

    connect(ui->SearchCriteriaComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateSearchCriteria()));
    connect(ui->SearchEntry, SIGNAL(textEdited(QString)),
            this, SLOT(updateReceiptTableView()));

    ui->ReceiptTableView->setModel(&receiptTableModel);
    updateSearchCriteria();
    updateReceiptTableView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openNewEntryDialog()
{
    NewEntryDialog *dialog = new NewEntryDialog(this);
    dialog->exec();
    updateReceiptTableView();
}

void MainWindow::openVisualizeEntryDialog()
{
    QTableView *table = ui->ReceiptTableView;
    int currentRow = table->currentIndex().row();
    if (currentRow == -1) {
        QMessageBox::information(this, "Visualizar Fatura",
                                 "Selecione primeiro a fatura a ser visualizada");
        return;
    }

    QModelIndex receiptIndex   = table->model()->index(currentRow, 0);
    unsigned int receiptNumber = receiptIndex.data().toUInt();

    VisualizeReceiptDialog *dialog = new VisualizeReceiptDialog(receiptNumber, this);
    dialog->exec();
}

void MainWindow::openManageClientsDialog()
{
    ManageClientsDialog *dialog = new ManageClientsDialog(this);
    dialog->exec();
}

void MainWindow::openManageUsersDialog()
{
    ManageUsersDialog *dialog = new ManageUsersDialog(this);
    dialog->exec();
}

void MainWindow::openFinancialReportDialog()
{
//    FinancialReportDialog dialog = FinancialReportDialog(this);
//    dialog.exec();
    try {
        UserSessionHelper::GetInstance().PrintUserFinancialReport();
    }
    catch (const SplashException &e) {
        ErrorHelper::GetInstance()
            .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }
}

void MainWindow::openSettingsDialog()
{
    SettingsDialog *dialog = new SettingsDialog(this);
    dialog->exec();
}

void MainWindow::updateSearchCriteria()
{
    _searchCriteria = (SearchCriteria)ui->SearchCriteriaComboBox->currentIndex();
    ui->SearchEntry->clear();
    switch (_searchCriteria) {
        case SearchByPhone:
            ui->SearchEntry->setValidator(&_phoneValidator);
            break;

        case SearchByReceiptNumber:
            ui->SearchEntry->setValidator(&_receiptValidator);
            break;

        case SearchByClientName:
            ui->SearchEntry->setValidator(nullptr);
            break;
    }
}

void MainWindow::updateReceiptTableView()
{
    QString searchText = ui->SearchEntry->text();
    QString whereCond;
    QString field;
    QString valuePlaceHolder;
    switch (_searchCriteria) {
        case SearchByPhone:
            field = "c.PhoneNumber";
            valuePlaceHolder = ":PhoneNumber";
            whereCond = "WHERE " + field + " LIKE " + valuePlaceHolder;
            break;

        case SearchByReceiptNumber:
            field = "r.ReceiptNumber";
            valuePlaceHolder = ":ReceiptNumber";
            whereCond = "WHERE " + field + " = " + valuePlaceHolder;
            break;

        case SearchByClientName:
            field = "c.Name";
            valuePlaceHolder = ":ClientName";
            whereCond = "WHERE lower(" + field + ") LIKE lower(" + valuePlaceHolder + ")";
            break;
    }

    QString queryTxt =
        "SELECT r.ReceiptNumber, u.Name, c.Name, r.RegistrationDate,"
        "       r.DeliveryDate, rp.TotalToPayNet"
        " FROM Receipt AS r"
        " INNER JOIN UserAccount AS u"
        "  ON u.UserAccountId = r.UserAccountId"
        " INNER JOIN Client AS c"
        "  ON c.ClientId = r.ClientId"
        " INNER JOIN ReceiptPayment AS rp"
        "  ON rp.ReceiptNumber = r.ReceiptNumber"
        " " + whereCond +
        " ORDER BY r.RegistrationDate DESC"
        " LIMIT 50";

    QSqlQuery query;
    query.prepare(queryTxt);
    switch (_searchCriteria) {
        case SearchByPhone:
        case SearchByClientName:
            query.bindValue(valuePlaceHolder, searchText.append("%"));
            break;

        case SearchByReceiptNumber:
            query.bindValue(valuePlaceHolder, searchText.toUInt());
            break;
    }

    try {
        if (!query.exec()) {
            throw SplashException(query.lastError().text() + "\n" +
                                  query.lastQuery(),
                                  "MainWindow");
        }

        receiptTableModel.setQuery(query);

        // Update table's header
        const int receiptNumberColumn = 0;
        const int userNameColumn = 1;
        const int clientNameColumn = 2;
        const int submissionDateColumn = 3;
        const int deliveryDateColumn = 4;
        const int totalPriceNetColumn = 5;

        receiptTableModel.setHeaderData(receiptNumberColumn, Qt::Horizontal, "Número da Fatura");
        receiptTableModel.setHeaderData(userNameColumn, Qt::Horizontal, "Funcionário");
        receiptTableModel.setHeaderData(clientNameColumn, Qt::Horizontal, "Cliente");
        receiptTableModel.setHeaderData(submissionDateColumn, Qt::Horizontal, "Data de Registo");
        receiptTableModel.setHeaderData(deliveryDateColumn, Qt::Horizontal, "Data de Entrega");
        receiptTableModel.setHeaderData(totalPriceNetColumn, Qt::Horizontal, "Total");
    }
    catch (const SplashException &e) {
        ErrorHelper::GetInstance()
                    .SaveAndShowError(e.Tag(), e.Level(), e.What());
    }

    ui->ReceiptTableView->setModel(&receiptTableModel);
    ui->ReceiptTableView->horizontalHeader()
            ->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::showEvent(QShowEvent *e)
{
    e->accept();

    auto user = UserSessionHelper::GetInstance().GetLoggedUser();
    ui->GreetingsLabel->setText(QString("Benvindo(a) %1.")
                                .arg(user.Name));
    updateReceiptTableView();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    emit WindowClosed();
    e->accept();
}
