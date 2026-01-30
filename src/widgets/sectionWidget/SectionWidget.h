#pragma once

#include <QWidget>
#include <QVector>
#include "../../models/Section.h"

// Forward declarations
namespace Ui { 
    class SectionWidget; 
}

class QuestionWidget;
class QPushButton;

/**
 * @file SectionWidget.h
 * @brief Defines the SectionWidget class for managing exam sections.
 */

/**
 * @class SectionWidget
 * @brief Widget for creating and managing an exam section with multiple questions.
 * 
 * SectionWidget provides a comprehensive interface for managing exam sections:
 * - Section metadata (label, subtitle, instructions)
 * - Dynamic question management (add, remove, reorder)
 * - Question widget lifecycle management
 * - Export to Section model format
 * - Validation and content checking
 * 
 * The widget maintains a collection of QuestionWidget instances and provides
 * methods to manipulate them programmatically or through the UI.
 * 
 * @note Questions are stored as child widgets and managed automatically.
 */
class SectionWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a SectionWidget.
     * @param parent Parent widget (default: nullptr)
     */
    explicit SectionWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor - cleans up UI and child widgets.
     */
    ~SectionWidget() override;

    /**
     * @brief Converts the current section content to a Section model object.
     * 
     * Exports all section data including:
     * - Section label and subtitle
     * - All questions from child QuestionWidgets
     * 
     * @return Section object containing all widget data
     * @note Empty or invalid questions are excluded from the export
     */
    Section toSection() const;

    /**
     * @brief Loads a Section model into the widget for editing.
     * @param section The Section object to load
     */
    void fromSection(const Section& section);

    /**
     * @brief Sets the default font for all questions in this section.
     * @param family Font family name
     * @param size Font size in points
     */
    void setDefaultFont(const QString& family, int size);

    /**
     * @brief Gets the number of questions in this section.
     * @return Count of QuestionWidget children
     */
    int getQuestionCount() const;

    /**
     * @brief Gets a specific question widget by index.
     * @param index Zero-based index
     * @return Pointer to QuestionWidget, or nullptr if index is invalid
     */
    QuestionWidget* getQuestionWidget(int index) const;

    /**
     * @brief Gets all question widgets in this section.
     * @return Vector of QuestionWidget pointers
     */
    QVector<QuestionWidget*> getQuestionWidgets() const;

    /**
     * @brief Checks if the section has valid content.
     * @return true if section has a label and at least one question with content
     */
    bool hasValidContent() const;

    /**
     * @brief Clears all section content including label, subtitle, and questions.
     */
    void clearSection();

    /**
     * @brief Sets the section label (e.g., "Section A", "Part I").
     * @param label Section label text
     */
    void setSectionLabel(const QString& label);

    /**
     * @brief Gets the current section label.
     * @return Section label text
     */
    QString getSectionLabel() const;

    /**
     * @brief Sets the section subtitle/instructions.
     * @param subtitle Subtitle text
     */
    void setSectionSubtitle(const QString& subtitle);

    /**
     * @brief Gets the current section subtitle.
     * @return Section subtitle text
     */
    QString getSectionSubtitle() const;

    /**
     * @brief Removes a question widget at the specified index.
     * @param index Zero-based index of the question to remove
     * @return true if removal was successful
     */
    bool removeQuestionAt(int index);

    /**
     * @brief Moves a question up in the order.
     * @param index Current zero-based index of the question
     * @return true if move was successful
     */
    bool moveQuestionUp(int index);

    /**
     * @brief Moves a question down in the order.
     * @param index Current zero-based index of the question
     * @return true if move was successful
     */
    bool moveQuestionDown(int index);

signals:
    /**
     * @brief Emitted when section content changes.
     */
    void sectionChanged();

    /**
     * @brief Emitted when a question is added.
     * @param index Index of the newly added question
     */
    void questionAdded(int index);

    /**
     * @brief Emitted when a question is removed.
     * @param index Index of the removed question
     */
    void questionRemoved(int index);

    /**
     * @brief Emitted when the question count changes.
     * @param count New question count
     */
    void questionCountChanged(int count);

public slots:
    /**
     * @brief Adds a new question widget to this section.
     */
    void addQuestion();

    /**
     * @brief Removes the last question widget.
     * @return true if a question was removed
     */
    bool removeLastQuestion();

private slots:
    /**
     * @brief Handles the "Add Question" button click.
     */
    void onAddQuestion();

    /**
     * @brief Handles changes in section label.
     */
    void onLabelChanged();

    /**
     * @brief Handles changes in section subtitle.
     */
    void onSubtitleChanged();

    /**
     * @brief Handles content changes in any question widget.
     */
    void onQuestionContentChanged();

private:
    /**
     * @brief UI object created from .ui file.
     */
    Ui::SectionWidget* ui;

    /**
     * @brief Default font family for new questions.
     */
    QString m_defaultFontFamily;

    /**
     * @brief Default font size for new questions.
     */
    int m_defaultFontSize;

    /**
     * @brief Adds a new question widget with specified font settings.
     */
    void addQuestionWidget();

    /**
     * @brief Sets up signal-slot connections.
     */
    void setupConnections();

    /**
     * @brief Configures UI components with default settings.
     */
    void setupUi();

    /**
     * @brief Updates the question numbering display.
     */
    void updateQuestionNumbers();

    /**
     * @brief Validates section data before export.
     * @return true if section has required data
     */
    bool validateSection() const;

    /**
     * @brief Removes a specific question widget.
     * @param widget Pointer to the widget to remove
     * @return true if removal was successful
     */
    bool removeQuestionWidget(QuestionWidget* widget);
};