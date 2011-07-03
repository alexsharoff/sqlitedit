#include "tableviewhelper.h"

TableViewHelper::TableViewHelper(QTableView* parent) : QObject(parent)
{
    table = parent;
}
void TableViewHelper::dataChanged ( const QModelIndex &,
                                    const QModelIndex & )
{
    table->resizeColumnsToContents();
}
