#include "ExamInfoPage.h"
#include "ui_ExamInfoPage.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSpinBox>

/**
 * @file ExamInfoPage.cpp
 * @brief Implementation of the ExamInfoPage class.
 */

// Constants
namespace {
constexpr int MIN_MARKS = 0;
constexpr int MAX_MARKS = 1000;
constexpr int DEFAULT_TOTAL_MARKS = 100;
constexpr int DEFAULT_PASS_MARKS = 40;
constexpr int MAX_TITLE_LENGTH = 200;
constexpr int MAX_SUBJECT_LENGTH = 100;
constexpr int MAX_DURATION_LENGTH = 50;
constexpr int MAX_CLASS_LENGTH = 50;

// Duration patterns: "2 hours", "90 minutes", "1.5 hours", etc.
const QString DURATION_PATTERN =
    R"(^[\d\.]+\s*(hour|hours|hr|hrs|minute|minutes|min|mins)$)";
} // namespace

ExamInfoPage::ExamInfoPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::ExamInfoPage), m_isModified(false) {
  ui->setupUi(this);
  setupUi();
  setupValidators();
  setupConnections();
}

ExamInfoPage::~ExamInfoPage() { delete ui; }

void ExamInfoPage::setupUi() {
  // Configure title field
  ui->titleEdit->setPlaceholderText(tr("e.g., Final Examination 2024"));
  ui->titleEdit->setMaxLength(MAX_TITLE_LENGTH);
  ui->titleEdit->setToolTip(tr("Enter the exam title or name"));

  // Configure subject field
  ui->subjectEdit->setPlaceholderText(tr("e.g., Mathematics, Physics"));
  ui->subjectEdit->setMaxLength(MAX_SUBJECT_LENGTH);
  ui->subjectEdit->setToolTip(tr("Enter the subject name"));

  // Configure duration field
  ui->durationEdit->setPlaceholderText(tr("e.g., 2 hours, 90 minutes"));
  ui->durationEdit->setMaxLength(MAX_DURATION_LENGTH);
  ui->durationEdit->setToolTip(
      tr("Enter the exam duration (e.g., 2 hours, 90 minutes)"));

  // Configure class field
  ui->classEdit->setPlaceholderText(tr("e.g., Grade 10, Class XII"));
  ui->classEdit->setMaxLength(MAX_CLASS_LENGTH);
  ui->classEdit->setToolTip(tr("Enter the class or grade"));

  // Configure marks spin boxes
  ui->totalMarksSpin->setRange(MIN_MARKS, MAX_MARKS);
  ui->totalMarksSpin->setValue(DEFAULT_TOTAL_MARKS);
  ui->totalMarksSpin->setSuffix(tr(" marks"));
  ui->totalMarksSpin->setToolTip(tr("Total marks for the exam"));

  ui->passMarksSpin->setRange(MIN_MARKS, MAX_MARKS);
  ui->passMarksSpin->setValue(DEFAULT_PASS_MARKS);
  ui->passMarksSpin->setSuffix(tr(" marks"));
  ui->passMarksSpin->setToolTip(tr("Minimum marks required to pass"));

  // Configure exam date
  ui->examDateEdit->setCalendarPopup(true);
  ui->examDateEdit->setDate(QDate::currentDate());
  ui->examDateEdit->setToolTip(tr("Date of the examination"));

  // Configure term/semester field
  ui->termEdit->setPlaceholderText(tr("e.g., First Term, Semester 1"));
  ui->termEdit->setToolTip(tr("Academic term or semester"));

  // Style the next button
  ui->nextButton->setMinimumHeight(35);
  ui->nextButton->setIcon(QIcon::fromTheme("go-next"));
  ui->nextButton->setDefault(true);
  ui->nextButton->setToolTip(tr("Continue to question editor"));

  // Initially disable next button until form is valid
  updateValidationState();
}

