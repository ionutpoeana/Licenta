#include "databasefactory.h"

constexpr char DatabaseFactory::driver[];
constexpr char DatabaseFactory::connectionString[];

QSqlDatabase DatabaseFactory::getConnection()
{
    QSqlDatabase m_database;
    m_database = QSqlDatabase::addDatabase(driver);

    m_database.setDatabaseName(connectionString);
    return m_database;
}
