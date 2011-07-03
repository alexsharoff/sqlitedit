#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "database.h"

MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pbOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Открыть базу данных SQLite"),
                                                    "C:/",
                                                    tr("Файлы SQLite 3 (*.db *.db3)"));
    if(!fileName.isNull())
    {
        Database::Connect(fileName);
        ui->cbTables->clear();
        foreach(Datatable* table, Database::Tables())
        {
            ui->cbTables->addItem(table->name());
        }
        Database::BindModelView(Database::Tables().at(0)->name(), ui->tableView);
    }
}

void MainWindow::on_cbTables_currentIndexChanged(QString table)
{
    Database::BindModelView(table, ui->tableView);
}
