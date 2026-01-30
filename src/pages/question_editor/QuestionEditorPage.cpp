#include "QuestionEditorPage.h"
#include "../../widgets/sectionWidget/SectionWidget.h"
#include "ui_QuestionEditorPage.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>

/**
 * @file QuestionEditorPage.cpp
 * @brief Implementation of the QuestionEditorPage class.
 */

// Constants
namespace {
const QString DEFAULT_FONT_FAMILY = "Times New Roman";
constexpr int DEFAULT_FONT_SIZE = 12;
constexpr int MIN_SECTIONS = 1;
constexpr int MAX_SECTIONS = 26; // A-Z

// Section label generation helpers
QString generateSectionLabel(int index) {
  if (index < 26) {
    return QString("Section %1").arg(QChar('A' + index));
  }
  // For more than 26 sections, use numbers
  return QString("Section %1").arg(index + 1);
}
} // namespace

QuestionEditorPage::QuestionEditorPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::QuestionEditorPage),
      m_addSectionButton(nullptr), m_defaultFontFamily(DEFAULT_FONT_FAMILY),
      m_defaultFontSize(DEFAULT_FONT_SIZE), m_contentModified(false) {
  ui->setupUi(this);
  setupUi();
  setupAddSectionButton();
  setupConnections();

  // Start with one default section
  addSectionWidget(generateNextSectionLabel());
}

QuestionEditorPage::~QuestionEditorPage() { delete ui; }

void QuestionEditorPage::setupUi() {
  // Configure scroll area
  ui->scrollArea->setWidgetResizable(true);
  ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  // Configure sections layout
  ui->sectionsLayout->setSpacing(15);
  ui->sectionsLayout->setContentsMargins(10, 10, 10, 10);

  // Style navigation buttons
  ui->backButton->setMinimumHeight(35);
  ui->nextButton->setMinimumHeight(35);
  ui->backButton->setIcon(QIcon::fromTheme("go-previous"));
  ui->nextButton->setIcon(QIcon::fromTheme("go-next"));

  // Make next button more prominent
  ui->nextButton->setDefault(true);

  // Add spacer at the bottom to push content up
  ui->sectionsLayout->addStretch();
}

void QuestionEditorPage::setupConnections() {
  // Navigation buttons
  connect(ui->backButton, &QPushButton::clicked, this,
          &QuestionEditorPage::onBackClicked);
  connect(ui->nextButton, &QPushButton::clicked, this,
          &QuestionEditorPage::onNextClicked);
}

void QuestionEditorPage::setupAddSectionButton() {
  // Create "Add Section" button
  m_addSectionButton = new QPushButton(tr("Add Section"), this);
  m_addSectionButton->setIcon(QIcon::fromTheme("list-add"));
  m_addSectionButton->setToolTip(
      tr("Add a new section to the exam paper (Ctrl+Shift+S)"));
  m_addSectionButton->setShortcut(QKeySequence("Ctrl+Shift+S"));
  m_addSectionButton->setMinimumHeight(40);

  // Add to layout (before the stretch)
  int stretchIndex = ui->sectionsLayout->count() - 1;
  ui->sectionsLayout->insertWidget(stretchIndex, m_addSectionButton);

  // Connect signal
  connect(m_addSectionButton, &QPushButton::clicked, this,
          &QuestionEditorPage::onAddSectionClicked);
}

void QuestionEditorPage::onAddSectionClicked() {
  // Generate default label
  QString defaultLabel = generateNextSectionLabel();

  // Prompt user for label
  QString label = promptForSectionLabel(defaultLabel);

  if (!label.isEmpty()) {
    addSection(label);
  }
}

QString QuestionEditorPage::promptForSectionLabel(const QString &defaultLabel) {
  bool ok;
  QString label = QInputDialog::getText(
      this, tr("Add Section"),
      tr("Enter section label (e.g., Section A, Part I):"), QLineEdit::Normal,
      defaultLabel, &ok);

  if (!ok) {
    return QString(); // User cancelled
  }

  label = label.trimmed();

  if (label.isEmpty()) {
    QMessageBox::warning(this, tr("Invalid Label"),
                         tr("Section label cannot be empty."));
    return QString();
  }

  return label;
}

void QuestionEditorPage::addSection(const QString &label) {
  // Check maximum sections
  if (getSectionCount() >= MAX_SECTIONS) {
    QMessageBox::warning(
        this, tr("Maximum Sections Reached"),
        tr("Cannot add more than %1 sections.").arg(MAX_SECTIONS));
    return;
  }

  // Use provided label or generate one
  QString sectionLabel = label.isEmpty() ? generateNextSectionLabel() : label;

  addSectionWidget(sectionLabel);
}

