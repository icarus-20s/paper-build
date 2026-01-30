#include "MainWindow.h"
#include "../dialogs/ExamInfoDialog.h"
#include "../exporters/DocxExporter.h"
#include "../exporters/PdfExporter.h"
#include "../models/PaperModel.h"
#include "../pages/question_editor/QuestionEditorPage.h"
#include "ui_MainWindow.h"
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontComboBox>
#include <QGraphicsOpacityEffect>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTextBrowser>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <QVector>

/**
 * @file MainWindow.cpp
 * @brief Implementation of the MainWindow class.
 */

// Constants
namespace {
const QString APP_NAME = "Exam Paper Generator";
const QString APP_VERSION = "1.0.0";
const QString ORGANIZATION_NAME = "YourOrganization";
const QString DEFAULT_FONT_FAMILY = "Times New Roman";
constexpr int DEFAULT_FONT_SIZE = 12;
constexpr int ANIMATION_DURATION = 300;

const QString PAPER_FILE_FILTER =
    QObject::tr("Exam Paper Files (*.epf);;All Files (*)");
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_tabWidget(nullptr),
      m_paperModel(nullptr), m_sectionsLayout(nullptr),
      m_previewBrowser(nullptr), m_themeCombo(nullptr),
      m_contentModified(false), m_defaultFontFamily(DEFAULT_FONT_FAMILY),
      m_defaultFontSize(DEFAULT_FONT_SIZE), m_portraitOrientation(true) {
  ui->setupUi(this);

  // Create paper model
  m_paperModel = new PaperModel();

  setupUi();
  setupPages();
  setupMenuBar();
  setupToolBar();
  setupStatusBar();
  setupConnections();

  loadSettings();
  updateWindowTitle();
  updateUiState();

  // Show editor tab first
  showQuestionEditorPage();
}

MainWindow::~MainWindow() {
  saveSettings();
  delete m_paperModel;
  delete ui;
}

void MainWindow::setupUi() {
  // Set window properties
  setWindowTitle(APP_NAME);
  setWindowIcon(QIcon::fromTheme("application-x-exam"));
  resize(1200, 800);
  setMinimumSize(800, 600);

  // Create central tab widget
  m_tabWidget = new QTabWidget(this);
  setCentralWidget(m_tabWidget);
}

void MainWindow::setupPages() {
  // Create editor tab
  setupEditorTab();

  // Create preview tab
  setupPreviewTab();
}

void MainWindow::setupEditorTab() {
  m_questionEditorPage = new QuestionEditorPage(this);
  m_questionEditorPage->setDefaultFont(m_defaultFontFamily, m_defaultFontSize);

  // Connect signals
  connect(m_questionEditorPage, &QuestionEditorPage::contentChanged, this,
          &MainWindow::onContentChanged);
  connect(m_questionEditorPage, &QuestionEditorPage::backClicked, this,
          &MainWindow::onPreviousPage);
  connect(m_questionEditorPage, &QuestionEditorPage::nextClicked, this,
          &MainWindow::onNextPage);

  m_tabWidget->addTab(m_questionEditorPage, tr("Editor"));
}

void MainWindow::setupPreviewTab() {
  QWidget *previewTab = new QWidget();
  QVBoxLayout *previewLayout = new QVBoxLayout(previewTab);

  // Preview browser
  m_previewBrowser = new QTextBrowser();
  m_previewBrowser->setReadOnly(true);
  previewLayout->addWidget(m_previewBrowser);

  // Export buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  QPushButton *exportDocxButton = new QPushButton("Export DOCX");
  QPushButton *exportPdfButton = new QPushButton("Export PDF");
  QPushButton *exportHtmlButton = new QPushButton("Export HTML");
  buttonLayout->addWidget(exportDocxButton);
  buttonLayout->addWidget(exportPdfButton);
  buttonLayout->addWidget(exportHtmlButton);
  buttonLayout->addStretch();
  previewLayout->addLayout(buttonLayout);

  connect(exportDocxButton, &QPushButton::clicked, this,
          &MainWindow::onExportDocx);
  connect(exportPdfButton, &QPushButton::clicked, this,
          &MainWindow::onExportPdf);
  connect(exportHtmlButton, &QPushButton::clicked, this,
          &MainWindow::onExportHtml);

  m_tabWidget->addTab(previewTab, tr("Preview"));

  connect(m_tabWidget, &QTabWidget::currentChanged, this,
          &MainWindow::onTabChanged);
}

void MainWindow::setupMenuBar() {
  // File menu
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

  QAction *newAction =
      fileMenu->addAction(QIcon::fromTheme("document-new"), tr("&New"));
  newAction->setShortcut(QKeySequence::New);
  connect(newAction, &QAction::triggered, this, &MainWindow::onNewPaper);

  QAction *openAction =
      fileMenu->addAction(QIcon::fromTheme("document-open"), tr("&Open..."));
  openAction->setShortcut(QKeySequence::Open);
  connect(openAction, &QAction::triggered, this, &MainWindow::onOpenPaper);

  QAction *saveAction =
      fileMenu->addAction(QIcon::fromTheme("document-save"), tr("&Save"));
  saveAction->setShortcut(QKeySequence::Save);
  connect(saveAction, &QAction::triggered, this, &MainWindow::onSavePaper);

  QAction *saveAsAction = fileMenu->addAction(
      QIcon::fromTheme("document-save-as"), tr("Save &As..."));
  saveAsAction->setShortcut(QKeySequence::SaveAs);
  connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAsPaper);

