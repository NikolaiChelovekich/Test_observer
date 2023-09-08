#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include <QtCore>
#include <detailswindow.h>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_RefreshButton_clicked();
    void on_DetailsButton_clicked();
    void on_listView_clicked(const QModelIndex &index);
    void on_listView_doubleClicked(const QModelIndex &index);
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<QSqlQueryModel> model;
    DetailsWindow detailsWindow;

    bool connect(QString dbUrl, QString dbName, QString userName, QString password);
};
#endif // MAINWINDOW_H
