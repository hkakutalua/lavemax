#include "selectdatetimedialog.h"
#include "ui_selectdatetimedialog.h"

SelectDateTimeDialog::SelectDateTimeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectDateTimeDialog)
{
    ui->setupUi(this);

    QDate todayDate = QDate::currentDate();

    // Date range for calendar widget will be set from today
    // to 30 days later
    // Time range for QTimeEdit will be set from current hour
    // to the last hour of the day
    ui->calendarWidget->setMinimumDate(todayDate);
    ui->calendarWidget->setMaximumDate(todayDate.addDays(30));

    // Delivery date is always predefined for tomorrow
    QDate deliveryDate = todayDate.addDays(1);
    ui->calendarWidget->setSelectedDate(deliveryDate);

    // Delivery is predefined for the current hour
    ui->timeEdit->setTime(QTime::currentTime());

    changeDescriptiveDateLabel();
    connect(ui->calendarWidget, SIGNAL(clicked(QDate)),
            this, SLOT(changeDescriptiveDateLabel()));
    connect(ui->timeEdit, SIGNAL(timeChanged(QTime)),
            this, SLOT(changeDescriptiveDateLabel()));

    connect(ui->ConfirmButton, SIGNAL(clicked()),
            this, SLOT(selectDateTimeAndQuit()));
    connect(ui->CancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));
}

SelectDateTimeDialog::~SelectDateTimeDialog()
{
    delete ui;
}

void SelectDateTimeDialog::changeDescriptiveDateLabel()
{
    QDateTime dateTime(ui->calendarWidget->selectedDate(),
                       ui->timeEdit->time());
    _descriptiveDateText = DateTimeHelper::GetDescriptiveTextFor(dateTime);
    ui->DescriptiveDateTextLabel->setText(_descriptiveDateText);
}

void SelectDateTimeDialog::selectDateTimeAndQuit()
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time = ui->timeEdit->time();
    emit DateTimeSelectionDone(QDateTime(date,time),
                               _descriptiveDateText);
    done(0);
}