  fileMenu->addSeparator();

  QAction *exitAction =
      fileMenu->addAction(QIcon::fromTheme("application-exit"), tr("E&xit"));
  exitAction->setShortcut(QKeySequence::Quit);
  connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

  // View menu
  QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

  QAction *examInfoAction = viewMenu->addAction(tr("Exam &Information"));
  examInfoAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_1));
  connect(examInfoAction, &QAction::triggered, this,
          &MainWindow::showExamInfoDialog);

  QAction *editorAction = viewMenu->addAction(tr("Question &Editor"));
  editorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_2));
  connect(editorAction, &QAction::triggered, this,
          &MainWindow::showQuestionEditorPage);

  QAction *previewAction = viewMenu->addAction(tr("&Preview"));
  previewAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_3));
  connect(previewAction, &QAction::triggered, this,
          &MainWindow::showPreviewPage);

  // Settings menu
  QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));

  QAction *preferencesAction = settingsMenu->addAction(
      QIcon::fromTheme("preferences-system"), tr("&Preferences..."));
  connect(preferencesAction, &QAction::triggered, this,
          &MainWindow::onShowSettings);

  // Paper orientation submenu
  QMenu *orientationMenu = settingsMenu->addMenu(tr("Paper &Orientation"));
  QAction *portraitAction = orientationMenu->addAction(tr("&Portrait"));
  portraitAction->setCheckable(true);
  portraitAction->setChecked(true); // Default
  connect(portraitAction, &QAction::triggered, this,
          [this]() { setPaperOrientation(true); });

  QAction *landscapeAction = orientationMenu->addAction(tr("&Landscape"));
  landscapeAction->setCheckable(true);
  connect(landscapeAction, &QAction::triggered, this,
          [this]() { setPaperOrientation(false); });

  QActionGroup *orientationGroup = new QActionGroup(this);
  orientationGroup->addAction(portraitAction);
  orientationGroup->addAction(landscapeAction);

  // Help menu
  QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

  QAction *aboutAction =
      helpMenu->addAction(QIcon::fromTheme("help-about"), tr("&About"));
  connect(aboutAction, &QAction::triggered, this, &MainWindow::onShowAbout);
}

