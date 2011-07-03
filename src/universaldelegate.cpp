#include <QSqlQueryModel>
#include <QSqlField>
#include <QtGui>
#include "UniversalDelegate.h"

UniversalDelegate::UniversalDelegate(Datatable *datatable)
    : QItemDelegate(datatable)
{
    this->datatable = datatable;
}

QWidget *UniversalDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem & ,
                                         const QModelIndex & index ) const
{
    QSqlField* f = datatable->field(index.column());
    QWidget* editor;
    switch(f->type())
    {
    case QVariant::Int:
        editor = new QSpinBox(parent);
        static_cast<QSpinBox*>(editor)->setMaximum(INT_MAX);
        break;
    case QVariant::Double:
        editor = new QDoubleSpinBox(parent);
        static_cast<QDoubleSpinBox*>(editor)->setMaximum(100);
        break;
    case QVariant::ByteArray:
        editor = new QLineEdit(parent);
        break;
    case QVariant::Char:
        editor = new QLineEdit(parent);
        static_cast<QLineEdit*>(editor)->setMaxLength(1);
        break;
    case QVariant::Bool:
        editor = new QCheckBox(parent);
        break;
    case QVariant::DateTime:
        editor = new QDateTimeEdit(parent);
        break;
    case QVariant::Date:
        editor = new QDateEdit(parent);
        static_cast<QDateEdit*>(editor)->setCalendarPopup(true);
        static_cast<QDateEdit*>(editor)->setDisplayFormat("dd.MM.yyyy");
        static_cast<QDateEdit*>(editor)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
        break;
    case QVariant::Time:
        editor = new QTimeEdit(parent);
        break;
    default:
        //String is default
        editor = new QLineEdit(parent);
        if(f->length())
            static_cast<QLineEdit*>(editor)->setMaxLength(f->length());
        break;

    }

    return editor;
}



void UniversalDelegate::setEditorData(QWidget *editor,
                                      const QModelIndex &index) const
{
    QVariant value = index.model()->data(index, Qt::EditRole);
    QSqlField* f = datatable->field(index.column());
    switch(f->type())
    {
    case QVariant::Int:
        static_cast<QSpinBox*>(editor)->setValue(value.toInt());
        break;
    case QVariant::Double:
        static_cast<QDoubleSpinBox*>(editor)->setValue(value.toDouble());
        break;
    case QVariant::Bool:
        static_cast<QCheckBox*>(editor)->setChecked(value.toInt() == 1);
        break;
    case QVariant::DateTime:
        static_cast<QDateTimeEdit*>(editor)->setDateTime(value.toDateTime());
        break;
    case QVariant::Date:
        static_cast<QDateEdit*>(editor)->setDate(value.toDate());
        break;
    case QVariant::Time:
        static_cast<QTimeEdit*>(editor)->setTime(value.toTime());
        break;
    case QVariant::ByteArray:
    case QVariant::Char:
    default:
        static_cast<QLineEdit*>(editor)->setText(value.toString());
    }
}

void UniversalDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    QVariant value;
    QSqlField* f = datatable->field(index.column());
    switch(f->type())
    {
    case QVariant::Int:
        static_cast<QSpinBox*>(editor)->interpretText();
        value = static_cast<QSpinBox*>(editor)->value();
        break;
    case QVariant::Double:
        static_cast<QDoubleSpinBox*>(editor)->interpretText();
        value = static_cast<QDoubleSpinBox*>(editor)->value();
        break;
    case QVariant::Bool:
        value = (static_cast<QCheckBox*>(editor)->checkState() == Qt::Checked);
        break;
    case QVariant::DateTime:
        value = static_cast<QDateTimeEdit*>(editor)->dateTime();
        break;
    case QVariant::Date:
        value = static_cast<QDateEdit*>(editor)->date();
        break;
    case QVariant::Time:
        value = static_cast<QTimeEdit*>(editor)->time();
        break;
    case QVariant::ByteArray:
    case QVariant::Char:
    default:
        value = static_cast<QLineEdit*>(editor)->text();
        break;
    }

    model->setData(index, value, Qt::EditRole);
}

void UniversalDelegate::updateEditorGeometry(QWidget *editor,
                                             const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
