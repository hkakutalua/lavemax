#include "usersessionhelper.h"

using namespace UserSessionType;
using namespace DatabaseType;
using namespace ErrorHelperType;

bool UserSessionHelper::LoginUser(const QString &login, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT UserAccountId, Name, Login,"
                  "       Password, IsActive, PrivilegeLevel"
                  " FROM UserAccount"
                  " WHERE Login = :Login AND Password = :Password");
    query.bindValue(":Login", login);
    query.bindValue(":Password", password);

    if (!query.exec()) {
        throw SplashException(query.lastError().text(),
                              _classTag, CriticalError);
    }

    if (!query.next())
        return false;

    _userAccount.Id       = query.value(0).toUInt();
    _userAccount.Name     = query.value(1).toString();
    _userAccount.Login    = query.value(2).toString();
    _userAccount.Password = query.value(3).toString();
    _userAccount.IsActive = query.value(4).toBool();

    const QString privilegeLevelTxt = query.value(5).toUInt();
    if (privilegeLevelTxt == "admin")
        _userAccount.Level = Administrator;
    else if (privilegeLevelTxt == "manager")
        _userAccount.Level = Manager;
    else if(privilegeLevelTxt == "user")
        _userAccount.Level = User;

    _loginState = UserLogged;
    return true;
}

void UserSessionHelper::LogoffUser()
{
    if (_loginState == UserLogged) {
        _userAccount = UserAccount();
        _loginState  = UserNotLogged;
    }
}

UserAccount UserSessionHelper::GetLoggedUser() const
{
    return _userAccount;
}

LoginState UserSessionHelper::GetLoginState() const
{
    return _loginState;
}
