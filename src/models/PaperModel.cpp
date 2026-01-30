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
                                     "@page { "
                                     "size: A4 %1; "
                                     "margin: 15mm; "
                                     "}"
                                     "@media print { "
                                     "body { "
                                     "font-family:'%2', serif; "
                                     "font-size:%3pt; "
                                     "margin:0; "
                                     "line-height:1.4; "
                                     "max-width:100%; "
                                     "}"
                                     "}"
                                     "body { "
                                     "font-family:'%2', serif; "
                                     "font-size:%3pt; "
                                     "margin:10px; "
                                     "line-height:1.4; "
                                     "max-width:100%; "
                                     "box-sizing:border-box; "
                                     "}"
                                     "p { "
                                     "margin:0; "
                                     "padding:0; "
                                     "}"
                                     "h1 { "
                                     "text-align:center; "
                                     "margin-bottom:6px; "
                                     "font-size:1.3em; "
                                     "font-weight:bold; "
                                     "}"
                                     "h2 { "
                                     "text-align:center; "
                                     "margin-top:12px; "
                                     "margin-bottom:3px; "
                                     "font-size:1.0em; "
                                     "font-weight:bold; "
                                     "}"
                                     ".metadata { "
                                     "text-align:center; "
                                     "margin-bottom:12px; "
                                     "font-size:0.8em; "
                                     "}"
                                     ".section { "
                                     "margin-top:12px; "
                                     "page-break-inside:avoid; "
                                     "}"
                                     ".subtitle { "
                                     "text-align:center; "
                                     "font-weight:bold; "
                                     "font-size:0.85em; "
                                     "margin-bottom:6px; "
                                     "font-style:italic; "
                                     "}"
                                     ".question { "
                                     "margin:2px 0; "
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
                                     "margin-top:2px; "
                                     "}"
                                     ".mcq-options { "
                                     "margin-left:15px; "
                                     "margin-top:1px; "
                                     "line-height:1.2; "
                                     "}"
                                     "table { "
                                     "border-collapse:collapse; "
                                     "width:100%; "
                                     "margin:3px 0; "
                                     "font-size:0.85em; "
                                     "}"
                                     "td, th { "
                                     "border:1px solid #000; "
                                     "padding:2px 4px; "
                                     "text-align:left; "
                                     "}"
                                     "th { "
                                     "background-color:#f5f5f5; "
                                     "font-weight:bold; "
                                     "}"
                                     "img { "
                                     "max-width:100%; "
                                     "height:auto; "
                                     "margin:2px 0; "
                                     "display:block; "
                                     "}"
                                     ".mcq-table { "
                                     "width: 95%; "
                                     "border: none; "
                                     "margin-left: 15px; "
                                     "margin-top: 5px; "
                                     "}"
                                     ".mcq-table td { "
                                     "border: none; "
                                     "padding: 2px 10px; "
                                     "vertical-align: top; "
                                     "}"
                                     ".data-table { "
                                     "float: right; "
                                     "width: auto; "
                                     "margin: 0 0 5px 15px; "
                                     "border: 1px solid #000; "
                                     "}"
                                     ".data-table td, .data-table th { "
                                     "border: 1px solid #000; "
                                     "}"
                                     ".question-image { "
                                     "margin: 5px; "
                                     "}"
                                     "</style>"
                                     "</head>"
                                     "<body>";
} // namespace

QString PaperModel::toHtml(const QString &fontFamily, int fontSize,
                           bool portrait) const {
  // Build HTML using QString builder for better performance
  QString html;
  html.reserve(4096); // Reserve initial capacity to reduce reallocations

  // HTML header with CSS
  QString orientation = portrait ? "portrait" : "landscape";
  html = QString(HTML_HEADER_TEMPLATE)
             .arg(orientation)
             .arg(fontFamily)
             .arg(fontSize)
             .arg(QUESTION_NUMBER_WIDTH)
             .arg(OR_INDENT);

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

  // Separator line
  html += "<hr style=\"border: 0; border-top: 2px solid #000; margin: 10px 0 "
          "20px 0;\" />";

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

  // Prepare floated content (Image + Data Table)
  QString floatedContent;

  // Embed diagram if present (floated right)
  if (!question.diagramPath.isEmpty()) {
    floatedContent += "<br/><img src=\"file://" % question.diagramPath %
                      "\" width=\"150\" align=\"right\" "
                      "class=\"question-image\" "
                      "alt=\"Question diagram\" />";
  }

  // Render table if present (floated right)
  if (!question.table.isEmpty()) {
    floatedContent += renderTable(question.table);
  }

  // Question Layout Table (Number | Text + Floats)
  questionHtml += "<table class=\"question-layout\"><tr>"
                  "<td class=\"question-num-cell\">" %
                  QString::number(questionNumber) %
                  ")</td>"
                  "<td class=\"question-text-cell\">" %
                  question.text % floatedContent % "</td></tr></table>";

  // Handle OR-type questions
  // Handle OR-type questions
  if (question.type == QuestionType::Or && !question.subQuestions.isEmpty()) {
    questionHtml += "<div style=\"text-align:center; font-weight:bold; margin: "
                    "5px 0;\">OR</div>";

    for (const Question &subQuestion : question.subQuestions) {
      questionHtml += "<div class=\"or-question\">" %
                      subQuestion.text.toHtmlEscaped() % "</div>";
    }
  }
  // Handle MCQ-type questions
  else if (question.type == QuestionType::Mcq && !question.options.isEmpty()) {
    questionHtml += "<div style=\"clear:both;\"></div>";
    questionHtml += "<table class=\"mcq-table\">";
    for (int i = 0; i < question.options.size(); i += 2) {
      questionHtml += "<tr>";

      // First column (a, c, ...)
      const QChar label1 = QChar('a' + i);
      questionHtml += "<td width=\"50%\">(" % label1 % ") " %
                      question.options[i].toHtmlEscaped() % "</td>";

      // Second column (b, d, ...)
      if (i + 1 < question.options.size()) {
        const QChar label2 = QChar('a' + i + 1);
        questionHtml += "<td width=\"50%\">(" % label2 % ") " %
                        question.options[i + 1].toHtmlEscaped() % "</td>";
      } else {
        questionHtml += "<td></td>";
      }

      questionHtml += "</tr>";
    }
    questionHtml += "</table>";
  }
  // Handle Mixed-type questions
  else if (question.type == QuestionType::Mixed &&
           !question.options.isEmpty()) {
    questionHtml += "<div style=\"clear:both;\"></div>";
    questionHtml += "<div class=\"mcq-options\">";
    for (int i = 0; i < question.options.size(); ++i) {
      const QChar optionLabel = QChar('a' + i);
      questionHtml += "(" % optionLabel % ") " %
                      question.options[i].toHtmlEscaped() % "<br/>";
    }
    questionHtml += "</div>";
  }

  questionHtml += "</div>";
  return questionHtml;
}

QString PaperModel::renderTable(const QVector<QVector<QString>> &table) const {
  if (table.isEmpty()) {
    return QString();
  }

  QString tableHtml = "<table class=\"data-table\">";

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