#ifndef DETAILSWINDOW_H
#define DETAILSWINDOW_H

#include <QDialog>
#include<QtSql>
#include <QtCore>
#include "questions_answers.h"
#include <QNetworkAccessManager>

namespace Ui {
class DetailsWindow;
}

class DetailsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DetailsWindow(QWidget *parent = nullptr);
    ~DetailsWindow();

    void setTestId(int testid);
    void freeResources();

private slots:
    void on_indexList_currentRowChanged(int currentRow);
    void on_previousButton_clicked();
    void on_nextButton_clicked();

private:
    Ui::DetailsWindow *ui;
    int currentTestId;
    QNetworkAccessManager networkManager;
    std::vector<Question> questions;
    Question *selectedQuestion{};

    QByteArray downloadImage(QString url);
    void fetchAndCacheTestData();
    void createLabelsForMainArea();
    void createQuestionsForIndexList();
    void setNavButtonsStatus();
};

#endif // DETAILSWINDOW_H
