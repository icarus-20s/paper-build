#pragma once

#include <QString>
#include <QVector>

enum class QuestionType { Regular, Or, Mcq, Mixed };

/**
 * Question: Stores question text, optional diagram path and optional table.
 */
class Question
{
public:
    QuestionType type = QuestionType::Regular;
    QString text; // HTML for formatting
    QString diagramPath;
    QVector<QVector<QString>> table;
    QVector<Question> subQuestions; // For OR questions
    QVector<QString> options; // For MCQ
    int correctIndex = -1; // For MCQ
};