void ExamInfoPage::setupValidators() {
  // Duration validator - allows common time formats
  QRegularExpression durationRegex(DURATION_PATTERN,
                                   QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionValidator *durationValidator =
      new QRegularExpressionValidator(durationRegex, this);
  ui->durationEdit->setValidator(durationValidator);
}

void ExamInfoPage::setupConnections() {
  // Next button
  connect(ui->nextButton, &QPushButton::clicked, this,
          &ExamInfoPage::onNextClicked);

  // Form field changes
  connect(ui->titleEdit, &QLineEdit::textChanged, this,
          &ExamInfoPage::onFieldChanged);
  connect(ui->subjectEdit, &QLineEdit::textChanged, this,
          &ExamInfoPage::onFieldChanged);
  connect(ui->durationEdit, &QLineEdit::textChanged, this,
          &ExamInfoPage::onFieldChanged);
  connect(ui->classEdit, &QLineEdit::textChanged, this,
          &ExamInfoPage::onFieldChanged);
  connect(ui->termEdit, &QLineEdit::textChanged, this,
          &ExamInfoPage::onFieldChanged);

  // Marks changes
  connect(ui->totalMarksSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &ExamInfoPage::onTotalMarksChanged);
  connect(ui->passMarksSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &ExamInfoPage::onPassMarksChanged);

  // Date change
  connect(ui->examDateEdit, &QDateEdit::dateChanged, this,
          &ExamInfoPage::onFieldChanged);

  // Orientation change
  connect(ui->landscapeCheckBox, &QCheckBox::toggled, this,
          &ExamInfoPage::onFieldChanged);
}

Exam ExamInfoPage::getExamInfo() const {
  Exam exam;

  exam.title = ui->titleEdit->text().trimmed();
  exam.subject = ui->subjectEdit->text().trimmed();
  exam.duration = ui->durationEdit->text().trimmed();
  exam.totalMarks = ui->totalMarksSpin->value();
  exam.passMarks = ui->passMarksSpin->value();
  exam.className = ui->classEdit->text().trimmed();
  exam.examDate = ui->examDateEdit->date();
  exam.term = ui->termEdit->text().trimmed();
  exam.isLandscape = ui->landscapeCheckBox->isChecked();

  return exam;
}

void ExamInfoPage::setExamInfo(const Exam &exam) {
  // Block signals to prevent triggering change events
  ui->titleEdit->blockSignals(true);
  ui->subjectEdit->blockSignals(true);
  ui->durationEdit->blockSignals(true);
  ui->classEdit->blockSignals(true);
  ui->termEdit->blockSignals(true);
  ui->totalMarksSpin->blockSignals(true);
  ui->passMarksSpin->blockSignals(true);
  ui->examDateEdit->blockSignals(true);

  // Set values
  ui->titleEdit->setText(exam.title);
  ui->subjectEdit->setText(exam.subject);
  ui->durationEdit->setText(exam.duration);
  ui->classEdit->setText(exam.className);
  ui->termEdit->setText(exam.term);
  ui->totalMarksSpin->setValue(exam.totalMarks);
  ui->passMarksSpin->setValue(exam.passMarks);

  if (exam.examDate.isValid()) {
    ui->examDateEdit->setDate(exam.examDate);
  }

  ui->landscapeCheckBox->setChecked(exam.isLandscape);

  // Unblock signals
  ui->titleEdit->blockSignals(false);
  ui->subjectEdit->blockSignals(false);
  ui->durationEdit->blockSignals(false);
  ui->classEdit->blockSignals(false);
  ui->termEdit->blockSignals(false);
  ui->totalMarksSpin->blockSignals(false);
  ui->passMarksSpin->blockSignals(false);
  ui->examDateEdit->blockSignals(false);

  // Store as original for modification tracking
  m_originalExam = exam;
  m_isModified = false;

  updateValidationState();
}

bool ExamInfoPage::validateForm(QString &errorMessage) const {
  // Validate title
  if (!validateRequiredField(ui->titleEdit->text(), tr("Title"),
                             errorMessage)) {
    return false;
  }

  // Validate subject
  if (!validateRequiredField(ui->subjectEdit->text(), tr("Subject"),
                             errorMessage)) {
    return false;
  }

  // Validate duration
  if (!validateDuration(errorMessage)) {
    return false;
  }

  // Validate class
  if (!validateRequiredField(ui->classEdit->text(), tr("Class"),
                             errorMessage)) {
    return false;
  }

  // Validate marks
  if (!validateMarks(errorMessage)) {
    return false;
  }

  // Validate exam date
  if (!ui->examDateEdit->date().isValid()) {
    errorMessage = tr("Please enter a valid exam date.");
    return false;
  }

  return true;
}

bool ExamInfoPage::validateRequiredField(const QString &text,
                                         const QString &fieldName,
                                         QString &errorMessage) const {
  if (text.trimmed().isEmpty()) {
    errorMessage = tr("%1 is required and cannot be empty.").arg(fieldName);
    return false;
  }

  return true;
}

bool ExamInfoPage::validateMarks(QString &errorMessage) const {
  int totalMarks = ui->totalMarksSpin->value();
  int passMarks = ui->passMarksSpin->value();

  if (totalMarks <= 0) {
    errorMessage = tr("Total marks must be greater than zero.");
    return false;
  }

  if (passMarks < 0) {
    errorMessage = tr("Pass marks cannot be negative.");
    return false;
  }

  if (passMarks > totalMarks) {
    errorMessage = tr("Pass marks (%1) cannot exceed total marks (%2).")
                       .arg(passMarks)
                       .arg(totalMarks);
    return false;
  }

  return true;
}

bool ExamInfoPage::validateDuration(QString &errorMessage) const {
  QString duration = ui->durationEdit->text().trimmed();

  if (duration.isEmpty()) {
    errorMessage = tr("Duration is required.");
    return false;
  }

  // Check if duration matches expected format
  QRegularExpression regex(DURATION_PATTERN,
                           QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatch match = regex.match(duration);

  if (!match.hasMatch()) {
    errorMessage = tr("Duration format is invalid. Use formats like: "
                      "\"2 hours\", \"90 minutes\", \"1.5 hours\"");
    return false;
  }

  return true;
}

bool ExamInfoPage::isModified() const { return m_isModified; }

void ExamInfoPage::clearForm() {
  ui->titleEdit->clear();
  ui->subjectEdit->clear();
  ui->durationEdit->clear();
  ui->classEdit->clear();
  ui->termEdit->clear();
  ui->totalMarksSpin->setValue(DEFAULT_TOTAL_MARKS);
  ui->passMarksSpin->setValue(DEFAULT_PASS_MARKS);
  ui->examDateEdit->setDate(QDate::currentDate());

  m_isModified = false;
  updateValidationState();
}

void ExamInfoPage::setInitialFocus() {
  ui->titleEdit->setFocus();
  ui->titleEdit->selectAll();
}

QString ExamInfoPage::getTitle() const {
  return ui->titleEdit->text().trimmed();
}

QString ExamInfoPage::getSubject() const {
  return ui->subjectEdit->text().trimmed();
}

QString ExamInfoPage::getDuration() const {
  return ui->durationEdit->text().trimmed();
}

int ExamInfoPage::getTotalMarks() const { return ui->totalMarksSpin->value(); }

int ExamInfoPage::getPassMarks() const { return ui->passMarksSpin->value(); }

QString ExamInfoPage::getClassName() const {
  return ui->classEdit->text().trimmed();
}

void ExamInfoPage::setNextButtonEnabled(bool enabled) {
  ui->nextButton->setEnabled(enabled);
}

void ExamInfoPage::onNextClicked() {
  QString errorMessage;
  if (!validateForm(errorMessage)) {
    showValidationError(errorMessage);
    return;
  }

  emit nextClicked();
}

void ExamInfoPage::onFieldChanged() {
  m_isModified = true;
  emit formChanged();
  updateValidationState();
}

void ExamInfoPage::onTotalMarksChanged(int value) {
  // Ensure pass marks don't exceed total marks
  if (ui->passMarksSpin->value() > value) {
    ui->passMarksSpin->blockSignals(true);
    ui->passMarksSpin->setValue(value);
    ui->passMarksSpin->blockSignals(false);
  }

  // Update pass marks maximum
  ui->passMarksSpin->setMaximum(value);

  onFieldChanged();
}

void ExamInfoPage::onPassMarksChanged(int value) {
  Q_UNUSED(value);
  onFieldChanged();
}

void ExamInfoPage::updateValidationState() {
  QString errorMessage;
  bool isValid = validateForm(errorMessage);

  ui->nextButton->setEnabled(isValid);

  // Optional: Show validation icon or message
  if (!isValid && m_isModified) {
    ui->nextButton->setToolTip(errorMessage);
  } else {
    ui->nextButton->setToolTip(tr("Continue to question editor"));
  }

  emit validationChanged(isValid);
}

void ExamInfoPage::showValidationError(const QString &message) {
  QMessageBox::warning(this, tr("Validation Error"), message);
}