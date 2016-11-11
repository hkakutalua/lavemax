#include "errorhelper.h"

using namespace ErrorHelperType;

void ErrorHelper::ShowError(const QString &tag, ErrorLevel level, const QString &text)
{
    QMessageBox errorMessage;
    errorMessage.setText(tag + "\n" + text);

    switch (level) {
        case WarningError:
            errorMessage.setWindowTitle("Erro!");
            errorMessage.setIcon(QMessageBox::Warning);
            break;
        case CriticalError:
            errorMessage.setWindowTitle("Erro Fatal!");
            errorMessage.setIcon(QMessageBox::Critical);
            break;
    }

    errorMessage.exec();
}

void ErrorHelper::SaveError(const QString &tag, ErrorLevel level, const QString &text)
{
    QString logfiles_path =
        AppDataDirHelper::get_instance().get_logfiles_dir();

    QString today_date = QDate::currentDate().toString("dd-MM-yyyy");
    QString today_time = QDateTime::currentDateTime().toString("hh:mm:ss");

    qDebug() << logfiles_path;
    QFile log_file(logfiles_path + "/" + today_date + ".log");
    if (!log_file.open(QIODevice::WriteOnly |
                       QIODevice::Append    |
                       QIODevice::Text)) {
        ShowError("ErrorHelper", CriticalError,
                   "Unable to open error log file");
        return;
    }

    QTextStream stream(&log_file);
    stream << today_time << '\n';
    stream << "[" + tag + "]\n";  // [tag]

    switch (level) {
        case WarningError:
            stream << "- Erro:\n";
            break;
        case CriticalError:
            stream << "- Erro Fatal:\n";
    }
    stream << text << "\n\n";
}

void ErrorHelper::SaveAndShowError(const QString &tag, ErrorLevel level, const QString &text)
{
    SaveError(tag,level,text);
    ShowError(tag,level,text);
}
