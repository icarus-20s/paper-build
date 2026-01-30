#include "ExamInfoDialog.h"

ExamInfoDialog::ExamInfoDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle("Edit Exam Information");
  QVBoxLayout *layout = new QVBoxLayout(this);

  // Title
  QLabel *titleLabel = new QLabel("Exam Title:");
  titleEdit = new QLineEdit;
  layout->addWidget(titleLabel);
  layout->addWidget(titleEdit);

  // Class
  QLabel *classLabel = new QLabel("Class:");
  classEdit = new QLineEdit;
  layout->addWidget(classLabel);
  layout->addWidget(classEdit);

  // Font controls
  QHBoxLayout *fontLayout = new QHBoxLayout;
  QLabel *fontLabel = new QLabel("Font:");
  fontBox = new QFontComboBox;
  fontBox->setCurrentFont(QFont("Times New Roman"));
  QLabel *sizeLabel = new QLabel("Size:");
  sizeSpin = new QSpinBox;
  sizeSpin->setValue(12);
  sizeSpin->setRange(8, 24);
  fontLayout->addWidget(fontLabel);
  fontLayout->addWidget(fontBox);
  fontLayout->addWidget(sizeLabel);
  fontLayout->addWidget(sizeSpin);
  fontLayout->addStretch();
  layout->addLayout(fontLayout);

  // Bottom line
  QHBoxLayout *bottomLayout = new QHBoxLayout;
  QLabel *durationLabel = new QLabel("Duration:");
  durationEdit = new QLineEdit;
  QLabel *subjectLabel = new QLabel("Subject:");
  subjectEdit = new QLineEdit;
  QLabel *marksLabel = new QLabel("Total Marks:");
  totalMarksSpin = new QSpinBox;
  totalMarksSpin->setRange(0, 999);
  QLabel *passLabel = new QLabel("Pass Marks:");
  passMarksSpin = new QSpinBox;
  passMarksSpin->setRange(0, 999);
  bottomLayout->addWidget(durationLabel);
  bottomLayout->addWidget(durationEdit);
  bottomLayout->addStretch();
  bottomLayout->addWidget(subjectLabel);
  bottomLayout->addWidget(subjectEdit);
  bottomLayout->addStretch();
  bottomLayout->addWidget(marksLabel);
  bottomLayout->addWidget(totalMarksSpin);
  bottomLayout->addWidget(passLabel);
  bottomLayout->addWidget(passMarksSpin);
  layout->addLayout(bottomLayout);

  // Orientation
  landscapeCheckBox = new QCheckBox("Landscape Orientation");
  layout->addWidget(landscapeCheckBox);

  // Buttons
  QDialogButtonBox *buttonBox =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  layout->addWidget(buttonBox);
}

ExamInfoDialog::~ExamInfoDialog() {}

void ExamInfoDialog::setExam(const Exam &exam) {
  titleEdit->setText(exam.title);
  subjectEdit->setText(exam.subject);
  durationEdit->setText(exam.duration);
  totalMarksSpin->setValue(exam.totalMarks);
  passMarksSpin->setValue(exam.passMarks);
  classEdit->setText(exam.className);
  landscapeCheckBox->setChecked(exam.isLandscape);
}

Exam ExamInfoDialog::getExam() const {
  Exam exam;
  exam.title = titleEdit->text();
  exam.subject = subjectEdit->text();
  exam.duration = durationEdit->text();
  exam.totalMarks = totalMarksSpin->value();
  exam.passMarks = passMarksSpin->value();
  exam.className = classEdit->text();
  exam.isLandscape = landscapeCheckBox->isChecked();
  return exam;
}