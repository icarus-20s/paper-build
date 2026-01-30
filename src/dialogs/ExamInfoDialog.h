#pragma once

#include "../models/Exam.h"
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

/**
 * ExamInfoDialog: A dialog for editing exam information.
 */
class ExamInfoDialog : public QDialog {
  Q_OBJECT
public:
  explicit ExamInfoDialog(QWidget *parent = nullptr);
  ~ExamInfoDialog() override;

  void setExam(const Exam &exam);
  Exam getExam() const;

private:
  QLineEdit *titleEdit;
  QLineEdit *subjectEdit;
  QLineEdit *durationEdit;
  QSpinBox *totalMarksSpin;
  QSpinBox *passMarksSpin;
  QLineEdit *classEdit;
  QFontComboBox *fontBox;
  QSpinBox *sizeSpin;
  QCheckBox *landscapeCheckBox;
};