#include "QuestionWidget.h"
#include "ui_QuestionWidget.h"
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontComboBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QToolButton>

/**
 * @file QuestionWidget.cpp
 * @brief Implementation of the QuestionWidget class.
 */

// Constants
namespace {
constexpr int DEFAULT_FONT_SIZE = 12;
constexpr int MIN_FONT_SIZE = 8;
constexpr int MAX_FONT_SIZE = 24;
constexpr int FONT_SIZE_STEP = 2;
constexpr int DEFAULT_TABLE_ROWS = 2;
constexpr int DEFAULT_TABLE_COLS = 2;

const QString IMAGE_FILTER =
    QObject::tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.svg)");
} // namespace

QuestionWidget::QuestionWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::QuestionWidget), m_fontComboBox(nullptr),
      m_fontSizeComboBox(nullptr), m_boldButton(nullptr),
      m_italicButton(nullptr), m_underlineButton(nullptr),
      m_formattingToolbar(nullptr) {
  ui->setupUi(this);
  setupUi();
  setupFormattingToolbar();
  setupConnections();
}

QuestionWidget::~QuestionWidget() { delete ui; }

void QuestionWidget::setupUi() {
  // Configure text edit
  ui->textEdit->setAcceptRichText(true);
  ui->textEdit->setPlaceholderText(tr("Enter question text here..."));

  // Configure type combo box
  ui->typeComboBox->addItem(tr("Regular"),
                            static_cast<int>(QuestionType::Regular));
  ui->typeComboBox->addItem(tr("MCQ"), static_cast<int>(QuestionType::Mcq));
  ui->typeComboBox->addItem(tr("OR"), static_cast<int>(QuestionType::Or));
  ui->typeComboBox->addItem(tr("Mixed"), static_cast<int>(QuestionType::Mixed));

  // Configure table widget
  ui->tableWidget->setVisible(false);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

  // Style icons
  ui->removeQuestionButton->setIcon(QIcon::fromTheme("edit-delete"));
  ui->removeQuestionButton->setToolTip(tr("Remove this question"));
  ui->addImageButton->setIcon(QIcon::fromTheme("insert-image"));
  ui->addTableButton->setIcon(QIcon::fromTheme("insert-table"));
  ui->addRowButton->setVisible(false);
  ui->addColumnButton->setVisible(false);
  ui->clearButton->setIcon(QIcon::fromTheme("edit-clear-all"));

  // Initial state
  ui->typeSpecificStack->setCurrentIndex(0);
}

void QuestionWidget::setupConnections() {
  // Button connections
  connect(ui->addImageButton, &QPushButton::clicked, this,
          &QuestionWidget::onAddImage);
  connect(ui->addTableButton, &QPushButton::clicked, this,
          &QuestionWidget::onAddTable);
  connect(ui->addRowButton, &QPushButton::clicked, this,
          &QuestionWidget::onAddRow);
  connect(ui->addColumnButton, &QPushButton::clicked, this,
          &QuestionWidget::onAddColumn);
  connect(ui->clearButton, &QPushButton::clicked, this,
          &QuestionWidget::clearContent);
  connect(ui->removeQuestionButton, &QPushButton::clicked, this,
          &QuestionWidget::removeRequested);
  connect(ui->doneButton, &QPushButton::clicked, this,
          &QuestionWidget::onDoneClicked);

  // Type change
  connect(ui->typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &QuestionWidget::onTypeChanged);

  // MCQ option changes
  connect(ui->optionAEdit, &QLineEdit::textChanged, this,
          &QuestionWidget::onOptionChanged);
  connect(ui->optionBEdit, &QLineEdit::textChanged, this,
          &QuestionWidget::onOptionChanged);
  connect(ui->optionCEdit, &QLineEdit::textChanged, this,
          &QuestionWidget::onOptionChanged);
  connect(ui->optionDEdit, &QLineEdit::textChanged, this,
          &QuestionWidget::onOptionChanged);

  // Text change notification
  connect(ui->textEdit, &QTextEdit::textChanged, this,
          &QuestionWidget::onTextChanged);
  connect(ui->orTextEdit, &QTextEdit::textChanged, this,
          &QuestionWidget::onTextChanged);

  // Cursor position change for formatting updates
  connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this,
          &QuestionWidget::updateFormattingButtons);
}

