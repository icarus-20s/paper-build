#include "SectionWidget.h"
#include "../questionWidget/QuestionWidget.h"
#include "ui_SectionWidget.h"
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <algorithm>

/**
 * @file SectionWidget.cpp
 * @brief Implementation of the SectionWidget class.
 */

// Constants
namespace {
const QString DEFAULT_FONT_FAMILY = "Times New Roman";
constexpr int DEFAULT_FONT_SIZE = 12;
constexpr int MIN_QUESTIONS_FOR_SECTION = 1;
} // namespace

SectionWidget::SectionWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::SectionWidget),
      m_defaultFontFamily(DEFAULT_FONT_FAMILY),
      m_defaultFontSize(DEFAULT_FONT_SIZE) {
  ui->setupUi(this);
  setupUi();
  setupConnections();
  addQuestionWidget(); // Start with one question
}

SectionWidget::~SectionWidget() { delete ui; }

void SectionWidget::setupUi() {
  // Configure label input
  ui->labelEdit->setPlaceholderText(tr("e.g., Section A, Part I"));
  ui->labelEdit->setMaxLength(50);

  // Configure subtitle input
  ui->subtitleEdit->setPlaceholderText(
      tr("e.g., Answer all questions, Attempt any two"));
  ui->subtitleEdit->setMaxLength(200);

  // Configure questions container
  ui->questionsLayout->setSpacing(2);
  ui->questionsLayout->setContentsMargins(0, 2, 0, 2);

  // Style the add question button
  ui->addQuestionButton->setIcon(QIcon::fromTheme("list-add"));
  ui->addQuestionButton->setToolTip(tr("Add a new question to this section"));
}

void SectionWidget::setupConnections() {
  // Connect add question button
  connect(ui->addQuestionButton, &QPushButton::clicked, this,
          &SectionWidget::onAddQuestion);

  // Connect section metadata changes
  connect(ui->labelEdit, &QLineEdit::textChanged, this,
          &SectionWidget::onLabelChanged);
  connect(ui->subtitleEdit, &QLineEdit::textChanged, this,
          &SectionWidget::onSubtitleChanged);
}

void SectionWidget::addQuestionWidget() {
  // Collapse existing questions to save space
  QVector<QuestionWidget *> existingWidgets = getQuestionWidgets();
  for (QuestionWidget *widget : existingWidgets) {
    if (widget) {
      widget->setCollapsed(true);
    }
  }

  QuestionWidget *questionWidget = new QuestionWidget(this);
  questionWidget->setDefaultFont(m_defaultFontFamily, m_defaultFontSize);
  // ...

  // Connect question content changes
  connect(questionWidget, &QuestionWidget::contentChanged, this,
          &SectionWidget::onQuestionContentChanged);

  // Connect remove request
  connect(
      questionWidget, &QuestionWidget::removeRequested,
      [this, questionWidget]() { this->removeQuestionWidget(questionWidget); });

  // Add to layout
  ui->questionsLayout->addWidget(questionWidget);

  int newIndex = getQuestionCount() - 1;
  emit questionAdded(newIndex);
  emit questionCountChanged(getQuestionCount());
  emit sectionChanged();
}

void SectionWidget::onAddQuestion() { addQuestionWidget(); }

void SectionWidget::addQuestion() { addQuestionWidget(); }

Section SectionWidget::toSection() const {
  Section section;

  // Export section metadata
  section.label = ui->labelEdit->text().trimmed();
  section.subtitle = ui->subtitleEdit->text().trimmed();

  // Gather all questions from QuestionWidgets
  QVector<QuestionWidget *> questionWidgets = getQuestionWidgets();

  for (QuestionWidget *questionWidget : questionWidgets) {
    if (questionWidget && questionWidget->hasContent()) {
      Question question = questionWidget->toQuestion();
      section.questions.append(question);
    }
  }

  return section;
}

void SectionWidget::fromSection(const Section &section) {
  // Clear existing content
  clearSection();

  // Load section metadata
  ui->labelEdit->setText(section.label);
  ui->subtitleEdit->setText(section.subtitle);

  // Load questions
  for (const Question &question : section.questions) {
    QuestionWidget *questionWidget = new QuestionWidget(this);
    questionWidget->setDefaultFont(m_defaultFontFamily, m_defaultFontSize);
    questionWidget->fromQuestion(question);

    // Connect signals
    connect(questionWidget, &QuestionWidget::contentChanged, this,
            &SectionWidget::onQuestionContentChanged);

    ui->questionsLayout->addWidget(questionWidget);
  }

  // If no questions were loaded, add one empty question
  if (getQuestionCount() == 0) {
    addQuestionWidget();
  }

  emit questionCountChanged(getQuestionCount());
  emit sectionChanged();
}

void SectionWidget::setDefaultFont(const QString &family, int size) {
  m_defaultFontFamily = family;
  m_defaultFontSize = size;

  // Apply to existing question widgets
  QVector<QuestionWidget *> questionWidgets = getQuestionWidgets();
  for (QuestionWidget *widget : questionWidgets) {
    if (widget) {
      widget->setDefaultFont(family, size);
    }
  }
}