void MainWindow::setupToolBar() {
  QToolBar *toolbar = addToolBar(tr("Main Toolbar"));
  toolbar->setMovable(false);
  toolbar->setIconSize(QSize(24, 24));

  // Navigation actions
  toolbar->addAction(QIcon::fromTheme("document-new"), tr("New"), this,
                     &MainWindow::onNewPaper);
  toolbar->addAction(QIcon::fromTheme("document-open"), tr("Open"), this,
                     &MainWindow::onOpenPaper);
  toolbar->addAction(QIcon::fromTheme("document-save"), tr("Save"), this,
                     &MainWindow::onSavePaper);

  toolbar->addSeparator();

  // Theme selector
  toolbar->addWidget(new QLabel(tr(" Theme: ")));
  m_themeCombo = new QComboBox();
  m_themeCombo->addItems({"Fusion", "Windows", "Macintosh", "Dark", "Light"});
  m_themeCombo->setToolTip(tr("Select application theme"));
  connect(m_themeCombo, &QComboBox::currentTextChanged, this,
          &MainWindow::onThemeChanged);
  toolbar->addWidget(m_themeCombo);

  toolbar->addSeparator();

  // Font controls
  toolbar->addWidget(new QLabel(tr(" Font: ")));
  QFontComboBox *fontCombo = new QFontComboBox();
  fontCombo->setCurrentFont(QFont(m_defaultFontFamily));
  fontCombo->setMaximumWidth(200);
  fontCombo->setToolTip(tr("Select default font for questions"));
  connect(fontCombo, &QFontComboBox::currentFontChanged, this,
          &MainWindow::onFontFamilyChanged);
  toolbar->addWidget(fontCombo);

  toolbar->addWidget(new QLabel(tr(" Size: ")));
  QSpinBox *fontSizeSpin = new QSpinBox();
  fontSizeSpin->setRange(8, 24);
  fontSizeSpin->setValue(m_defaultFontSize);
  fontSizeSpin->setSuffix(tr(" pt"));
  fontSizeSpin->setToolTip(tr("Select default font size"));
  connect(fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &MainWindow::onFontSizeChanged);
  toolbar->addWidget(fontSizeSpin);

  toolbar->addSeparator();

  // Add stretch to push everything to the left
  QWidget *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  toolbar->addWidget(spacer);
}

void MainWindow::setupStatusBar() {
  statusBar()->showMessage(tr("Ready"), 3000);
}

void MainWindow::setupConnections() {
  // No page connections for now
}

void MainWindow::showExamInfoDialog() {
  ExamInfoDialog dialog(this);
  dialog.setExam(m_paperModel->exam);
  if (dialog.exec() == QDialog::Accepted) {
    m_paperModel->exam = dialog.getExam();
    m_contentModified = true;
    updateWindowTitle();
    // Update preview if needed
    // m_previewPage->refreshPreview();
  }
}

void MainWindow::showQuestionEditorPage() {
  navigateToPage(0);
  updateStatus(tr("Create sections and questions"), 0);
}

void MainWindow::showPreviewPage() {
  navigateToPage(1);
  updateStatus(tr("Preview and export"), 0);
}

void MainWindow::onNextPage() {
  QString errorMessage;
  if (!validateCurrentPage(errorMessage)) {
    showError(tr("Validation Error"), errorMessage);
    return;
  }

  int currentIndex = getCurrentPageIndex();

  switch (currentIndex) {
  case 0: // Editor -> Preview
    showPreviewPage();
    break;
  default:
    break;
  }
}

void MainWindow::onPreviousPage() {
  int currentIndex = getCurrentPageIndex();

  switch (currentIndex) {
  case 1: // Preview -> Editor
    showQuestionEditorPage();
    break;
  default:
    break;
  }
}

void MainWindow::navigateToPage(int pageIndex) {
  if (pageIndex < 0 || pageIndex >= m_tabWidget->count()) {
    return;
  }

  m_tabWidget->setCurrentIndex(pageIndex);
  updateUiState();
}

int MainWindow::getCurrentPageIndex() const {
  return m_tabWidget->currentIndex();
}

void MainWindow::updatePaperModel() {
  if (m_questionEditorPage && m_paperModel) {
    m_paperModel->sections = m_questionEditorPage->getSections();
  }
}

bool MainWindow::validateCurrentPage(QString &errorMessage) {
  Q_UNUSED(errorMessage);
  return true; // Always valid for now
}

void MainWindow::onContentChanged() {
  m_contentModified = true;
  updateWindowTitle();
}

