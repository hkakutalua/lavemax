#include "datetimehelper.h"



QString DateTimeHelper::GetDescriptiveTextFor(const QDateTime &dateTime)
{
    QDate selectedDate      = dateTime.date();
    QTime selectedTime      = dateTime.time();
    QDate todayDate         = QDate::currentDate();
    QDate tomorrowDate      = todayDate.addDays(1);
    QDate afterTomorrowDate = todayDate.addDays(2);
    QString futureMomentStr;

    // Find out if selected date is today, tomorrow
    // or after tomorrow, and change future moment text according.
    if (selectedDate == todayDate)
        futureMomentStr = "Hoje, ";
    else if (selectedDate == tomorrowDate)
        futureMomentStr = "Amanhã, ";
    else if (selectedDate == afterTomorrowDate)
        futureMomentStr = "Depois de amanhã, ";

    // Descriptive text is formated example:
    //   Hoje, 14 de outubro de 2016, Sexta-Feira.
    //   Às 15h:30m.
    return futureMomentStr +
        selectedDate.toString("dd 'de' MMMM 'de' yyyy") +
        selectedDate.toString("',' dddd'.'") +
        selectedTime.toString(" 'Às' HH'h':mm'm.'");
}
