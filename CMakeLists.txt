cmake_minimum_required(VERSION 3.16)
project(QuestionPaperSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui PrintSupport)

# Collect sources (explicit lists are more maintainable)
set(SOURCES
    src/main.cpp
    src/app/MainWindow.cpp
    src/models/PaperModel.cpp
    src/exporters/DocxExporter.cpp
    src/exporters/PdfExporter.cpp
    src/dialogs/ExamInfoDialog.cpp
    src/widgets/questionWidget/QuestionWidget.cpp
    src/widgets/sectionWidget/SectionWidget.cpp
    src/pages/question_editor/QuestionEditorPage.cpp
    src/pages/exam_info/ExamInfoPage.cpp
    src/pages/preview/PreviewPage.cpp
)

set(HEADERS
    src/app/MainWindow.h
    src/models/Exam.h
    src/models/Section.h
    src/models/Question.h
    src/models/MCQ.h
    src/models/PaperModel.h
    src/exporters/DocxExporter.h
    src/exporters/PdfExporter.h
    src/utils/Constants.h
    src/utils/FileUtils.h
    src/utils/Validation.h
    src/dialogs/ExamInfoDialog.h
    src/widgets/questionWidget/QuestionWidget.h
    src/widgets/sectionWidget/SectionWidget.h
    src/pages/question_editor/QuestionEditorPage.h
    src/pages/exam_info/ExamInfoPage.h
    src/pages/preview/PreviewPage.h
)

set(UI_FILES
    src/app/MainWindow.ui
    src/widgets/questionWidget/QuestionWidget.ui
    src/widgets/sectionWidget/SectionWidget.ui
    src/pages/question_editor/QuestionEditorPage.ui
    src/pages/exam_info/ExamInfoPage.ui
    src/pages/preview/PreviewPage.ui
)

set(RESOURCE_FILES
    src/resources.qrc
)

# Create executable with Qt helper
qt_add_executable(question_paper_system
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

target_include_directories(question_paper_system PRIVATE src)

target_link_libraries(question_paper_system PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Qt6::PrintSupport)

set_target_properties(question_paper_system PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
