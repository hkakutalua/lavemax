#ifndef SELECTDATETIMEDIALOG_H
#define SELECTDATETIMEDIALOG_H

#include <QDialog>
#include "../Helpers/datetimehelper.h"

namespace Ui {
class SelectDateTimeDialog;
}

class SelectDateTimeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDateTimeDialog(QWidget *parent = 0);
    ~SelectDateTimeDialog();

signals:
    void DateTimeSelectionDone(const QDateTime &dateTime,
                       QString descriptiveDateText);

private slots:
    void changeDescriptiveDateLabel();
    void selectDateTimeAndQuit();

private:
    QString _descriptiveDateText;
    Ui::SelectDateTimeDialog *ui;
};

#endif // SELECTDATETIMEDIALOG_H
