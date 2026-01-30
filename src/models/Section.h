#pragma once

#include <QString>
#include <QVector>
#include "Question.h"

/**
 * Section: Stores label, subtitle and list of questions.
 */
class Section
{
public:
    QString label;
    QString subtitle;
    QVector<Question> questions;
};
