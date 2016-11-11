#ifndef USERSESSIONHELPER_H
#define USERSESSIONHELPER_H

#include "databasehelper.h"
#include <QSqlQuery>

namespace UserSessionType {
    enum LoginState
    {
        UserLogged,
        UserNotLogged
    };
}

class UserSessionHelper
{
public:
    static UserSessionHelper& GetInstance()
    {
        static UserSessionHelper instance;
        return instance;
    }

    bool LoginUser(const QString& login, const QString& password);
    void LogoffUser();

    DatabaseType::UserAccount   GetLoggedUser() const;
    UserSessionType::LoginState GetLoginState() const;

private:
    UserSessionHelper() {}
    UserSessionHelper(const UserSessionHelper&);
    const UserSessionHelper& operator=(const UserSessionHelper&);

    QString _classTag {"UserSessionHelper"};

    DatabaseType::UserAccount   _userAccount;
    UserSessionType::LoginState _loginState
    {UserSessionType::UserNotLogged};
};

#endif // USERSESSIONHELPER_H
