#include "printreportdialog.h"
#include "ui_printreportdialog.h"

PrintReportDialog::PrintReportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintReportDialog)
{
    ui->setupUi(this);
    ui->EndTimeEdit->setTime(QTime::currentTime());
}

PrintReportDialog::~PrintReportDialog()
{
    delete ui;
}

void PrintReportDialog::printReport()
{

}

void PrintReportDialog::loadReceipts()
{

}

