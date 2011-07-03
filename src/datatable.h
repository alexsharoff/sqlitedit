#ifndef CUSTOMERSQUERYMODEL_H
#define CUSTOMERSQUERYMODEL_H


#include <QSqlTableModel>
#include <QSqlRecord>


class Relation;

class Datatable : public QSqlTableModel
{
    Q_OBJECT
public:
    Datatable(const QString& name, QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = 0);
    QVariant data(const QModelIndex &item, int role = 0) const;

    static void setAlias(const QString& fieldName, const QString& alias);
    static QString alias(const QString& fieldName);
    const QString& name();
    QList<Relation*> relations();
    QList<QSqlField*> fields() const;
    QSqlField*  field(const QString& fieldName) const;
    QSqlField*  field(int i) const;
    void setupRelations();
    QSqlRecord newRecord();
    void insertNewRecord(QSqlRecord rec);
    void deleteRow(int row);

private:
    const QString prepareForInsertion(const QModelIndex &index,
                                      const QVariant &value);
    QVariant prepareForDisplaying(const QModelIndex &index,
                                  const QVariant &value) const ;
    QString tableName;
    static QMap<QString, QString> aliases;
    QList<Relation*> _relations;
    QList<QSqlField*> _fields;
    void setFieldType(QSqlField* field, const QString& type);
};

class Relation : QObject
{
public:
    Relation(Datatable* parent, const QString& tableName,
             const QString& fieldFrom, const QString& fieldTo);
    const Datatable*  table();
    const QSqlField*  from();
    const QSqlField*  to();
private:
    Datatable* _parent;
    Datatable* _foreignTable;
    QSqlField* _from;
    QSqlField* _to;

};
#endif // CUSTOMERSQUERYMODEL_H
