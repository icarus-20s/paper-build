#include "PaperModel.h"
#include <QChar>
#include <QStringBuilder>

/**
 * @file PaperModel.cpp
 * @brief Implementation of the PaperModel class.
 */

// Constructor with parameters
PaperModel::PaperModel(const Exam &exam, const QVector<Section> &sections)
    : exam(exam), sections(sections) {}

// HTML generation constants
namespace {
constexpr int DEFAULT_MARGIN = 20;
constexpr int QUESTION_NUMBER_WIDTH = 30;
constexpr int OR_INDENT = 20;

const QString HTML_HEADER_TEMPLATE = "<html>"
                                     "<head>"
                                     "<meta charset=\"utf-8\">"
                                     "<style>"
                                     "body { "
                                     "font-family:'%1', serif; "
                                     "font-size:%2pt; "
                                     "margin:%3px; "
                                     "line-height:1.6; "
                                     "}"
                                     "p { "
                                     "margin:0; "
                                     "padding:0; "
                                     "}"
                                     "h1 { "
                                     "text-align:center; "
                                     "margin-bottom:10px; "
                                     "font-size:1.5em; "
                                     "}"
                                     "h2 { "
                                     "text-align:center; "
                                     "margin-top:20px; "
                                     "margin-bottom:5px; "
                                     "font-size:1.2em; "
                                     "}"
                                     ".metadata { "
                                     "text-align:center; "
                                     "margin-bottom:20px; "
                                     "font-size:0.9em; "
                                     "}"
                                     ".section { "
                                     "margin-top:20px; "
                                     "page-break-inside:avoid; "
                                     "}"
                                     ".subtitle { "
                                     "text-align:center; "
                                     "font-weight:bold; "
                                     "font-size:0.95em; "
                                     "margin-bottom:10px; "
                                     "font-style:italic; "
                                     "}"
                                     ".question { "
                                     "margin:5px 0; "
                                     "text-align:left; "
                                     "}"
                                     ".question-layout { "
                                     "width:100%; "
                                     "border-collapse:collapse; "
                                     "}"
                                     ".question-layout td { "
                                     "border:none; "
                                     "padding:0; "
                                     "vertical-align:top; "
                                     "}"
                                     ".question-num-cell { "
                                     "width:%4px; "
                                     "font-weight:bold; "
                                     "}"
                                     ".or-question { "
                                     "margin-left:%5px; "
                                     "margin-top:5px; "
                                     "}"
                                     ".mcq-table { "
                                     "width:100%; "
                                     "border-collapse:collapse; "
                                     "margin-top:5px; "
                                     "margin-left:30px; "
                                     "}"
                                     ".mcq-table td { "
                                     "border:none; "
                                     "padding:2px 5px; "
                                     "width:50%; "
                                     "}"
                                     "table { "
                                     "border-collapse:collapse; "
                                     "margin:5px 0; "
                                     "width:auto; "
                                     "}"
                                     "td, th { "
                                     "border:1px solid #000; "
                                     "padding:4px 6px; "
                                     "text-align:left; "
                                     "}"
                                     "th { "
                                     "background-color:#f0f0f0; "
                                     "font-weight:bold; "
                                     "}"
                                     "img { "
                                     "max-width:100%; "
                                     "height:auto; "
                                     "margin:5px 0; "
                                     "display:block; "
                                     "}"
                                     "</style>"
                                     "</head>"
                                     "<body>";
} // namespace

QString PaperModel::toHtml(const QString &fontFamily, int fontSize) const {
  // Build HTML using QString builder for better performance
  QString html;
  html.reserve(4096); // Reserve initial capacity to reduce reallocations

  // HTML header with CSS
  QString pageStyle = exam.isLandscape ? "size: A4 landscape; margin: 10mm;"
                                       : "size: A4 portrait; margin: 10mm;";

  html = QString(HTML_HEADER_TEMPLATE)
             .arg(fontFamily)
             .arg(fontSize)
             .arg(DEFAULT_MARGIN)
             .arg(QUESTION_NUMBER_WIDTH)
             .arg(OR_INDENT);

  // Inject page style
  html.replace("</style>", "@page { " + pageStyle + " } </style>");

  // Exam title
  if (!exam.title.isEmpty()) {
    html += "<h1>" % exam.title.toHtmlEscaped() % "</h1>";
  }

  // Exam metadata
  html += "<div class=\"metadata\">";
  QStringList metadataParts;

  if (!exam.subject.isEmpty()) {
    metadataParts << exam.subject.toHtmlEscaped();
  }
  if (!exam.duration.isEmpty()) {
    metadataParts << exam.duration.toHtmlEscaped();
  }
  if (exam.totalMarks > 0) {
    metadataParts << QString("Total Marks: %1").arg(exam.totalMarks);
  }
  if (exam.passMarks > 0) {
    metadataParts << QString("Pass Marks: %1").arg(exam.passMarks);
  }
  if (!exam.className.isEmpty()) {
    metadataParts << QString("Class: %1").arg(exam.className.toHtmlEscaped());
  }

  html += metadataParts.join(" | ");
  html += "</div>";

  // Render each section
  for (const Section &section : sections) {
    html += renderSection(section);
  }

  html += "</body></html>";
  return html;
}

