#ifndef TABLEVIEWHELPER_H
#define TABLEVIEWHELPER_H

#include <QObject>
#include <QModelIndex>
#include <QTableView>

class TableViewHelper : public QObject
{
    Q_OBJECT
public:
    TableViewHelper(QTableView* parent);
private:
    QTableView* table;
public slots:
    void dataChanged ( const QModelIndex&, const QModelIndex& );
};

#endif // TABLEVIEWHELPER_H
