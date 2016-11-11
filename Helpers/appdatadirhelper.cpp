#include "appdatadirhelper.h"

using namespace ErrorHelperType;

AppDataDirHelper::AppDataDirHelper()
{
    ErrorHelper &error = ErrorHelper::GetInstance();

    // Should return %APPDATA%/<ApplicationName>
    const QString app_data =
        QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
    if (app_data.isEmpty()) {
        error.ShowError("ErrorHelper", CriticalError, "AppData folder was not founded");
        return;
    }

    m_app_data_dir = app_data;
    m_database_dir = app_data + "/database";
    m_logfiles_dir = app_data + "/logfiles";
    m_settings_dir = app_data + "/settings";

    create_app_data_dir();
}

void AppDataDirHelper::create_app_data_dir()
{
    ErrorHelper &error = ErrorHelper::GetInstance();

    QStringList dir_list;
    dir_list << m_app_data_dir
             << m_database_dir
             << m_logfiles_dir
             << m_settings_dir;

    for (int i=0; i<dir_list.size(); i++) {
        QDir dir(dir_list.at(i));

        if (!dir.exists()) {
            if (!dir.mkdir(dir_list.at(i))) {
                error.ShowError("AppDataDirHelper", CriticalError,
                           "Unable to create " + dir.path());
            }
        }
    }
}

QString AppDataDirHelper::get_app_data_dir()
{
    return m_app_data_dir;
}

QString AppDataDirHelper::get_database_dir()
{
    return m_database_dir;
}

QString AppDataDirHelper::get_logfiles_dir()
{
    return m_logfiles_dir;
}

QString AppDataDirHelper::get_settings_dir()
{
    return m_settings_dir;
}
