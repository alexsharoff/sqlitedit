#include <QtSql>
#include <QTableView>
#include "datatable.h"
#include "database.h"


Relation::Relation(Datatable* parent, const QString& tableName,
                   const QString& fieldFrom, const QString& fieldTo)
                       :QObject(parent)
{
    _parent = parent;
    _foreignTable = Database::Table(tableName);
    _from = _foreignTable->field(fieldTo);
    _to = parent->field(fieldFrom);
}

const Datatable*  Relation::table()
{
    return _foreignTable;
}

const QSqlField*  Relation::from()
{
    return _from;
}

const QSqlField*  Relation::to()
{
    return _to;
}


QMap<QString, QString> Datatable::aliases;

const QString& Datatable::name()
{
    return tableName;
}

Datatable::Datatable(const QString& name, QObject *parent)
    : QSqlTableModel(parent)
{
    tableName = name;
    this->setQuery("select * from " + tableName);
    this->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ///TODO: exceptions in constr is bad practice. Fix it
    if(this->lastError().isValid())
    {
        qDebug() << "unable to establish connection - " + tableName + ", " +
                this->lastError().text();
    }
    else
    {

        ///TODO: set up actual field types, relations
        QSqlQuery query;

        if(!query.exec("PRAGMA table_info("+ name +")"))
        {
            qDebug() << query.lastQuery();
            qDebug() << query.lastError();
        }
        else
        {
            _fields.clear();
            int indexName = query.record().indexOf("name");
            int indexType = query.record().indexOf("type");
            int indexNotnull = query.record().indexOf("notnull");
            int indexDefault = query.record().indexOf("dflt_value");
            int indexPrimary = query.record().indexOf("pk");
            while (query.next())
            {
                QSqlField* field =  new QSqlField(
                        query.value(indexName).toString(),
                        QVariant::Invalid);
                setFieldType(field,
                             query.value(indexType).toString());
                field->setRequired(query.value(indexNotnull).toBool());

                //CURRENT_TIME, CURRENT_DATE or CURRENT_TIMESTAMP
                //^default for dates


                if( query.value(indexDefault).toString().
                    compare( "CURRENT_TIME",
                             Qt::CaseInsensitive) == 0 )
                {
                    field->setDefaultValue(
                            QTime::currentTime());
                }
                else if( query.value(indexDefault).toString().
                         compare( "CURRENT_DATE",
                                  Qt::CaseInsensitive) == 0 )
                {
                    field->setDefaultValue(
                            QDate::currentDate());
                }
                else if( query.value(indexDefault).toString().
                         compare( "CURRENT_TIMESTAMP",
                                  Qt::CaseInsensitive) == 0 )
                {
                    field->setDefaultValue(
                            QDateTime::currentDateTime());
                }
                else
                {
                    field->setDefaultValue(query.value(indexDefault));
                }

                field->setAutoValue(query.value(indexPrimary).toBool());
                _fields.append(field);
            }
        }
        for(int i = 0; i < record().count(); i++)
        {
            setHeaderData(i,Qt::Horizontal, alias(record().fieldName(i)));
        }
    }
}



