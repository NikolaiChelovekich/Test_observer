#ifndef QUESTIONS_ANSWERS_H
#define QUESTIONS_ANSWERS_H
#include<QString>
struct Answer
{
    int id{};
    int questionId{};
    QString text;
    bool isCorrect{};
};

struct Question
{
    int id{};
    int testId{};
    QString text;
    QString imagePath;
    std::vector<Answer> answers;
    QByteArray imageData;
};
#endif // QUESTIONS_ANSWERS_H
