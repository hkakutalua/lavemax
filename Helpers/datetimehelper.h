#ifndef DATETIMEHELPER_H
#define DATETIMEHELPER_H

#include <QDateTime>

class DateTimeHelper
{
public:
    static QString GetDescriptiveTextFor(const QDateTime &dateTime);
};

#endif // DATETIMEHELPER_H
