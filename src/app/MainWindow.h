#pragma once

#include <QMainWindow>
#include <QString>
#include <QTextBrowser>
#include <QVBoxLayout>

// Forward declarations
namespace Ui {
class MainWindow;
}

class PaperModel;
class ExamInfoDialog;
class QTabWidget;
class Question;
class Section;
class QComboBox;
class QuestionEditorPage;

/**
 * @file MainWindow.h
 * @brief Defines the MainWindow class for the Exam Paper Generator application.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void showExamInfoDialog();
  void showQuestionEditorPage();
  void showPreviewPage();
  void onNextPage();
  void onPreviousPage();
  void onThemeChanged(const QString &theme);
  void onFontFamilyChanged(const QFont &font);
  void onFontSizeChanged(int size);
  void updatePreview();
  void onExportDocx();
  void onExportPdf();
  void onExportHtml();
  void onNewPaper();
  void onOpenPaper();
  void onSavePaper();
  void onSaveAsPaper();
  void onShowSettings();
  void onShowAbout();
  void setPaperOrientation(bool portrait);
  void setupEditorTab();
  void setupPreviewTab();
  void onTabChanged(int index);
  void updatePaperModel();
  void onContentChanged();

private:
  Ui::MainWindow *ui;
  QTabWidget *m_tabWidget;
  PaperModel *m_paperModel;
  QuestionEditorPage *m_questionEditorPage;
  QVBoxLayout *m_sectionsLayout;
  QTextBrowser *m_previewBrowser;
  QComboBox *m_themeCombo;
  QString m_currentFilePath;
  bool m_contentModified;
  QString m_defaultFontFamily;
  int m_defaultFontSize;
  bool m_portraitOrientation;

  void setupUi();
  void setupPages();
  void setupMenuBar();
  void setupToolBar();
  void setupStatusBar();
  void setupConnections();
  void applyTheme(const QString &theme);
  void updateWindowTitle();
  void updateUiState();
  bool checkUnsavedChanges();
  bool loadPaperFromFile(const QString &filePath);
  bool savePaperToFile(const QString &filePath);
  int getCurrentPageIndex() const;
  void navigateToPage(int pageIndex);
  bool validateCurrentPage(QString &errorMessage);
  void updateStatus(const QString &message, int timeout = 0);
  void showError(const QString &title, const QString &message);
  void showInfo(const QString &title, const QString &message);
  bool confirmAction(const QString &title, const QString &message);
  void loadSettings();
  void saveSettings();
};