#include "models/Exam.h"
#include "models/PaperModel.h"
#include "models/Question.h"
#include "models/Section.h"
#include <QDebug>
#include <QString>
#include <QVector>
#include <iostream>

// Simple assertion helper
bool assertContains(const QString &haystack, const QString &needle,
                    const char *testName) {
  if (haystack.contains(needle)) {
    std::cout << "[PASS] " << testName << std::endl;
    return true;
  } else {
    std::cout << "[FAIL] " << testName
              << "\nExpected to find: " << needle.toStdString()
              << "\nIn: " << haystack.toStdString().substr(0, 500) << "..."
              << std::endl;
    return false;
  }
}

int main() {
  std::cout << "Running Layout Tests..." << std::endl;

  // Test 1: MCQ Layout
  {
    std::cout << "\nTest 1: MCQ Layout" << std::endl;
    Question q;
    q.type = QuestionType::Mcq;
    q.text = "Testing MCQ";
    q.options = {"Option A", "Option B", "Option C", "Option D"};

    Section s;
    s.label = "Section A";
    s.questions.append(q);

    PaperModel model;
    model.exam.title = "Test Exam";
    model.sections.append(s);

    QString html = model.toHtml();

    // Check for mcq-table class
    assertContains(html, "class=\"mcq-table\"", "MCQ Table Class present");

    // Check for no border logic (in CSS) - we check if CSS is injected
    assertContains(html, ".mcq-table { width: 95%; border: none;",
                   "MCQ CSS injected");

    // Check for 2-column formatting
    assertContains(html, "<td width=\"50%\">(a) Option A</td>",
                   "Option A formatted correctly");
    assertContains(html, "<td width=\"50%\">(b) Option B</td>",
                   "Option B formatted correctly");
  }

  // Test 2: Floating Image
  {
    std::cout << "\nTest 2: Floating Image" << std::endl;
    Question q;
    q.type = QuestionType::Regular;
    q.text = "Image Question";
    q.diagramPath = "/tmp/test.png";

    Section s;
    s.questions.append(q);

    PaperModel model;
    model.sections.append(s);

    QString html = model.toHtml();

    // Check for question-image class
    assertContains(html, "class=\"question-image\"", "Image Class present");

    // Check positioning: Float content should appear BEFORE text in HTML for it
    // to float right correctly? My implementation: floatedContent +
    // question.text. So <img ...> should actally be inside text cell but BEFORE
    // text? Wait, my implementation was:
    // "<td class=\"question-text-cell\">" % floatedContent % question.text %
    // "</td>"

    // Verify Image is AFTER Text (since we append floated content to float it
    // right/below)
    int imgPos = html.indexOf("class=\"question-image\"");
    int textPos = html.indexOf("Image Question");
    if (imgPos != -1 && textPos != -1 && imgPos > textPos) {
      std::cout
          << "[PASS] Image appears after text (correct for floating below)"
          << std::endl;
    } else {
      std::cout << "[FAIL] Image order incorrect. Img: " << imgPos
                << " Text: " << textPos << std::endl;
    }
  }

  // Test 3: Floating Table
  {
    std::cout << "\nTest 3: Floating Table" << std::endl;
    Question q;
    q.text = "Table Question";
    QVector<QString> row;
    row << "Col 1" << "Col 2";
    q.table.append(row);

    Section s;
    s.questions.append(q);

    PaperModel model;
    model.sections.append(s);

    QString html = model.toHtml();

    // Check for data-table class
    assertContains(html, "class=\"data-table\"", "Data Table Class present");
  }

  // Test 4: OR Layout
  {
    std::cout << "\nTest 4: OR Layout" << std::endl;
    Question q;
    q.type = QuestionType::Or;
    q.text = "Main Question";

    Question altQ;
    altQ.text = "Alternative Question";
    q.subQuestions.append(altQ);

    Section s;
    s.questions.append(q);

    PaperModel model;
    model.sections.append(s);

    QString html = model.toHtml();

    // Check for OR separator with styling
    assertContains(html, ">OR</div>", "OR separator present");

    // Check for both texts
    assertContains(html, "Main Question", "Main Question text present");
    assertContains(html, "Alternative Question",
                   "Alternative Question text present");
  }

  return 0;
}