void QuestionEditorPage::addSectionWidget(const QString &label) {
  // Create section widget
  SectionWidget *sectionWidget = new SectionWidget(this);
  sectionWidget->setSectionLabel(label);
  sectionWidget->setDefaultFont(m_defaultFontFamily, m_defaultFontSize);

  // Connect section signals
  connect(sectionWidget, &SectionWidget::sectionChanged, this,
          &QuestionEditorPage::onSectionContentChanged);

  // Add to layout (before the "Add Section" button)
  int buttonIndex = ui->sectionsLayout->indexOf(m_addSectionButton);
  ui->sectionsLayout->insertWidget(buttonIndex, sectionWidget);

  // Add a separator line (optional visual improvement)
  QFrame *separator = new QFrame(this);
  separator->setFrameShape(QFrame::HLine);
  separator->setFrameShadow(QFrame::Sunken);
  ui->sectionsLayout->insertWidget(buttonIndex + 1, separator);

  int newIndex = getSectionCount() - 1;
  emit sectionAdded(newIndex);
  emit sectionCountChanged(getSectionCount());
  emit contentChanged();

  m_contentModified = true;

  // Scroll to the new section
  QScrollBar *scrollBar = ui->scrollArea->verticalScrollBar();
  if (scrollBar) {
    scrollBar->setValue(scrollBar->maximum());
  }
}

QVector<Section> QuestionEditorPage::getSections() const {
  QVector<Section> sections;

  QVector<SectionWidget *> sectionWidgets = getSectionWidgets();

  for (SectionWidget *widget : sectionWidgets) {
    if (widget && widget->hasValidContent()) {
      sections.append(widget->toSection());
    }
  }

  return sections;
}

void QuestionEditorPage::setSections(const QVector<Section> &sections) {
  // Clear existing sections
  clearAllSections();

  // Load sections
  for (const Section &section : sections) {
    SectionWidget *sectionWidget = new SectionWidget(this);
    sectionWidget->setDefaultFont(m_defaultFontFamily, m_defaultFontSize);
    sectionWidget->fromSection(section);

    // Connect signals
    connect(sectionWidget, &SectionWidget::sectionChanged, this,
            &QuestionEditorPage::onSectionContentChanged);

    // Add to layout
    int buttonIndex = ui->sectionsLayout->indexOf(m_addSectionButton);
    ui->sectionsLayout->insertWidget(buttonIndex, sectionWidget);
  }

  // If no sections were loaded, add one empty section
  if (getSectionCount() == 0) {
    addSection(generateNextSectionLabel());
  }

  emit sectionCountChanged(getSectionCount());
  m_contentModified = false;
}

int QuestionEditorPage::getSectionCount() const {
  return getSectionWidgets().size();
}

SectionWidget *QuestionEditorPage::getSectionWidget(int index) const {
  QVector<SectionWidget *> widgets = getSectionWidgets();

  if (index >= 0 && index < widgets.size()) {
    return widgets[index];
  }

  return nullptr;
}

QVector<SectionWidget *> QuestionEditorPage::getSectionWidgets() const {
  QVector<SectionWidget *> sectionWidgets;

  QLayout *layout = ui->sectionsLayout;
  if (!layout) {
    return sectionWidgets;
  }

  for (int i = 0; i < layout->count(); ++i) {
    QLayoutItem *item = layout->itemAt(i);
    if (item && item->widget()) {
      SectionWidget *sectionWidget =
          qobject_cast<SectionWidget *>(item->widget());
      if (sectionWidget) {
        sectionWidgets.append(sectionWidget);
      }
    }
  }

  return sectionWidgets;
}

bool QuestionEditorPage::hasValidContent() const {
  QVector<SectionWidget *> widgets = getSectionWidgets();

  for (SectionWidget *widget : widgets) {
    if (widget && widget->hasValidContent()) {
      return true;
    }
  }

  return false;
}

bool QuestionEditorPage::validateSections(QString &errorMessage) const {
  // Check minimum sections
  if (getSectionCount() < MIN_SECTIONS) {
    errorMessage = tr("At least one section is required.");
    return false;
  }

  // Check if at least one section has valid content
  if (!hasValidContent()) {
    errorMessage = tr(
        "At least one section must have valid content (label and questions).");
    return false;
  }

  // Validate each section
  QVector<SectionWidget *> widgets = getSectionWidgets();
  for (int i = 0; i < widgets.size(); ++i) {
    SectionWidget *widget = widgets[i];
    if (widget) {
      QString label = widget->getSectionLabel();
      if (label.isEmpty()) {
        errorMessage = tr("Section %1 is missing a label.").arg(i + 1);
        return false;
      }

      if (widget->getQuestionCount() == 0) {
        errorMessage = tr("Section '%1' has no questions.").arg(label);
        return false;
      }
    }
  }

  return true;
}

