#ifndef ERRORHELPER_H
#define ERRORHELPER_H

/*
 * Author: Henrick Pedro Kakutalwa
 * Date: 14/03/2016
 * Modification: 27/08/2016
 * -- Naming conventions of the class altered to CamelCase
 * -------------------------------------------------------------------------------------
 * ErrorHelper class is responsible for showing errors to user, as well saving the
 * errors in log files. You can handle sweetly SplashException exceptions by using this
 * class.
 * ErrorLevel enum specifies the seriousness of the error (if it's an warning
 * or a critical error).
 *
 * --- ShowError
 * Shows a MessageBox to user, where:
 *  tag:    appears in the first paragraph of the MessageBox text, normally is the
 *          source of error
 *  level:  represents the severity of the error, so, the MessageBox will appear
 *          different depending of the error level
 *  text:   explains why the error occurred.
 *
 * --- SaveError
 * Saves the error in a log file, this file is created with the name format:
 *  [current-day]-[current-month]-[current-year]
 * Multiples errors are logged in the same file if they occur in the same date.
 *
 * The log content is saved in this way:
 *  [hour]:[minute]:[second]
 *  [[tag]]
 *  - [level]:
 *  [text]
 *
 * Example:
 * 18:40:54
 * [DbHelper]
 * - Erro:
 * The user 'admin' has not privileges to delete receipts
 * -------------------------------------------------------------------------------------
 *
 *
*/

#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDate>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include "appdatadirhelper.h"

namespace ErrorHelperType{
    enum ErrorLevel
    {
        WarningError,
        CriticalError
    };
}

class ErrorHelper
{
public:
    static ErrorHelper& GetInstance()
    {
        static ErrorHelper instance;
        return instance;
    }

    void ShowError(const QString &tag, ErrorHelperType::ErrorLevel level,
                   const QString &text);
    void SaveError(const QString& tag, ErrorHelperType::ErrorLevel level,
                   const QString& text);
    void SaveAndShowError(const QString& tag, ErrorHelperType::ErrorLevel level,
                          const QString& text);

private:
    ErrorHelper() {}
    ErrorHelper(const ErrorHelper&);
    const ErrorHelper& operator=(const ErrorHelper&);
};

#endif // ERRORHELPER_H
