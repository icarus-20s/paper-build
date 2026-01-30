#pragma once

#include "Question.h"

// Placeholder for MCQ type - can be extended later.
class MCQ : public Question
{
public:
    QVector<QString> options;
    int correctIndex = -1;
};
