#pragma once

#include "../../models/Question.h"
#include <QFont>
#include <QWidget>

// Forward declarations
namespace Ui {
class QuestionWidget;
}

class QFontComboBox;
class QComboBox;
class QToolButton;

/**
 * @file QuestionWidget.h
 * @brief Defines the QuestionWidget class for question editing interface.
 */

/**
 * @class QuestionWidget
 * @brief Interactive widget for creating and editing exam questions.
 *
 * QuestionWidget provides a comprehensive interface for creating exam questions
 * with:
 * - Rich text formatting (font family, size, bold, italic, underline)
 * - Image attachment support
 * - Embedded table creation
 * - Export to Question model format
 *
 * The widget uses a QTextEdit for rich text input, supports file dialog-based
 * image selection, and provides dynamic table creation capabilities.
 *
 * @note This widget follows Qt's Model-View pattern for data management.
 */
class QuestionWidget : public QWidget {
  Q_OBJECT

public:
  /**
   * @brief Constructs a QuestionWidget.
   * @param parent Parent widget (default: nullptr)
   */
  explicit QuestionWidget(QWidget *parent = nullptr);

  /**
   * @brief Destructor - cleans up UI resources.
   */
  ~QuestionWidget() override;

  /**
   * @brief Converts the current UI content to a Question model object.
   *
   * Exports all question data including:
   * - Rich text content (with HTML formatting)
   * - Attached image path
   * - Table data (if present)
   *
   * @return Question object containing all widget data
   * @note The returned Question preserves HTML formatting from the text editor
   */
  Question toQuestion() const;

  /**
   * @brief Loads a Question model into the widget for editing.
   * @param question The Question object to load
   */
  void fromQuestion(const Question &question);

  /**
   * @brief Sets the default font for question text.
   * @param family Font family name (e.g., "Times New Roman", "Arial")
   * @param size Font size in points
   */
  void setDefaultFont(const QString &family, int size);

  /**
   * @brief Clears all content from the widget.
   */
  void clearContent();

  /**
   * @brief Checks if the widget has any content.
   * @return true if question has text, image, or table data
   */
  bool hasContent() const;

  /**
   * @brief Gets the plain text content (without HTML formatting).
   * @return Plain text string
   */
  QString getPlainText() const;

  /**
   * @brief Sets whether the rich text toolbar is visible.
   * @param visible true to show toolbar, false to hide
   */
  void setFormattingToolbarVisible(bool visible);
  /**
   * @brief Sets whether the widget is collapsed (summary view) or expanded
   * (editor).
   * @param collapsed true to collapse, false to expand
   */
  void setCollapsed(bool collapsed);

signals:
  /**
   * @brief Emitted when the question content changes.
   */
  void contentChanged();

  /**
   * @brief Emitted when an image is added or changed.
   * @param imagePath Path to the selected image
   */
  void imageChanged(const QString &imagePath);

  /**
   * @brief Emitted when the user wants to remove this question.
   */
  void removeRequested();

private slots:
  /**
   * @brief Handles the "Add Image" button click.
   * Opens a file dialog to select an image file.
   */
  void onAddImage();

  /**
   * @brief Handles the "Add Table" button click.
   * Creates a new table or adds a row to existing table.
   */
  void onAddTable();

  /**
   * @brief Handles the "Add Row" button click.
   * Adds a new row to the table.
   */
  void onAddRow();

  /**
   * @brief Handles the "Add Column" button click.
   * Adds a new column to the table.
   */
  void onAddColumn();

  /**
   * @brief Handles image removal.
   */
  void onRemoveImage();

  /**
   * @brief Handles changes in the text editor.
   */
  void onTextChanged();

  /**
   * @brief Handles change in question type.
   */
  void onTypeChanged(int index);

  /**
   * @brief Handles changes in MCQ options.
   */
  void onOptionChanged();

  /**
   * @brief Handles the "Done/Edit" button click.
   */
  void onDoneClicked();

private:
  /**
   * @brief UI object created from .ui file.
   */
  Ui::QuestionWidget *ui;

  /**
   * @brief Rich text formatting toolbar widgets.
   */
  QFontComboBox *m_fontComboBox;
  QComboBox *m_fontSizeComboBox;
  QToolButton *m_boldButton;
  QToolButton *m_italicButton;
  QToolButton *m_underlineButton;

  /**
   * @brief Layout containing formatting toolbar.
   */
  QWidget *m_formattingToolbar;

  /**
   * @brief Initializes the rich text formatting toolbar.
   * Creates and configures font selection, size selection, and text styling
   * buttons.
   */
  void setupFormattingToolbar();

  /**
   * @brief Sets up signal-slot connections.
   */
  void setupConnections();

  /**
   * @brief Configures UI components with default settings.
   */
  void setupUi();

  /**
   * @brief Updates the state of formatting buttons based on current cursor
   * format.
   */
  void updateFormattingButtons();

  /**
   * @brief Applies font formatting to selected text.
   * @param font Font to apply
   */
  void applyFontToSelection(const QFont &font);

  /**
   * @brief Validates image file path.
   * @param filePath Path to validate
   * @return true if path is valid and file exists
   */
  bool isValidImagePath(const QString &filePath) const;

  /**
   * @brief Exports table widget content to 2D vector.
   * @return QVector of QVector containing table cell data
   */
  QVector<QVector<QString>> exportTableData() const;

  /**
   * @brief Imports table data into the table widget.
   * @param tableData 2D vector containing table cell data
   */
  void importTableData(const QVector<QVector<QString>> &tableData);

  /**
   * @brief Tracks whether the widget is in collapsed summary view.
   */
  bool m_isCollapsed = false;

  /**
   * @brief Updates the UI to show either the editor or the summary.
   */
  void updateViewState();

  /**
   * @brief Generates a short summary text for the question.
   */
  QString generateSummary() const;
};