void QuestionWidget::onTypeChanged(int index) {
  QuestionType type =
      static_cast<QuestionType>(ui->typeComboBox->itemData(index).toInt());

  // Show/hide relevant stack page
  if (type == QuestionType::Mcq || type == QuestionType::Mixed) {
    ui->typeSpecificStack->setCurrentWidget(ui->mcqPage);
  } else if (type == QuestionType::Or) {
    ui->typeSpecificStack->setCurrentWidget(ui->orPage);
  } else {
    ui->typeSpecificStack->setCurrentWidget(ui->regularPage);
  }

  emit contentChanged();
}

void QuestionWidget::onOptionChanged() { emit contentChanged(); }

void QuestionWidget::setupFormattingToolbar() {
  // Create toolbar widget
  m_formattingToolbar = new QWidget(this);
  QHBoxLayout *toolbarLayout = new QHBoxLayout(m_formattingToolbar);
  toolbarLayout->setContentsMargins(0, 0, 0, 5);
  toolbarLayout->setSpacing(5);

  // Font family combo box
  m_fontComboBox = new QFontComboBox(this);
  m_fontComboBox->setMaximumWidth(200);
  m_fontComboBox->setToolTip(tr("Font Family"));
  connect(m_fontComboBox, &QFontComboBox::currentFontChanged, this,
          [this](const QFont &font) {
            ui->textEdit->setFontFamily(font.family());
            ui->textEdit->setFocus();
          });

  // Font size combo box
  m_fontSizeComboBox = new QComboBox(this);
  m_fontSizeComboBox->setMaximumWidth(60);
  m_fontSizeComboBox->setToolTip(tr("Font Size"));
  for (int size = MIN_FONT_SIZE; size <= MAX_FONT_SIZE;
       size += FONT_SIZE_STEP) {
    m_fontSizeComboBox->addItem(QString::number(size));
  }
  m_fontSizeComboBox->setCurrentText(QString::number(DEFAULT_FONT_SIZE));
  connect(m_fontSizeComboBox, &QComboBox::currentTextChanged, this,
          [this](const QString &sizeText) {
            bool ok;
            double size = sizeText.toDouble(&ok);
            if (ok && size > 0) {
              ui->textEdit->setFontPointSize(size);
              ui->textEdit->setFocus();
            }
          });

  // Bold button
  m_boldButton = new QToolButton(this);
  m_boldButton->setText(tr("B"));
  m_boldButton->setCheckable(true);
  m_boldButton->setToolTip(tr("Bold (Ctrl+B)"));
  m_boldButton->setFont(QFont("Arial", 10, QFont::Bold));
  m_boldButton->setMaximumSize(30, 30);
  connect(m_boldButton, &QToolButton::clicked, this, [this](bool checked) {
    ui->textEdit->setFontWeight(checked ? QFont::Bold : QFont::Normal);
    ui->textEdit->setFocus();
  });

  // Italic button
  m_italicButton = new QToolButton(this);
  m_italicButton->setText(tr("I"));
  m_italicButton->setCheckable(true);
  m_italicButton->setToolTip(tr("Italic (Ctrl+I)"));
  QFont italicFont("Arial", 10);
  italicFont.setItalic(true);
  m_italicButton->setFont(italicFont);
  m_italicButton->setMaximumSize(30, 30);
  connect(m_italicButton, &QToolButton::clicked, this, [this](bool checked) {
    ui->textEdit->setFontItalic(checked);
    ui->textEdit->setFocus();
  });

  // Underline button
  m_underlineButton = new QToolButton(this);
  m_underlineButton->setText(tr("U"));
  m_underlineButton->setCheckable(true);
  m_underlineButton->setToolTip(tr("Underline (Ctrl+U)"));
  QFont underlineFont("Arial", 10);
  underlineFont.setUnderline(true);
  m_underlineButton->setFont(underlineFont);
  m_underlineButton->setMaximumSize(30, 30);
  connect(m_underlineButton, &QToolButton::clicked, this, [this](bool checked) {
    ui->textEdit->setFontUnderline(checked);
    ui->textEdit->setFocus();
  });

  // Add widgets to toolbar
  toolbarLayout->addWidget(m_fontComboBox);
  toolbarLayout->addWidget(m_fontSizeComboBox);
  toolbarLayout->addWidget(m_boldButton);
  toolbarLayout->addWidget(m_italicButton);
  toolbarLayout->addWidget(m_underlineButton);
  toolbarLayout->addStretch();

  // Insert toolbar at the top of the layout
  ui->leftLayout->insertWidget(0, m_formattingToolbar);
}

