#ifndef DATABASEFACTORY_H
#define DATABASEFACTORY_H

#include <QString>
#include <QSqlDatabase>

class DatabaseFactory
{
private:
    static constexpr const   char connectionString[] =
            "DRIVER={SQL Server Native Client 11.0};"
            "SERVER=(localdb)\\ProjectsV13;"
            "DATABASE=LICENTA;"
            "Trusted_Connection=Yes;";

    static constexpr const char driver[] = "QODBC";

    static QSqlDatabase databaseInstance;

public:

    static QSqlDatabase getConnection();
};

#endif // DATABASEFACTORY_H
