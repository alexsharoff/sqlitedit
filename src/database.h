#ifndef DATABASE_H
#define DATABASE_H

#include "Datatable.h"

#include <QAbstractItemView>
#include <QList>

class  Database
{
public:
    static void BindModelView(const QString& tableName,
                              QAbstractItemView* view);
    static bool Connect(const QString& filePath);
    static const QList<Datatable*> Tables();
    static Datatable* Table(const QString& name);
private:
    static QList<Datatable*> tables;
    static QObject *connectionSemaphore;
};

#endif // DATABASE_H
