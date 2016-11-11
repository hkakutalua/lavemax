#ifndef SPLASHEXCEPTION_H
#define SPLASHEXCEPTION_H

/*
 * Author: Henrick Pedro Kakutalwa
 * Date: 14/03/2016
 * -------------------------------------------------------------------------------------
 * SplashException is the standard exception class for this software
 * You can specifie the reason of the exception like in the std::exception
 * But in addition, you can also specifie a tag(is somekind a marker, see as a title of the
 * exception), and an ErrorLevel (see ErrorHelper class for details about ErrorLevel)
 * -------------------------------------------------------------------------------------
 *
 * Modified: 27/08/2016
 * -- Naming convention changed all to CamelCase
 * -- This class can handle now QString, and uses QString internally
*/

#include "../helpers/errorhelper.h"
#include "exception"
#include "string"

#include <QString>

class SplashException : public std::exception
{
public:
    SplashException(QString what, QString tag = "",
                    ErrorHelperType::ErrorLevel level = ErrorHelperType::WarningError)
    {
        this->w   = what;
        this->tag    = tag;
        this->level  = level;
    }

    QString What() const throw()
    {
        return w;
    }

	QString Tag() const throw()
    {
        return tag;
    }

    ErrorHelperType::ErrorLevel Level() const throw()
    {
        return level;
    }

    virtual const char *what() const throw() override
    {
        return w.toStdString().c_str();
    }

private:
    QString w;
    QString tag;
    ErrorHelperType::ErrorLevel level;
};

#endif // SPLASHEXCEPTION_H