void Datatable::setupRelations()
{
    QSqlQuery query;
    if(!query.exec("PRAGMA foreign_key_list("+ tableName +")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }
    else
    {
        _relations.clear();
        int indexTable = query.record().indexOf("table");
        int indexFrom = query.record().indexOf("from");
        int indexTo = query.record().indexOf("to");
        while (query.next())
        {
            _relations.append(
                    new Relation(this,
                                 query.value(indexTable).toString(),
                                 query.value(indexFrom).toString(),
                                 query.value(indexTo).toString())
                    );
        }
    }
}

void Datatable::setFieldType(QSqlField* field, const QString& type)
{
    if(type.compare( "real", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::Double);
        return;
    }
    if(type.compare( "decimal", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::Double);
        return;
    }
    if(type.compare( "integer", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::Int);
        return;
    }
    if(type.compare( "blob", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::ByteArray);
        return;
    }
    QRegExp exp("varchar\\((\\d+)\\)", Qt::CaseInsensitive);
    if(exp.indexIn(type, 0) != -1)
    {
        int len = exp.cap(1).toInt();
        field->setType(QVariant::String);
        field->setLength(len);
        return;
    }
    if(type.compare( "char", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::Char);
        return;
    }
    //not officially supported by sqlite
    //but really needed
    if(type.compare( "enum", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::String);
        return;
    }
    //not officially supported by sqlite
    //but really needed
    if(type.compare( "boolean", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::Bool);
        return;
    }
    if(type.compare( "datetime", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::DateTime);
        return;
    }
    if(type.compare( "date", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::Date);
        return;
    }
    if(type.compare( "time", Qt::CaseInsensitive) == 0)
    {
        field->setType(QVariant::Time);
        return;
    }

    field->setType(QVariant::String);
}

QList<Relation*> Datatable::relations()
{
    return _relations;
}

QList<QSqlField*> Datatable::fields() const
{
    return _fields;
}

QSqlField*  Datatable::field(const QString& fieldName) const
{
    foreach(QSqlField* f, _fields)
    {
        if(f->name() == fieldName)
            return f;
    }
    return 0;
}

QSqlField*  Datatable::field(int i) const
{
    return _fields[i];
}

void Datatable::setAlias(const QString& fieldName, const QString& alias)
{
    aliases.insert(fieldName,alias);
}

QString Datatable::alias(const QString& fieldName)
{
    return aliases.value(fieldName,fieldName);
}

QVariant Datatable::data(const QModelIndex &index, int role) const
{
    QVariant value;

    value = QSqlTableModel::data(index, role);

    if (role == Qt::DisplayRole) {
        value = prepareForDisplaying(index, value);
    }

    /*if (role == Qt::TextColorRole && index.column() == 1)
        return qVariantFromValue(QColor(Qt::blue));*/
    return value;

}

Qt::ItemFlags Datatable::flags(
        const QModelIndex &index) const
{
    ///TODO: add some functionality there
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}

QVariant Datatable::prepareForDisplaying(const QModelIndex &index,
                                         const QVariant &value) const
{
    QSqlField* f = this->field(index.column());
    QVariant displaying;
    switch(f->type())
    {
    case QVariant::Int:
        displaying = value.toInt();
        break;
    case QVariant::Double:
        displaying = value.toDouble();
        break;
    case QVariant::ByteArray:
        displaying = value.toByteArray();
        break;
    case QVariant::Char:
        displaying = value.toChar();
        break;
    case QVariant::Bool:
        displaying = value.toBool();
        break;
    case QVariant::DateTime:
        displaying = value.toDateTime();
        break;
    case QVariant::Date:
        displaying = value.toDate();
        break;
    case QVariant::Time:
        displaying = value.toTime();
        break;
    default:
        displaying = value.toString();
        break;
    }

    return displaying;
}

const QString Datatable::prepareForInsertion(const QModelIndex &index,
                                             const QVariant &value)
{
    QSqlField* f = this->field(index.column());
    QString str;
    QDateTime* dt;
    QDate* d;
    QTime* t;
    switch(f->type())
    {

    case QVariant::Bool:
        if(value.toBool())
            str = "1";
        else
            str = "0";
        break;
    case QVariant::DateTime:
        dt = new QDateTime(value.toDateTime());
        str = dt->toString("yyyy-MM-dd HH:mm:ss");
        break;
    case QVariant::Date:
        d = new QDate(value.toDate());
        str = d->toString("yyyy-MM-dd");
        break;
    case QVariant::Time:
        t = new QTime(value.toTime());
        str = t->toString("HH:mm:ss");
        break;
    case QVariant::Char:
    case QVariant::ByteArray:
    case QVariant::Double:
    case QVariant::Int:
    default:
        str = value.toString();
        break;
    }
    
    return str;
}
QSqlRecord Datatable::newRecord()
{
    QSqlRecord rec = this->record();

    for(int i = 0 ; i < _fields.count(); i++)
    {
        rec.setValue(i, _fields.at(i)->defaultValue());
    }

    return rec;
}



void Datatable::insertNewRecord(QSqlRecord rec)
{
    //insertRow(rowCount());
    insertRecord(rowCount(), rec);

    //submitNewRow();
}

void Datatable::deleteRow(int row)
{
    QSqlQuery query;
    query.prepare("delete from " + tableName +  " where id=?");
    query.addBindValue(data(this->index(row, 0)).toInt());
    if(!query.exec())
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }
    removeRows(row, 1);
    submitAll();
}

bool Datatable::setData(const QModelIndex &index, const QVariant &value,
                        int  role )
{
    QSqlTableModel::setData(index, value, role);
    QSqlTableModel::setData(index, value, role);
    QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
    QVariant tmp = data(primaryKeyIndex, Qt::EditRole);
    int id = tmp.toInt();

    QSqlQuery query;
    query.prepare("update " + tableName +  " set " +
                  record().fieldName(index.column())
                  + QString("= ? where id = ?"));
    query.addBindValue(prepareForInsertion(index, value));
    query.addBindValue(id);
    if(!query.exec())
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }

    return true;
}
