#ifndef UNIVERSALDELEGATE_H
#define UNIVERSALDELEGATE_H

#include <QItemDelegate>
#include <QtGui>
#include "datatable.h"


//придется хранить указатель на datatable, иначе информацию о полях не получить
//date, integer, time, text, relation(combobox)
class UniversalDelegate : public QItemDelegate
{
public:
    UniversalDelegate(Datatable *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
private:
    Datatable* datatable;
};

#endif // UNIVERSALDELEGATE_H


