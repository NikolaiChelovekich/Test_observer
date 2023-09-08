#include "detailswindow.h"
#include "ui_detailswindow.h"
#include "utility.h"
#include <memory>
#include <QLabel>
#include <QBoxLayout>
#include <QNetworkReply>


DetailsWindow::DetailsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailsWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Details");
}

DetailsWindow::~DetailsWindow()
{
    delete ui;
}

void DetailsWindow::setTestId(int testid)
{
    currentTestId = testid;
    fetchAndCacheTestData();
    if(warnIf(questions.size() == 0,"Selected Test doesnt have any questions")) return;

    selectedQuestion = &questions.front();
    createQuestionsForIndexList();
    setNavButtonsStatus();
    createLabelsForMainArea();
}

void DetailsWindow::freeResources()
{
    questions.clear();
    selectedQuestion = nullptr;
}

void DetailsWindow::fetchAndCacheTestData()
{
    QSqlQuery queryQuestions(QString("SELECT * FROM Questions WHERE [TestId] = %1").arg(currentTestId)); //Creating query which extracts questions from db

    questions.clear();
    panicIf(queryQuestions.lastError().isValid(),QString("Failed to make a query to the database: \n") + queryQuestions.lastError().text());

    while(queryQuestions.next()) //extracting Questions to questions[]
    {
        int id = queryQuestions.value("Id").toInt();
        int testId = queryQuestions.value("TestId").toInt();
        QString text = queryQuestions.value("Text").toString();
        QString imagePath = queryQuestions.value("ImagePath").toString();
        QByteArray downloadedImage = imagePath.isEmpty() ? QByteArray() : downloadImage(imagePath);

        questions.push_back({id,testId,text,imagePath,{},std::move(downloadedImage)});
    }

    QString queryStringAnswers  ("SELECT * FROM Answers WHERE QuestionId IN (");

     for(size_t i = 0; i < questions.size(); i++)
    {
        queryStringAnswers.append((std::to_string(questions.at(i).id) + ",").c_str());
    }

    queryStringAnswers.erase(queryStringAnswers.end()-1);
    queryStringAnswers.append(")");

    QSqlQuery queryAnswers(queryStringAnswers); //Creating query which extracts answers from db

    panicIf(queryAnswers.lastError().isValid(),QString("Failed to make a query to the database: \n") + queryAnswers.lastError().text());


    while(queryAnswers.next()) //extracting Answers to questions[].answers
    {
        int id = queryAnswers.value("Id").toInt();
        int questionId = queryAnswers.value("QuestionId").toInt();
        QString text = queryAnswers.value("Text").toString();
        bool isCorrect = queryAnswers.value("IsCorrect").toBool();

        auto itr = std::find_if(questions.begin(),questions.end(),[questionId](const Question  &question){return question.id == questionId;});

        if(itr == questions.end())
        {
            qDebug() << "answer without question, continuing";
            continue;
        }
        itr->answers.push_back({id,questionId,text,isCorrect});
    }
}

void DetailsWindow::createLabelsForMainArea()
{
    auto layout = qobject_cast<QBoxLayout*>(ui->mainArea->layout());
    QList<QLabel*> labels =ui->mainArea->findChildren<QLabel*>();
    qDeleteAll(labels);

    QLabel *description = new QLabel();

    description->setText(selectedQuestion->text);
    description->setStyleSheet("font: 16pt;");
    description->setAlignment(Qt::AlignTop);
    description->setWordWrap(true);

    layout->insertWidget(layout->count()-2,description);

    if(!selectedQuestion->imageData.isEmpty())
    {
        QLabel *image = new QLabel();
        QPixmap pixMap;
        image->setAlignment(Qt::AlignCenter);
        image->setMargin(10);
        pixMap.loadFromData(selectedQuestion->imageData);
        image->setPixmap(pixMap.scaled(400,400,Qt::KeepAspectRatio, Qt::SmoothTransformation));
        layout->insertWidget(layout->count()-1,image);
    }

    for(size_t i = 0; i < selectedQuestion->answers.size();i++)
    {
        QLabel *answer = new QLabel();

        answer->setText((std::to_string(i+1) + ". ").c_str() + selectedQuestion->answers[i].text);
        answer->setWordWrap(true);


        if(selectedQuestion->answers[i].isCorrect)
        {
            answer->setStyleSheet("font: 12pt; background-color: rgb(29, 219, 15); color: white ; border-bottom : 1px solid grey; padding: 5px; ");
        }
        else
        {
            answer->setStyleSheet("font: 12pt ; background-color: rgb(220,220,220) ; border-bottom : 1px solid grey; padding: 5px;");
        }
        answer->setAlignment(Qt::AlignTop);
        layout->insertWidget(layout->count()-1,answer);

    }
    QLabel* lastLabel =  qobject_cast<QLabel*>(ui->mainArea->children().last());

    lastLabel->setStyleSheet(lastLabel->styleSheet() + "border: none;");
}

void DetailsWindow::createQuestionsForIndexList()
{
    ui->indexList->clear();

    for(size_t i = 0; i < questions.size();i++)
    {
        ui->indexList->addItem(QString("Question ") + (std::to_string(i+1)).c_str());

    }
    ui->indexList->blockSignals(true);
    ui->indexList->setCurrentRow(0);
    ui->indexList->blockSignals(false);
}

void DetailsWindow::setNavButtonsStatus()
{
    int currentRow = ui->indexList->currentIndex().row();
    bool moreThanOneQuestions = questions.size() > 1;
    ui->nextButton->setEnabled((currentRow < questions.size() - 1) && moreThanOneQuestions);
    ui->previousButton->setEnabled(currentRow > 0 && moreThanOneQuestions);
}

QByteArray DetailsWindow::downloadImage(QString urlArg)
{
    const QUrl url = QUrl(urlArg);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager.get(request);
    QEventLoop eventLoop;
    connect(reply,&QNetworkReply::finished,&eventLoop,&QEventLoop::quit);
    eventLoop.exec();

    return warnIf(reply->error(),reply->errorString()) ? QByteArray() : reply->readAll();
}

void DetailsWindow::on_indexList_currentRowChanged(int currentRow)
{
    if(currentRow < 0 || (!selectedQuestion && selectedQuestion->id == questions[currentRow].id))
    {
        return;
    }
    selectedQuestion = &questions[currentRow];
    createLabelsForMainArea();

    setNavButtonsStatus();
}

void DetailsWindow::on_previousButton_clicked()
{
   ui->indexList->setCurrentRow((ui->indexList->currentRow()) - 1);
}

void DetailsWindow::on_nextButton_clicked()
{
   ui->indexList->setCurrentRow((ui->indexList->currentRow()) + 1);
}