QString PaperModel::renderSection(const Section &section) const {
  QString sectionHtml = "<div class=\"section\">";

  // Section label (centered heading)
  if (!section.label.isEmpty()) {
    sectionHtml += "<h2>" % section.label.toHtmlEscaped() % "</h2>";
  }

  // Section subtitle (if present)
  if (!section.subtitle.isEmpty()) {
    sectionHtml += "<div class=\"subtitle\">" %
                   section.subtitle.toHtmlEscaped() % "</div>";
  }

  // Render questions
  int questionNumber = 1;
  for (const Question &question : section.questions) {
    sectionHtml += renderQuestion(question, questionNumber);
    ++questionNumber;
  }

  sectionHtml += "</div>";
  return sectionHtml;
}

QString PaperModel::renderQuestion(const Question &question,
                                   int questionNumber) const {
  QString questionHtml = "<div class=\"question\">";

  // Use a tiny layout table to keep number and text in same line
  questionHtml += "<table class=\"question-layout\"><tr>"
                  "<td class=\"question-num-cell\">" %
                  QString::number(questionNumber) %
                  ")</td>"
                  "<td class=\"question-text-cell\">" %
                  question.text % "</td></tr></table>";

  // Handle OR-type questions
  if (question.type == QuestionType::Or && !question.subQuestions.isEmpty()) {
    questionHtml += " <i>(Answer any one)</i><br/>";

    int subQuestionNumber = 1;
    for (const Question &subQuestion : question.subQuestions) {
      questionHtml += "<div class=\"or-question\">"
                      "<b>(" %
                      QString::number(subQuestionNumber) % ")</b> " %
                      subQuestion.text.toHtmlEscaped() % "</div>";
      ++subQuestionNumber;
    }
  }
  // Handle MCQ-type questions
  else if (question.type == QuestionType::Mcq && !question.options.isEmpty()) {
    questionHtml += "<table class=\"mcq-table\">";
    for (int i = 0; i < question.options.size(); i += 2) {
      questionHtml += "<tr>";

      // First column (a, c, ...)
      const QChar label1 = QChar('a' + i);
      questionHtml += "<td>(" % label1 % ") " %
                      question.options[i].toHtmlEscaped() % "</td>";

      // Second column (b, d, ...)
      if (i + 1 < question.options.size()) {
        const QChar label2 = QChar('a' + i + 1);
        questionHtml += "<td>(" % label2 % ") " %
                        question.options[i + 1].toHtmlEscaped() % "</td>";
      } else {
        questionHtml += "<td></td>";
      }

      questionHtml += "</tr>";
    }
    questionHtml += "</table>";
  }
  // Handle Mixed-type questions (render as MCQ if options present)
  else if (question.type == QuestionType::Mixed &&
           !question.options.isEmpty()) {
    questionHtml += "<div class=\"mcq-options\">";
    for (int i = 0; i < question.options.size(); ++i) {
      const QChar optionLabel = QChar('a' + i);
      questionHtml += "(" % optionLabel % ") " %
                      question.options[i].toHtmlEscaped() % "<br/>";
    }
    questionHtml += "</div>";
  }

  // Embed diagram if present
  if (!question.diagramPath.isEmpty()) {
    questionHtml += "<img src=\"file://" % question.diagramPath %
                    "\" "
                    "alt=\"Question diagram\" />";
  }

  // Render table if present
  if (!question.table.isEmpty()) {
    questionHtml += renderTable(question.table);
  }

  questionHtml += "</div>";
  return questionHtml;
}

QString PaperModel::renderTable(const QVector<QVector<QString>> &table) const {
  if (table.isEmpty()) {
    return QString();
  }

  QString tableHtml = "<table>";

  for (int row = 0; row < table.size(); ++row) {
    tableHtml += "<tr>";

    const auto &rowData = table[row];
    const QString cellTag = (row == 0) ? "th" : "td"; // First row as header

    for (const QString &cell : rowData) {
      tableHtml +=
          "<" % cellTag % ">" % cell.toHtmlEscaped() % "</" % cellTag % ">";
    }

    tableHtml += "</tr>";
  }

  tableHtml += "</table>";
  return tableHtml;
}

bool PaperModel::isValid() const {
  return !exam.title.isEmpty() && !sections.isEmpty();
}

int PaperModel::getTotalQuestions() const {
  int total = 0;
  for (const Section &section : sections) {
    total += section.questions.size();
  }
  return total;
}

void PaperModel::clear() {
  exam = Exam();
  sections.clear();
}