#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include "database.h"
#include "universaldelegate.h"



QList<Datatable*> Database::tables;
QObject* Database::connectionSemaphore = 0;

Datatable* Database::Table(const QString& name)
{
    foreach(Datatable* dt, tables)
    {
        if(dt->name() == name)
        {
            return dt;
        }
    }
    return 0;
}

//использовать QSortFilterProxyModel для сортировки и фильтров
void Database::BindModelView(const QString& tableName, QAbstractItemView* view)
{
    view->setModel(Table(tableName));
    view->setItemDelegate( new UniversalDelegate(Table(tableName)) );

}
bool Database::Connect(const QString& filePath)
{
    if(connectionSemaphore != 0)
        delete connectionSemaphore;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath);

    if (!db.open()) {
        qDebug() << "Cannot open database " << filePath << ":"
                << db.lastError().text();
        return false;
    }


    connectionSemaphore = new QObject();
    QSqlQuery query;
    if(!query.exec("SELECT name FROM sqlite_master "
                   "WHERE type IN ('table','view') "
                   "AND name NOT LIKE 'sqlite_%' "
                   "UNION ALL "
                   "SELECT name FROM sqlite_temp_master "
                   "WHERE type IN ('table','view') "
                   "ORDER BY 1"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }
    tables.clear();
    while (query.next())
    {
        tables.append(new Datatable(query.value(0).toString(),
                                    connectionSemaphore));
    }
    foreach(Datatable* dt, tables)
    {
        dt->setupRelations();
    }
    return true;
}
const QList<Datatable*> Database::Tables()
{
    return tables;
}