void QuestionEditorPage::clearAllSections() {
  QVector<SectionWidget *> widgets = getSectionWidgets();

  for (SectionWidget *widget : widgets) {
    if (widget) {
      ui->sectionsLayout->removeWidget(widget);
      widget->deleteLater();
    }
  }

  // Also remove any separator frames
  for (int i = ui->sectionsLayout->count() - 1; i >= 0; --i) {
    QLayoutItem *item = ui->sectionsLayout->itemAt(i);
    if (item && item->widget()) {
      QFrame *frame = qobject_cast<QFrame *>(item->widget());
      if (frame) {
        ui->sectionsLayout->removeWidget(frame);
        frame->deleteLater();
      }
    }
  }

  emit sectionCountChanged(0);
  emit contentChanged();
  m_contentModified = true;
}

void QuestionEditorPage::setDefaultFont(const QString &family, int size) {
  m_defaultFontFamily = family;
  m_defaultFontSize = size;

  // Apply to existing sections
  QVector<SectionWidget *> widgets = getSectionWidgets();
  for (SectionWidget *widget : widgets) {
    if (widget) {
      widget->setDefaultFont(family, size);
    }
  }
}

bool QuestionEditorPage::removeSectionAt(int index) {
  SectionWidget *widget = getSectionWidget(index);

  if (!widget) {
    return false;
  }

  return removeSectionWidget(widget);
}

bool QuestionEditorPage::removeSectionWidget(SectionWidget *widget) {
  if (!widget) {
    return false;
  }

  // Don't allow removing the last section
  if (getSectionCount() <= MIN_SECTIONS) {
    QMessageBox::information(
        this, tr("Cannot Remove Section"),
        tr("An exam paper must have at least one section."));
    return false;
  }

  // Confirm deletion if section has content
  if (widget->hasValidContent()) {
    QMessageBox::StandardButton reply =
        QMessageBox::question(this, tr("Confirm Deletion"),
                              tr("This section contains questions. Are you "
                                 "sure you want to delete it?"),
                              QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
      return false;
    }
  }

  // Get index before removal
  int index = ui->sectionsLayout->indexOf(widget);

  // Remove from layout and delete
  ui->sectionsLayout->removeWidget(widget);
  widget->deleteLater();

  emit sectionRemoved(index);
  emit sectionCountChanged(getSectionCount());
  emit contentChanged();

  m_contentModified = true;

  return true;
}

bool QuestionEditorPage::removeLastSection() {
  int count = getSectionCount();

  if (count > 0) {
    return removeSectionAt(count - 1);
  }

  return false;
}

bool QuestionEditorPage::moveSectionUp(int index) {
  if (index <= 0 || index >= getSectionCount()) {
    return false;
  }

  QLayoutItem *item = ui->sectionsLayout->takeAt(index);
  if (!item) {
    return false;
  }

  ui->sectionsLayout->insertItem(index - 1, item);
  updateSectionLabels();
  emit contentChanged();

  return true;
}

bool QuestionEditorPage::moveSectionDown(int index) {
  int count = getSectionCount();

  if (index < 0 || index >= count - 1) {
    return false;
  }

  QLayoutItem *item = ui->sectionsLayout->takeAt(index);
  if (!item) {
    return false;
  }

  ui->sectionsLayout->insertItem(index + 1, item);
  updateSectionLabels();
  emit contentChanged();

  return true;
}

int QuestionEditorPage::getTotalQuestionCount() const {
  int total = 0;

  QVector<SectionWidget *> widgets = getSectionWidgets();
  for (SectionWidget *widget : widgets) {
    if (widget) {
      total += widget->getQuestionCount();
    }
  }

  return total;
}

void QuestionEditorPage::setNavigationEnabled(bool backEnabled,
                                              bool nextEnabled) {
  ui->backButton->setEnabled(backEnabled);
  ui->nextButton->setEnabled(nextEnabled);
}

void QuestionEditorPage::updateSectionLabels() {
  // Optional: Auto-update section labels to maintain A, B, C sequence
  // This can be commented out if users prefer manual labels
  /*
  QVector<SectionWidget*> widgets = getSectionWidgets();
  for (int i = 0; i < widgets.size(); ++i) {
      if (widgets[i]) {
          widgets[i]->setSectionLabel(generateSectionLabel(i));
      }
  }
  */
  emit contentChanged();
}

QString QuestionEditorPage::generateNextSectionLabel() const {
  int count = getSectionCount();
  return generateSectionLabel(count);
}

bool QuestionEditorPage::validateMinimumSections() const {
  return getSectionCount() >= MIN_SECTIONS;
}

void QuestionEditorPage::onBackClicked() { emit backClicked(); }

void QuestionEditorPage::onNextClicked() {
  // Validate before allowing navigation
  QString errorMessage;
  if (!validateSections(errorMessage)) {
    QMessageBox::warning(this, tr("Validation Error"), errorMessage);
    return;
  }

  emit nextClicked();
}

void QuestionEditorPage::onSectionContentChanged() {
  m_contentModified = true;
  emit contentChanged();
}