void MainWindow::onThemeChanged(const QString &theme) { applyTheme(theme); }

void MainWindow::applyTheme(const QString &theme) {
  if (theme == "Dark") {
    qApp->setStyleSheet(
        "QWidget { background-color: #1e1e1e; color: #e0e0e0; font-family: "
        "'Segoe UI', sans-serif; }"
        "QMainWindow { background-color: #121212; }"
        "QTabWidget::pane { border: 1px solid #333; top: -1px; background: "
        "#1e1e1e; border-radius: 4px; }"
        "QTabBar::tab { background: #252526; border: 1px solid #333; padding: "
        "10px 20px; border-top-left-radius: 4px; border-top-right-radius: 4px; "
        "margin-right: 2px; }"
        "QTabBar::tab:selected { background: #007acc; color: white; "
        "border-bottom-color: #007acc; }"
        "QPushButton { background-color: #333; border: none; padding: 8px "
        "16px; border-radius: 4px; color: #e0e0e0; font-weight: bold; }"
        "QPushButton:hover { background-color: #444; }"
        "QPushButton:pressed { background-color: #007acc; }"
        "QLineEdit, QTextEdit, QComboBox, QSpinBox { background-color: "
        "#2d2d2d; border: 1px solid #3f3f3f; padding: 5px; border-radius: 3px; "
        "selection-background-color: #007acc; }"
        "QGroupBox { border: 1px solid #333; margin-top: 15px; font-weight: "
        "bold; border-radius: 5px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 "
        "3px; }"
        "QScrollBar:vertical { border: none; background: #1e1e1e; width: 10px; "
        "margin: 0px; }"
        "QScrollBar::handle:vertical { background: #333; min-height: 20px; "
        "border-radius: 5px; }"
        "QScrollBar::handle:vertical:hover { background: #444; }");
  } else if (theme == "Light") {
    qApp->setStyleSheet(
        "QWidget { background-color: #f5f5f5; color: #333; font-family: 'Segoe "
        "UI', sans-serif; }"
        "QMainWindow { background-color: #ffffff; }"
        "QTabWidget::pane { border: 1px solid #ddd; top: -1px; background: "
        "white; border-radius: 4px; }"
        "QTabBar::tab { background: #e1e1e1; border: 1px solid #ddd; padding: "
        "10px 20px; border-top-left-radius: 4px; border-top-right-radius: 4px; "
        "margin-right: 2px; }"
        "QTabBar::tab:selected { background: #007acc; color: white; "
        "border-bottom-color: #007acc; }"
        "QPushButton { background-color: #007acc; border: none; padding: 8px "
        "16px; border-radius: 4px; color: white; font-weight: bold; }"
        "QPushButton:hover { background-color: #005a9e; }"
        "QPushButton:pressed { background-color: #004a80; }"
        "QLineEdit, QTextEdit, QComboBox, QSpinBox { background-color: white; "
        "border: 1px solid #ccc; padding: 5px; border-radius: 3px; }"
        "QGroupBox { border: 1px solid #ddd; margin-top: 15px; font-weight: "
        "bold; border-radius: 5px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 "
        "3px; }");
  } else {
    qApp->setStyle(QStyleFactory::create(theme));
    qApp->setStyleSheet("");
  }
}

void MainWindow::onFontFamilyChanged(const QFont &font) {
  m_defaultFontFamily = font.family();
  if (m_questionEditorPage) {
    m_questionEditorPage->setDefaultFont(m_defaultFontFamily,
                                         m_defaultFontSize);
  }
  onContentChanged();
}

void MainWindow::onFontSizeChanged(int size) {
  m_defaultFontSize = size;
  if (m_questionEditorPage) {
    m_questionEditorPage->setDefaultFont(m_defaultFontFamily,
                                         m_defaultFontSize);
  }
  onContentChanged();
}

void MainWindow::onNewPaper() {
  if (!checkUnsavedChanges()) {
    return;
  }

  delete m_paperModel;
  m_paperModel = new PaperModel();

  if (m_questionEditorPage) {
    m_questionEditorPage->setSections(m_paperModel->sections);
  }

  m_currentFilePath.clear();
  m_contentModified = false;
  showQuestionEditorPage();
  updateWindowTitle();
  updateStatus(tr("New exam paper created"), 3000);
}

