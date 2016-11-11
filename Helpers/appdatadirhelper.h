#ifndef APPDATADIRECTORY_H
#define APPDATADIRECTORY_H

/*
 * Author: Henrick Pedro Kakutalwa
 * Date: 17/03/2016
 * Modifcation: 27/08/2016
 * -- ErrorHelper::get_instance is now ErrorHelper::GetInstance()
 * -- ErrorHelper::show_error is now ErrorHelper::ShowError()
 * -------------------------------------------------------------------------------------
 * AppDataDirHelper class retrieves common data directories of the applications.
 * Such directories includes:
 * - The root data directory: %APPDATA%/SplashService
 * - The directory that holds the database file: %APPDATA%/SplashService/database
 * - The directory that holds log files: %APPDATA%/SplashService/logfiles
 * - The directory that holds settings files: %APPDATA%/SplashService/settings
 *
 * -------------------------------------------------------------------------------------
 *
*/

#include <QString>
#include <QDir>
#include <QStandardPaths>
#include "errorhelper.h"

class AppDataDirHelper
{
public:
    static AppDataDirHelper& get_instance()
    {
        static AppDataDirHelper instance;
        return instance;
    }

    void create_app_data_dir();

    QString get_app_data_dir();
    QString get_database_dir();
    QString get_logfiles_dir();
    QString get_settings_dir();

private:
    AppDataDirHelper();
    AppDataDirHelper(const AppDataDirHelper&);
    const AppDataDirHelper& operator=(const AppDataDirHelper&);

    QString m_app_data_dir;
    QString m_database_dir;
    QString m_logfiles_dir;
    QString m_settings_dir;
};

#endif // APPDATADIRECTORY_H
