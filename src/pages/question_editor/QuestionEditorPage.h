#pragma once

#include <QWidget>
#include <QVector>
#include "../../models/PaperModel.h"

// Forward declarations
namespace Ui { 
    class QuestionEditorPage; 
}

class SectionWidget;
class QPushButton;

/**
 * @file QuestionEditorPage.h
 * @brief Defines the QuestionEditorPage class for managing exam paper sections.
 */

/**
 * @class QuestionEditorPage
 * @brief Main page for creating and editing exam paper sections and questions.
 * 
 * QuestionEditorPage provides a comprehensive interface for exam paper creation:
 * - Dynamic section management (add, remove, reorder)
 * - Section lifecycle management
 * - Data export to PaperModel format
 * - Validation and content checking
 * - Navigation between wizard pages
 * - Auto-save functionality support
 * 
 * The page maintains a collection of SectionWidget instances and provides
 * methods to manipulate them programmatically or through the UI.
 * 
 * @note This is typically used as part of a wizard-style interface.
 */
class QuestionEditorPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a QuestionEditorPage.
     * @param parent Parent widget (default: nullptr)
     */
    explicit QuestionEditorPage(QWidget* parent = nullptr);

    /**
     * @brief Destructor - cleans up UI and child widgets.
     */
    ~QuestionEditorPage() override;

    /**
     * @brief Exports all sections to a vector of Section objects.
     * @return QVector of Section objects
     * @note Only exports sections with valid content
     */
    QVector<Section> getSections() const;

    /**
     * @brief Loads sections into the page for editing.
     * @param sections Vector of Section objects to load
     */
    void setSections(const QVector<Section>& sections);

    /**
     * @brief Gets the number of sections in this page.
     * @return Count of SectionWidget children
     */
    int getSectionCount() const;

    /**
     * @brief Gets a specific section widget by index.
     * @param index Zero-based index
     * @return Pointer to SectionWidget, or nullptr if index is invalid
     */
    SectionWidget* getSectionWidget(int index) const;

    /**
     * @brief Gets all section widgets.
     * @return Vector of SectionWidget pointers
     */
    QVector<SectionWidget*> getSectionWidgets() const;

    /**
     * @brief Checks if the page has valid content for all sections.
     * @return true if at least one section has valid content
     */
    bool hasValidContent() const;

    /**
     * @brief Validates all sections.
     * @param errorMessage Output parameter for error message
     * @return true if all sections are valid
     */
    bool validateSections(QString& errorMessage) const;

    /**
     * @brief Clears all sections from the page.
     */
    void clearAllSections();

    /**
     * @brief Sets the default font for all sections.
     * @param family Font family name
     * @param size Font size in points
     */
    void setDefaultFont(const QString& family, int size);

    /**
     * @brief Removes a section at the specified index.
     * @param index Zero-based index of the section to remove
     * @return true if removal was successful
     */
    bool removeSectionAt(int index);

    /**
     * @brief Moves a section up in the order.
     * @param index Current zero-based index of the section
     * @return true if move was successful
     */
    bool moveSectionUp(int index);

    /**
     * @brief Moves a section down in the order.
     * @param index Current zero-based index of the section
     * @return true if move was successful
     */
    bool moveSectionDown(int index);

    /**
     * @brief Gets the total number of questions across all sections.
     * @return Total question count
     */
    int getTotalQuestionCount() const;

    /**
     * @brief Enables or disables the navigation buttons.
     * @param backEnabled Enable/disable back button
     * @param nextEnabled Enable/disable next button
     */
    void setNavigationEnabled(bool backEnabled, bool nextEnabled);

signals:
    /**
     * @brief Emitted when the user clicks the back button.
     */
    void backClicked();

    /**
     * @brief Emitted when the user clicks the next button.
     */
    void nextClicked();

    /**
     * @brief Emitted when any content changes in the page.
     */
    void contentChanged();

    /**
     * @brief Emitted when a section is added.
     * @param index Index of the newly added section
     */
    void sectionAdded(int index);

    /**
     * @brief Emitted when a section is removed.
     * @param index Index of the removed section
     */
    void sectionRemoved(int index);

    /**
     * @brief Emitted when the section count changes.
     * @param count New section count
     */
    void sectionCountChanged(int count);

public slots:
    /**
     * @brief Adds a new section with the specified label.
     * @param label Section label (e.g., "Section A", "Part I")
     */
    void addSection(const QString& label = QString());

    /**
     * @brief Removes the last section.
     * @return true if a section was removed
     */
    bool removeLastSection();

private slots:
    /**
     * @brief Handles the back button click.
     */
    void onBackClicked();

    /**
     * @brief Handles the next button click.
     */
    void onNextClicked();

    /**
     * @brief Handles the "Add Section" button click.
     */
    void onAddSectionClicked();

    /**
     * @brief Handles content changes in any section.
     */
    void onSectionContentChanged();

private:
    /**
     * @brief UI object created from .ui file.
     */
    Ui::QuestionEditorPage* ui;

    /**
     * @brief Add Section button.
     */
    QPushButton* m_addSectionButton;

    /**
     * @brief Default font family for new sections.
     */
    QString m_defaultFontFamily;

    /**
     * @brief Default font size for new sections.
     */
    int m_defaultFontSize;

    /**
     * @brief Flag to track if content has been modified.
     */
    bool m_contentModified;

    /**
     * @brief Sets up signal-slot connections.
     */
    void setupConnections();

    /**
     * @brief Configures UI components with default settings.
     */
    void setupUi();

    /**
     * @brief Creates and configures the "Add Section" button.
     */
    void setupAddSectionButton();

    /**
     * @brief Prompts user for section label.
     * @param defaultLabel Default label to show in dialog
     * @return Section label entered by user, or empty string if cancelled
     */
    QString promptForSectionLabel(const QString& defaultLabel = QString());

    /**
     * @brief Adds a section widget with the specified label.
     * @param label Section label
     */
    void addSectionWidget(const QString& label);

    /**
     * @brief Removes a specific section widget.
     * @param widget Pointer to the widget to remove
     * @return true if removal was successful
     */
    bool removeSectionWidget(SectionWidget* widget);

    /**
     * @brief Updates section labels to maintain consistency (A, B, C...).
     */
    void updateSectionLabels();

    /**
     * @brief Generates the next section label in sequence.
     * @return Next label (e.g., "A", "B", "C"...)
     */
    QString generateNextSectionLabel() const;

    /**
     * @brief Validates that at least one section exists.
     * @return true if validation passes
     */
    bool validateMinimumSections() const;
};