int SectionWidget::getQuestionCount() const {
  return getQuestionWidgets().size();
}

QuestionWidget *SectionWidget::getQuestionWidget(int index) const {
  QVector<QuestionWidget *> widgets = getQuestionWidgets();

  if (index >= 0 && index < widgets.size()) {
    return widgets[index];
  }

  return nullptr;
}

QVector<QuestionWidget *> SectionWidget::getQuestionWidgets() const {
  QVector<QuestionWidget *> questionWidgets;

  // Iterate through layout items
  QLayout *layout = ui->questionsLayout;
  if (!layout) {
    return questionWidgets;
  }

  for (int i = 0; i < layout->count(); ++i) {
    QLayoutItem *item = layout->itemAt(i);
    if (item && item->widget()) {
      QuestionWidget *questionWidget =
          qobject_cast<QuestionWidget *>(item->widget());
      if (questionWidget) {
        questionWidgets.append(questionWidget);
      }
    }
  }

  return questionWidgets;
}

bool SectionWidget::hasValidContent() const {
  // Section must have at least one question with content
  QVector<QuestionWidget *> questionWidgets = getQuestionWidgets();

  for (QuestionWidget *widget : questionWidgets) {
    if (widget && widget->hasContent()) {
      return true;
    }
  }

  return false;
}

void SectionWidget::clearSection() {
  // Clear metadata
  ui->labelEdit->clear();
  ui->subtitleEdit->clear();

  // Remove all question widgets
  QVector<QuestionWidget *> questionWidgets = getQuestionWidgets();

  for (QuestionWidget *widget : questionWidgets) {
    if (widget) {
      ui->questionsLayout->removeWidget(widget);
      widget->deleteLater();
    }
  }

  emit questionCountChanged(0);
  emit sectionChanged();
}

void SectionWidget::setSectionLabel(const QString &label) {
  ui->labelEdit->setText(label);
}

QString SectionWidget::getSectionLabel() const { return ui->labelEdit->text(); }

void SectionWidget::setSectionSubtitle(const QString &subtitle) {
  ui->subtitleEdit->setText(subtitle);
}

QString SectionWidget::getSectionSubtitle() const {
  return ui->subtitleEdit->text();
}

bool SectionWidget::removeQuestionAt(int index) {
  QuestionWidget *widget = getQuestionWidget(index);

  if (!widget) {
    return false;
  }

  return removeQuestionWidget(widget);
}

bool SectionWidget::removeQuestionWidget(QuestionWidget *widget) {
  if (!widget) {
    return false;
  }

  // Don't allow removing the last question if it would leave section empty
  int currentCount = getQuestionCount();
  if (currentCount <= MIN_QUESTIONS_FOR_SECTION) {
    QMessageBox::information(this, tr("Cannot Remove Question"),
                             tr("A section must have at least one question."));
    return false;
  }

  // Get index before removal
  int index = ui->questionsLayout->indexOf(widget);

  // Remove from layout and delete
  ui->questionsLayout->removeWidget(widget);
  widget->deleteLater();

  emit questionRemoved(index);
  emit questionCountChanged(getQuestionCount());
  emit sectionChanged();

  return true;
}

bool SectionWidget::removeLastQuestion() {
  int count = getQuestionCount();

  if (count > 0) {
    return removeQuestionAt(count - 1);
  }

  return false;
}

bool SectionWidget::moveQuestionUp(int index) {
  if (index <= 0 || index >= getQuestionCount()) {
    return false;
  }

  QLayoutItem *item = ui->questionsLayout->takeAt(index);
  if (!item) {
    return false;
  }

  ui->questionsLayout->insertItem(index - 1, item);
  updateQuestionNumbers();
  emit sectionChanged();

  return true;
}

bool SectionWidget::moveQuestionDown(int index) {
  int count = getQuestionCount();

  if (index < 0 || index >= count - 1) {
    return false;
  }

  QLayoutItem *item = ui->questionsLayout->takeAt(index);
  if (!item) {
    return false;
  }

  ui->questionsLayout->insertItem(index + 1, item);
  updateQuestionNumbers();
  emit sectionChanged();

  return true;
}

void SectionWidget::updateQuestionNumbers() {
  // This could be extended to update visual question numbers
  // in each QuestionWidget if such a feature is added
  emit sectionChanged();
}

bool SectionWidget::validateSection() const {
  // Check if label is present
  if (ui->labelEdit->text().trimmed().isEmpty()) {
    return false;
  }

  // Check if there's at least one question
  if (getQuestionCount() == 0) {
    return false;
  }

  // Check if at least one question has content
  QVector<QuestionWidget *> widgets = getQuestionWidgets();
  for (QuestionWidget *widget : widgets) {
    if (widget && widget->hasContent()) {
      return true;
    }
  }

  return false;
}

void SectionWidget::onLabelChanged() { emit sectionChanged(); }

void SectionWidget::onSubtitleChanged() { emit sectionChanged(); }

void SectionWidget::onQuestionContentChanged() { emit sectionChanged(); }