void MainWindow::onOpenPaper() {
  if (!checkUnsavedChanges()) {
    return;
  }

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open Exam Paper"),
                                                  "", PAPER_FILE_FILTER);
  if (!filePath.isEmpty()) {
    if (loadPaperFromFile(filePath)) {
      m_currentFilePath = filePath;
      m_contentModified = false;
      updateWindowTitle();
      updateStatus(tr("Opened: %1").arg(filePath), 3000);
    }
  }
}

void MainWindow::onSavePaper() {
  if (m_currentFilePath.isEmpty()) {
    onSaveAsPaper();
    return;
  }
  if (savePaperToFile(m_currentFilePath)) {
    m_contentModified = false;
    updateWindowTitle();
    updateStatus(tr("Saved: %1").arg(m_currentFilePath), 3000);
  }
}

void MainWindow::onSaveAsPaper() {
  QString filePath = QFileDialog::getSaveFileName(this, tr("Save Exam Paper"),
                                                  "", PAPER_FILE_FILTER);
  if (!filePath.isEmpty()) {
    if (!filePath.endsWith(".epf", Qt::CaseInsensitive))
      filePath += ".epf";
    if (savePaperToFile(filePath)) {
      m_currentFilePath = filePath;
      m_contentModified = false;
      updateWindowTitle();
      updateStatus(tr("Saved as: %1").arg(filePath), 3000);
    }
  }
}

bool MainWindow::loadPaperFromFile(const QString &filePath) {
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    showError(tr("Load Error"),
              tr("Failed to open file: %1").arg(file.errorString()));
    return false;
  }

  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if (doc.isNull() || !doc.isObject()) {
    showError(tr("Load Error"), tr("Invalid file format."));
    return false;
  }

  // TODO: Implement proper JSON deserialization
  // For now, just show a message
  showInfo(tr("Load"), tr("File loaded successfully (implementation pending)"));

  // In a real app, you'd deserialize the JSON into sections
  // For now, let's assume it's done and update the editor
  if (m_questionEditorPage) {
    m_questionEditorPage->setSections(m_paperModel->sections);
  }
  return true;
}

bool MainWindow::savePaperToFile(const QString &filePath) {
  updatePaperModel();

  QJsonObject json;
  json["version"] = APP_VERSION;
  json["exam"] = QJsonObject();    // Add exam data
  json["sections"] = QJsonArray(); // Add sections data
  // Serialize model here...

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) {
    showError(tr("Save Error"),
              tr("Failed to save file: %1").arg(file.errorString()));
    return false;
  }
  file.write(QJsonDocument(json).toJson());
  file.close();
  return true;
}

void MainWindow::onShowSettings() {
  showInfo(tr("Settings"), tr("Settings dialog coming soon!"));
}

void MainWindow::onShowAbout() {
  QMessageBox::about(this, tr("About %1").arg(APP_NAME),
                     tr("<h2>%1</h2><p>Version %2</p><p>Professional Exam "
                        "Paper Generator.</p>")
                         .arg(APP_NAME)
                         .arg(APP_VERSION));
}

void MainWindow::updateWindowTitle() {
  QString title = APP_NAME;
  if (!m_currentFilePath.isEmpty())
    title = QFileInfo(m_currentFilePath).fileName() + " - " + APP_NAME;
  if (m_contentModified)
    title = "* " + title;
  setWindowTitle(title);
}

void MainWindow::updateUiState() {}

void MainWindow::updateStatus(const QString &message, int timeout) {
  statusBar()->showMessage(message, timeout);
}

bool MainWindow::checkUnsavedChanges() {
  if (!m_contentModified)
    return true;
  return confirmAction(tr("Unsaved Changes"),
                       tr("You have unsaved changes. Discard?"));
}