void QuestionWidget::updateFormattingButtons() {
  if (!m_boldButton || !m_italicButton || !m_underlineButton) {
    return;
  }

  // Block signals to prevent recursive updates
  m_boldButton->blockSignals(true);
  m_italicButton->blockSignals(true);
  m_underlineButton->blockSignals(true);

  // Update button states based on current format
  QTextCharFormat format = ui->textEdit->currentCharFormat();
  m_boldButton->setChecked(format.fontWeight() == QFont::Bold);
  m_italicButton->setChecked(format.fontItalic());
  m_underlineButton->setChecked(format.fontUnderline());

  // Unblock signals
  m_boldButton->blockSignals(false);
  m_italicButton->blockSignals(false);
  m_underlineButton->blockSignals(false);
}

Question QuestionWidget::toQuestion() const {
  Question question;

  // Set type
  question.type =
      static_cast<QuestionType>(ui->typeComboBox->currentData().toInt());

  // Export rich text content
  question.text = ui->textEdit->toHtml();

  // Export image path (only if valid)
  QString imagePath = ui->imagePathLabel->text();
  if (isValidImagePath(imagePath)) {
    question.diagramPath = imagePath;
  }

  // Export table data
  question.table = exportTableData();

  // Export MCQ options if applicable
  if (question.type == QuestionType::Mcq ||
      question.type == QuestionType::Mixed) {
    question.options.clear();
    question.options << ui->optionAEdit->text().trimmed();
    question.options << ui->optionBEdit->text().trimmed();
    question.options << ui->optionCEdit->text().trimmed();
    question.options << ui->optionDEdit->text().trimmed();
  } else if (question.type == QuestionType::Or) {
    // Export alternative question
    QString orText = ui->orTextEdit->toPlainText().trimmed();
    if (!orText.isEmpty()) {
      Question altQuestion;
      altQuestion.text = orText;
      // Sub-question type doesn't arguably matter much for basic rendering,
      // but we can set it to Regular.
      altQuestion.type = QuestionType::Regular;
      question.subQuestions.append(altQuestion);
    }
  }

  return question;
}

void QuestionWidget::fromQuestion(const Question &question) {
  // Block signals to prevent recursive updates
  ui->typeComboBox->blockSignals(true);

  // Set type
  int typeIndex = ui->typeComboBox->findData(static_cast<int>(question.type));
  if (typeIndex >= 0) {
    ui->typeComboBox->setCurrentIndex(typeIndex);
    onTypeChanged(typeIndex);
  }

  ui->typeComboBox->blockSignals(false);

  // Load text content
  if (question.text.contains("<html>") || question.text.contains("<p>")) {
    ui->textEdit->setHtml(question.text);
  } else {
    ui->textEdit->setPlainText(question.text);
  }

  // Load image path
  if (!question.diagramPath.isEmpty() &&
      isValidImagePath(question.diagramPath)) {
    ui->imagePathLabel->setText(question.diagramPath);
  }

  // Load table data
  if (!question.table.isEmpty()) {
    importTableData(question.table);
    ui->tableWidget->setVisible(true);
  }

  // Load MCQ options
  if (question.options.size() >= 4) {
    ui->optionAEdit->setText(question.options[0]);
    ui->optionBEdit->setText(question.options[1]);
    ui->optionCEdit->setText(question.options[2]);
    ui->optionCEdit->setText(question.options[2]);
    ui->optionDEdit->setText(question.options[3]);
  }

  // Load OR alternative
  if (question.type == QuestionType::Or && !question.subQuestions.isEmpty()) {
    ui->orTextEdit->setText(question.subQuestions.first().text);
  }
}

