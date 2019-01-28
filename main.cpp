#include <QApplication>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include "Views/logindialog.h"
#include "Helpers/errorhelper.h"

using namespace ErrorHelperType;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("fusion");
    app.setOrganizationDomain("splashinkd.com");
    app.setApplicationName("Lavemax");

    QLocale::setDefault(QLocale(QLocale::Portuguese, QLocale::Angola));

    try {
        QSqlDatabase database = QSqlDatabase::addDatabase("QPSQL");
        database.setDatabaseName("lavemax");
        database.setHostName("localhost");
        database.setUserName("postgres");
        database.setPassword("admin128");
        if (!database.open()) {
            throw SplashException(database.lastError().text(),
                                  "Main - Database error",
                                  ErrorHelperType::CriticalError);
        }

        LoginDialog loginDialog;
        loginDialog.show();

        return app.exec();;
    }
    catch (const SplashException &e) {
        ErrorHelper::GetInstance()
                    .SaveAndShowError(e.Tag(), e.Level(), e.What());
        return -1;
    }
}
