#pragma once

#include <QString>
#include <QVector>
#include "Exam.h"
#include "Section.h"

/**
 * @file PaperModel.h
 * @brief Defines the PaperModel class for exam paper representation and rendering.
 */

/**
 * @class PaperModel
 * @brief Model class representing a complete exam paper with metadata and sections.
 * 
 * The PaperModel class encapsulates all exam data including exam metadata (title, subject,
 * duration, marks) and multiple sections containing questions. It provides functionality
 * to export the exam paper to various formats, primarily HTML for preview and further
 * conversion to PDF or DOCX.
 * 
 * @note This class follows the Model component of the MVC pattern.
 */
class PaperModel
{
public:
    /**
     * @brief Default constructor.
     */
    PaperModel() = default;

    /**
     * @brief Constructs a PaperModel with exam metadata and sections.
     * @param exam The exam metadata
     * @param sections Vector of sections containing questions
     */
    PaperModel(const Exam& exam, const QVector<Section>& sections);

    /**
     * @brief Exam metadata including title, subject, duration, and marks.
     */
    Exam exam;

    /**
     * @brief Collection of exam sections, each containing questions.
     */
    QVector<Section> sections;

    /**
     * @brief Converts the paper model to a formatted HTML string.
     * 
     * Generates a complete HTML document with embedded CSS styling that includes:
     * - Exam title and metadata (subject, duration, marks, class)
     * - Sectioned questions with proper formatting
     * - Support for multiple question types (MCQ, OR questions, standard questions)
     * - Embedded diagrams and tables
     * - Professional typography and layout
     * 
     * The generated HTML is suitable for:
     * - Preview in a web view or browser
     * - Conversion to PDF using a rendering engine
     * - Export to DOCX format
     * 
     * @param fontFamily Font family for the document (default: "Times New Roman")
     * @param fontSize Font size in points (default: 12)
     * @return QString Complete HTML document as a string
     * 
     * @note The HTML uses UTF-8 encoding and includes responsive image sizing.
     * @note Diagram paths are embedded as file:// URLs.
     */
    QString toHtml(const QString& fontFamily = "Times New Roman", int fontSize = 12, bool portrait = true) const;

    /**
     * @brief Validates the exam paper structure.
     * @return true if the paper has valid exam metadata and at least one section
     */
    bool isValid() const;

    /**
     * @brief Gets the total number of questions across all sections.
     * @return Total question count
     */
    int getTotalQuestions() const;

    /**
     * @brief Clears all exam data and sections.
     */
    void clear();

private:
    /**
     * @brief Renders a single section to HTML.
     * @param section The section to render
     * @return HTML string for the section
     */
    QString renderSection(const Section& section) const;

    /**
     * @brief Renders a single question to HTML.
     * @param question The question to render
     * @param questionNumber The question number
     * @return HTML string for the question
     */
    QString renderQuestion(const Question& question, int questionNumber) const;

    /**
     * @brief Renders a table to HTML.
     * @param table The table data
     * @return HTML string for the table
     */
    QString renderTable(const QVector<QVector<QString>>& table) const;
};