QVector<QVector<QString>> QuestionWidget::exportTableData() const {
  QVector<QVector<QString>> tableData;

  int rowCount = ui->tableWidget->rowCount();
  int colCount = ui->tableWidget->columnCount();

  if (rowCount == 0 || colCount == 0) {
    return tableData;
  }

  tableData.resize(rowCount);
  for (int row = 0; row < rowCount; ++row) {
    tableData[row].resize(colCount);
    for (int col = 0; col < colCount; ++col) {
      QTableWidgetItem *item = ui->tableWidget->item(row, col);
      tableData[row][col] = item ? item->text() : QString();
    }
  }

  return tableData;
}

void QuestionWidget::importTableData(
    const QVector<QVector<QString>> &tableData) {
  if (tableData.isEmpty()) {
    return;
  }

  int rowCount = tableData.size();
  int colCount = tableData[0].size();

  ui->tableWidget->setRowCount(rowCount);
  ui->tableWidget->setColumnCount(colCount);

  for (int row = 0; row < rowCount; ++row) {
    for (int col = 0; col < colCount && col < tableData[row].size(); ++col) {
      QTableWidgetItem *item = new QTableWidgetItem(tableData[row][col]);
      ui->tableWidget->setItem(row, col, item);
    }
  }
}

void QuestionWidget::onAddImage() {
  QString filePath = QFileDialog::getOpenFileName(this, tr("Select Image"),
                                                  QString(), IMAGE_FILTER);

  if (filePath.isEmpty()) {
    return; // User cancelled
  }

  if (!isValidImagePath(filePath)) {
    QMessageBox::warning(
        this, tr("Invalid Image"),
        tr("The selected file does not exist or is not accessible."));
    return;
  }

  ui->imagePathLabel->setText(filePath);
  emit imageChanged(filePath);
  emit contentChanged();
}

void QuestionWidget::onRemoveImage() {
  ui->imagePathLabel->setText(tr("No image selected"));
  emit imageChanged(QString());
  emit contentChanged();
}

void QuestionWidget::onAddTable() {
  bool ok;
  // Prompt for Rows
  int rows = QInputDialog::getInt(this, tr("Add Table"), tr("Rows:"), 2, 1, 20,
                                  1, &ok);
  if (!ok) {
    return;
  }

  // Prompt for Columns
  int cols = QInputDialog::getInt(this, tr("Add Table"), tr("Columns:"), 2, 1,
                                  10, 1, &ok);
  if (!ok) {
    return;
  }

  // If table already exists, confirm replacement or new setup (since model only
  // supports one table per question for now)
  if (ui->tableWidget->rowCount() > 0) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Replace Table"),
        tr("A table already exists. Do you want to replace it with a new one?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
      return;
    }
  }

  ui->tableWidget->setRowCount(rows);
  ui->tableWidget->setColumnCount(cols);
  ui->tableWidget->setVisible(true);

  emit contentChanged();
}

void QuestionWidget::onAddRow() {
  if (ui->tableWidget->rowCount() == 0) {
    // Create new table if none exists
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->setColumnCount(DEFAULT_TABLE_COLS);
    ui->tableWidget->setVisible(true);
  } else {
    // Add a new row to existing table
    int newRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(newRow);
  }

  emit contentChanged();
}

void QuestionWidget::onAddColumn() {
  if (ui->tableWidget->columnCount() == 0) {
    // Create new table if none exists
    ui->tableWidget->setRowCount(DEFAULT_TABLE_ROWS);
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setVisible(true);
  } else {
    // Add a new column to existing table
    int newColumn = ui->tableWidget->columnCount();
    ui->tableWidget->insertColumn(newColumn);
  }

  emit contentChanged();
}

void QuestionWidget::onTextChanged() { emit contentChanged(); }

void QuestionWidget::setDefaultFont(const QString &family, int size) {
  QFont font(family, size);
  ui->textEdit->setFont(font);

  // Update combo boxes to reflect the new defaults
  if (m_fontComboBox) {
    m_fontComboBox->setCurrentFont(font);
  }
  if (m_fontSizeComboBox) {
    m_fontSizeComboBox->setCurrentText(QString::number(size));
  }
}

