#pragma once

#include <QWidget>
#include "../../models/Exam.h"

// Forward declarations
namespace Ui { 
    class ExamInfoPage; 
}

/**
 * @file ExamInfoPage.h
 * @brief Defines the ExamInfoPage class for collecting exam metadata.
 */

/**
 * @class ExamInfoPage
 * @brief Page for collecting and validating exam metadata.
 * 
 * ExamInfoPage provides a comprehensive interface for entering exam information:
 * - Basic metadata (title, subject, duration)
 * - Marking information (total marks, pass marks)
 * - Class/grade information
 * - Date and term/semester information
 * - Real-time validation
 * - Data persistence (load/save)
 * 
 * The page validates all inputs before allowing navigation to ensure
 * data integrity and completeness.
 * 
 * @note This is typically the first page in a wizard-style interface.
 */
class ExamInfoPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an ExamInfoPage.
     * @param parent Parent widget (default: nullptr)
     */
    explicit ExamInfoPage(QWidget* parent = nullptr);

    /**
     * @brief Destructor - cleans up UI resources.
     */
    ~ExamInfoPage() override;

    /**
     * @brief Exports the exam metadata to an Exam object.
     * @return Exam object containing all form data
     */
    Exam getExamInfo() const;

    /**
     * @brief Loads exam metadata into the form.
     * @param exam Exam object to load
     */
    void setExamInfo(const Exam& exam);

    /**
     * @brief Validates all form fields.
     * @param errorMessage Output parameter for error message
     * @return true if all fields are valid
     */
    bool validateForm(QString& errorMessage) const;

    /**
     * @brief Checks if the form has been modified.
     * @return true if any field has been changed
     */
    bool isModified() const;

    /**
     * @brief Clears all form fields.
     */
    void clearForm();

    /**
     * @brief Sets focus to the first input field.
     */
    void setInitialFocus();

    /**
     * @brief Gets the exam title.
     * @return Exam title string
     */
    QString getTitle() const;

    /**
     * @brief Gets the subject name.
     * @return Subject string
     */
    QString getSubject() const;

    /**
     * @brief Gets the exam duration.
     * @return Duration string
     */
    QString getDuration() const;

    /**
     * @brief Gets the total marks.
     * @return Total marks value
     */
    int getTotalMarks() const;

    /**
     * @brief Gets the pass marks.
     * @return Pass marks value
     */
    int getPassMarks() const;

    /**
     * @brief Gets the class/grade name.
     * @return Class name string
     */
    QString getClassName() const;

    /**
     * @brief Enables or disables the next button.
     * @param enabled Enable/disable state
     */
    void setNextButtonEnabled(bool enabled);

signals:
    /**
     * @brief Emitted when the user clicks the next button.
     */
    void nextClicked();

    /**
     * @brief Emitted when any form field changes.
     */
    void formChanged();

    /**
     * @brief Emitted when form validation state changes.
     * @param isValid true if form is valid
     */
    void validationChanged(bool isValid);

private slots:
    /**
     * @brief Handles the next button click.
     */
    void onNextClicked();

    /**
     * @brief Handles changes in any form field.
     */
    void onFieldChanged();

    /**
     * @brief Handles changes in total marks.
     */
    void onTotalMarksChanged(int value);

    /**
     * @brief Handles changes in pass marks.
     */
    void onPassMarksChanged(int value);

private:
    /**
     * @brief UI object created from .ui file.
     */
    Ui::ExamInfoPage* ui;

    /**
     * @brief Flag to track if form has been modified.
     */
    bool m_isModified;

    /**
     * @brief Original exam data for modification tracking.
     */
    Exam m_originalExam;

    /**
     * @brief Sets up signal-slot connections.
     */
    void setupConnections();

    /**
     * @brief Configures UI components with default settings.
     */
    void setupUi();

    /**
     * @brief Configures form field validators and limits.
     */
    void setupValidators();

    /**
     * @brief Updates the UI based on current validation state.
     */
    void updateValidationState();

    /**
     * @brief Validates a required text field.
     * @param text Text to validate
     * @param fieldName Name of the field for error messages
     * @param errorMessage Output parameter for error message
     * @return true if field is valid
     */
    bool validateRequiredField(const QString& text, 
                               const QString& fieldName, 
                               QString& errorMessage) const;

    /**
     * @brief Validates marks fields (ensures pass marks <= total marks).
     * @param errorMessage Output parameter for error message
     * @return true if marks are valid
     */
    bool validateMarks(QString& errorMessage) const;

    /**
     * @brief Validates duration format.
     * @param errorMessage Output parameter for error message
     * @return true if duration is valid
     */
    bool validateDuration(QString& errorMessage) const;

    /**
     * @brief Shows validation error to the user.
     * @param message Error message to display
     */
    void showValidationError(const QString& message);
};