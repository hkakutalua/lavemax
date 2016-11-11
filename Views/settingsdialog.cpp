#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    loadSettings();

    connect(ui->SaveButton, SIGNAL(clicked()),
            this, SLOT(saveSettings()));
    connect(ui->SaveButton, SIGNAL(clicked()),
            this, SLOT(accept()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    ui->MornShiftBeginTime->setTime(settings.value("shifts/mornBeginShift").toTime());
    ui->MornShiftEndTime->setTime(settings.value("shifts/mornEndShift").toTime());

    ui->AftShiftBeginTime->setTime(settings.value("shifts/aftBeginShift").toTime());
    ui->AftShiftEndTime->setTime(settings.value("shifts/aftEndShift").toTime());

    ui->NightShiftBeginTime->setTime(settings.value("shifts/nightBeginShift").toTime());
    ui->NightShiftEndTime->setTime(settings.value("shifts/nightEndShift").toTime());
}

void SettingsDialog::saveSettings()
{
    QSettings settings;
    settings.setValue("shifts/mornBeginShift", ui->MornShiftBeginTime->time());
    settings.setValue("shifts/mornEndShift", ui->MornShiftEndTime->time());

    settings.setValue("shifts/aftBeginShift", ui->AftShiftBeginTime->time());
    settings.setValue("shifts/aftEndShift", ui->AftShiftEndTime->time());

    settings.setValue("shifts/nightBeginShift", ui->NightShiftBeginTime->time());
    settings.setValue("shifts/nightEndShift", ui->NightShiftEndTime->time());

    settings.sync();
}