void QuestionWidget::clearContent() {
  ui->textEdit->clear();
  ui->imagePathLabel->setText(tr("No image selected"));
  ui->tableWidget->clear();
  ui->tableWidget->setRowCount(0);
  ui->tableWidget->setColumnCount(0);
  ui->tableWidget->setVisible(false);

  // Clear MCQ options
  ui->optionAEdit->clear();
  ui->optionBEdit->clear();
  ui->optionCEdit->clear();
  ui->optionCEdit->clear();
  ui->optionDEdit->clear();

  // Clear OR text
  ui->orTextEdit->clear();

  emit contentChanged();
}

bool QuestionWidget::hasContent() const {
  bool hasText = !ui->textEdit->toPlainText().trimmed().isEmpty();
  bool hasImage = isValidImagePath(ui->imagePathLabel->text());
  bool hasTable = ui->tableWidget->rowCount() > 0;

  return hasText || hasImage || hasTable;
}

QString QuestionWidget::getPlainText() const {
  return ui->textEdit->toPlainText();
}

void QuestionWidget::setFormattingToolbarVisible(bool visible) {
  if (m_formattingToolbar) {
    m_formattingToolbar->setVisible(visible);
  }
}

void QuestionWidget::setCollapsed(bool collapsed) {
  if (m_isCollapsed != collapsed) {
    m_isCollapsed = collapsed;
    updateViewState();
  }
}

bool QuestionWidget::isValidImagePath(const QString &filePath) const {
  if (filePath.isEmpty() || filePath == tr("No image selected")) {
    return false;
  }

  QFileInfo fileInfo(filePath);
  return fileInfo.exists() && fileInfo.isFile();
}

void QuestionWidget::applyFontToSelection(const QFont &font) {
  QTextCursor cursor = ui->textEdit->textCursor();
  if (!cursor.hasSelection()) {
    cursor.select(QTextCursor::WordUnderCursor);
  }

  QTextCharFormat format;
  format.setFont(font);
  cursor.mergeCharFormat(format);
  ui->textEdit->mergeCurrentCharFormat(format);
}

void QuestionWidget::onDoneClicked() {
  m_isCollapsed = !m_isCollapsed;
  updateViewState();
}

void QuestionWidget::updateViewState() {
  bool isEditing = !m_isCollapsed;

  // Toggle editor visibility
  ui->textEdit->setVisible(isEditing);
  ui->typeComboBox->setVisible(isEditing);
  ui->typeLabel->setVisible(isEditing);
  ui->typeSpecificStack->setVisible(isEditing);
  ui->tableWidget->setVisible(isEditing && ui->tableWidget->rowCount() > 0);
  if (m_formattingToolbar) {
    m_formattingToolbar->setVisible(isEditing);
  }
  ui->addImageButton->setVisible(isEditing);
  ui->addTableButton->setVisible(isEditing);
  ui->addRowButton->setVisible(isEditing && ui->tableWidget->rowCount() > 0);
  ui->addColumnButton->setVisible(isEditing &&
                                  ui->tableWidget->columnCount() > 0);
  ui->imagePathLabel->setVisible(isEditing);
  ui->clearButton->setVisible(isEditing);

  // Toggle summary visibility
  ui->summaryLabel->setVisible(m_isCollapsed);
  if (m_isCollapsed) {
    ui->summaryLabel->setText(generateSummary());
    ui->doneButton->setText(tr("Edit"));
    ui->doneButton->setStyleSheet("background-color: #007acc; color: white;");
  } else {
    ui->doneButton->setText(tr("Done"));
    ui->doneButton->setStyleSheet("background-color: #28a745; color: white;");
  }
}

QString QuestionWidget::generateSummary() const {
  QString plainText = ui->textEdit->toPlainText().trimmed();
  if (plainText.isEmpty()) {
    plainText = tr("(No question text)");
  }

  // Limit summary length
  if (plainText.length() > 100) {
    plainText = plainText.left(97) + "...";
  }

  QString typeName = ui->typeComboBox->currentText();
  return tr("<b>[%1]</b> %2").arg(typeName, plainText);
}