void MainWindow::showError(const QString &title, const QString &message) {
  QMessageBox::critical(this, title, message);
}
void MainWindow::showInfo(const QString &title, const QString &message) {
  QMessageBox::information(this, title, message);
}
bool MainWindow::confirmAction(const QString &title, const QString &message) {
  return QMessageBox::question(this, title, message,
                               QMessageBox::Yes | QMessageBox::No) ==
         QMessageBox::Yes;
}

void MainWindow::loadSettings() {
  QSettings settings(ORGANIZATION_NAME, APP_NAME);
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
  QString theme = settings.value("theme", "Dark").toString();
  int idx = m_themeCombo->findText(theme);
  if (idx >= 0) {
    m_themeCombo->setCurrentIndex(idx);
    applyTheme(theme);
  }
  m_defaultFontFamily =
      settings.value("fontFamily", DEFAULT_FONT_FAMILY).toString();
  m_defaultFontSize = settings.value("fontSize", DEFAULT_FONT_SIZE).toInt();
  m_portraitOrientation = settings.value("paperOrientation", "portrait").toString() == "portrait";
}

void MainWindow::saveSettings() {
  QSettings settings(ORGANIZATION_NAME, APP_NAME);
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  settings.setValue("theme", m_themeCombo->currentText());
  settings.setValue("fontFamily", m_defaultFontFamily);
  settings.setValue("fontSize", m_defaultFontSize);
  settings.setValue("paperOrientation", m_portraitOrientation ? "portrait" : "landscape");
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (checkUnsavedChanges()) {
    saveSettings();
    event->accept();
  } else
    event->ignore();
}

void MainWindow::onTabChanged(int index) {
  if (index == 1)
    updatePreview();
}

void MainWindow::updatePreview() {
  if (m_previewBrowser) {
    updatePaperModel();
    m_previewBrowser->setHtml(
        m_paperModel->toHtml(m_defaultFontFamily, m_defaultFontSize, m_portraitOrientation));
  }
}

void MainWindow::onExportDocx() {
  QString filePath = QFileDialog::getSaveFileName(this, tr("Export DOCX"), "",
                                                  "DOCX Files (*.docx)");
  if (!filePath.isEmpty()) {
    updatePaperModel();
    DocxExporter exporter;
    if (exporter.exportToDocx(*m_paperModel, filePath, m_defaultFontFamily, m_defaultFontSize, m_portraitOrientation))
      QMessageBox::information(this, tr("Success"), tr("DOCX exported."));
    else
      QMessageBox::warning(this, tr("Error"), tr("Export failed."));
  }
}

void MainWindow::onExportPdf() {
  QString filePath = QFileDialog::getSaveFileName(this, tr("Export PDF"), "",
                                                  "PDF Files (*.pdf)");
  if (!filePath.isEmpty()) {
    updatePaperModel();
    PdfExporter exporter;
    if (exporter.exportToPdf(*m_paperModel, filePath, m_defaultFontFamily, m_defaultFontSize, m_portraitOrientation))
      QMessageBox::information(this, tr("Success"), tr("PDF exported."));
    else
      QMessageBox::warning(this, tr("Error"), tr("Export failed."));
  }
}

void MainWindow::onExportHtml() {
  QString filePath = QFileDialog::getSaveFileName(this, tr("Export HTML"), "",
                                                  "HTML Files (*.html)");
  if (!filePath.isEmpty()) {
    updatePaperModel();
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      out.setEncoding(QStringConverter::Utf8);
      out << m_paperModel->toHtml(m_defaultFontFamily, m_defaultFontSize, m_portraitOrientation);
      file.close();
      QMessageBox::information(this, tr("Success"), tr("HTML exported."));
    } else {
      QMessageBox::warning(this, tr("Export Failed"),
                           tr("Failed to export HTML."));
    }
  }
}

void MainWindow::setPaperOrientation(bool portrait) {
  m_portraitOrientation = portrait;
  
  // Update preview with new orientation
  updatePreview();
  
  // Save setting
  QSettings settings(ORGANIZATION_NAME, APP_NAME);
  settings.setValue("paperOrientation", portrait ? "portrait" : "landscape");
  
  updateStatus(portrait ? tr("Switched to portrait mode") : tr("Switched to landscape mode"));
}