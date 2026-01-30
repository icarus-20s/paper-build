#pragma once

#include <QDate>
#include <QString>

/**
 * Exam: Stores basic exam metadata.
 */
class Exam {
public:
  QString title;
  QString subject;
  QString duration;
  int totalMarks = 0;
  int passMarks = 0;
  QString className;
  QDate examDate;
  QString term;
  bool isLandscape = false;
};
