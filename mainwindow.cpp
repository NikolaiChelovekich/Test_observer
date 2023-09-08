#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utility.h"
#include <memory>

static const char* qry = "SELECT [Id],[Name] + ' | Duration : ' + cast(Duration as nvarchar(50)) + ' min | Created : ' + cast(CreationDate as nvarchar(30)) +"
"' | ' + CASE WHEN IsPublished = 0 THEN 'Not Published' WHEN IsPublished = 1 THEN 'Is Published' END AS IsPublished FROM Tests" ;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainAreaWidget->hide();
    ui->serverUrlLineEdit->setText(".\\sqlexpress");
    ui->dbNameLineEdit->setText("testdb4");
    ui->downloadLabel->hide();

    this->setWindowTitle("LeverX Observer");
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_RefreshButton_clicked()
{
    model->setQuery(qry);
    ui->DetailsButton->setEnabled(false);
}


void MainWindow::on_DetailsButton_clicked()
{
    ui->downloadLabel->show();
    QModelIndex idx = ui->listView->currentIndex();
    if(idx.row() < 0)return;
    detailsWindow.setTestId(model->record(idx.row()).value("Id").toInt());
    detailsWindow.show();
    ui->downloadLabel->hide();
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QModelIndex idx = ui->listView->currentIndex();
    if(idx.row() >= 0)
    {
        ui->DetailsButton->setEnabled(true);
    }
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    ui->downloadLabel->show();
    QModelIndex idx = ui->listView->currentIndex();
    if(idx.row() < 0)return;
    detailsWindow.setTestId(model->record(idx.row()).value("Id").toInt());
    detailsWindow.show();
    ui->downloadLabel->hide();
}

bool MainWindow::connect(QString dbUrl, QString dbName, QString userName, QString password)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(QString("DRIVER={SQL SERVER};Server=%1;Database=%2;").arg(dbUrl, dbName));
    db.setUserName(userName);
    db.setPassword(password);

    return !warnIf(!db.open(),"Unable to access the database" + db.lastError().text());
}

void MainWindow::on_connectButton_clicked()
{
    if(!connect(ui->serverUrlLineEdit->text(),ui->dbNameLineEdit->text(),ui->userNameLineEdit->text(),ui->passwordLineEdit->text()))
        return;

    model = std::make_unique<QSqlQueryModel>();
    model->setQuery(qry);
    ui->listView->setModel(model.get());

    panicIf(model->lastError().isValid(),QString("Failed to make a query to the database: \n") + model->lastError().text());

    ui->listView->setModelColumn(1);
    ui->DetailsButton->setEnabled(false);

    ui->loginAreaWidget->hide();
    ui->mainAreaWidget->show();
}

void MainWindow::on_disconnectButton_clicked()
{
    detailsWindow.freeResources();
    detailsWindow.close();

    ui->passwordLineEdit->clear();
    ui->loginAreaWidget->show();
    ui->mainAreaWidget->hide();
}

