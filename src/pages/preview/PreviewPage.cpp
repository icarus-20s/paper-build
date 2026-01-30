#include "PreviewPage.h"
#include "ui_PreviewPage.h"
#include "../../models/PaperModel.h"
#include "../../exporters/DocxExporter.h"
#include "../../exporters/PdfExporter.h"
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextBrowser>
#include <QPrintDialog>
#include <QPrinter>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDateTime>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QRegularExpression>
#include <QStringConverter>

/**
 * @file PreviewPage.cpp
 * @brief Implementation of the PreviewPage class.
 */

// Constants
namespace {
    constexpr int DEFAULT_ZOOM = 100;
    constexpr int MIN_ZOOM = 50;
    constexpr int MAX_ZOOM = 200;
    constexpr int ZOOM_STEP = 10;
    
    const QString DOCX_FILTER = QObject::tr("Word Document (*.docx)");
    const QString PDF_FILTER = QObject::tr("PDF File (*.pdf)");
    const QString HTML_FILTER = QObject::tr("HTML File (*.html)");
    
    QString formatEnumToString(PreviewPage::ExportFormat format) {
        switch (format) {
            case PreviewPage::FormatDocx: return "DOCX";
            case PreviewPage::FormatPdf: return "PDF";
            case PreviewPage::FormatHtml: return "HTML";
            default: return "Unknown";
        }
    }
}

PreviewPage::PreviewPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PreviewPage)
    , m_model(nullptr)
    , m_zoomLevel(DEFAULT_ZOOM)
    , m_defaultExportDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
{
    ui->setupUi(this);
    setupUi();
    setupToolbar();
    setupConnections();
}

PreviewPage::~PreviewPage()
{
    delete ui;
}

void PreviewPage::setupUi()
{
    // Configure preview text browser
    ui->previewBrowser->setReadOnly(true);
    ui->previewBrowser->setOpenExternalLinks(false);
    ui->previewBrowser->setAcceptRichText(true);
    
    // Style navigation buttons
    ui->backButton->setMinimumHeight(35);
    ui->backButton->setIcon(QIcon::fromTheme("go-previous"));
    ui->backButton->setToolTip(tr("Go back to question editor"));
    
    // Style export buttons
    ui->exportDocxButton->setMinimumHeight(35);
    ui->exportDocxButton->setIcon(QIcon::fromTheme("application-vnd.ms-word"));
    ui->exportDocxButton->setToolTip(tr("Export as Microsoft Word document"));
    
    ui->exportPdfButton->setMinimumHeight(35);
    ui->exportPdfButton->setIcon(QIcon::fromTheme("application-pdf"));
    ui->exportPdfButton->setToolTip(tr("Export as PDF file"));
    
    ui->exportHtmlButton->setMinimumHeight(35);
    ui->exportHtmlButton->setIcon(QIcon::fromTheme("text-html"));
    ui->exportHtmlButton->setToolTip(tr("Export as HTML file"));
    
    ui->printButton->setMinimumHeight(35);
    ui->printButton->setIcon(QIcon::fromTheme("document-print"));
    ui->printButton->setToolTip(tr("Print the exam paper"));
    
    // Initially disable export/print buttons until model is set
    updateButtonStates();
}

void PreviewPage::setupToolbar()
{
    // Configure zoom slider
    ui->zoomSlider->setRange(MIN_ZOOM, MAX_ZOOM);
    ui->zoomSlider->setValue(DEFAULT_ZOOM);
    ui->zoomSlider->setTickPosition(QSlider::TicksBelow);
    ui->zoomSlider->setTickInterval(ZOOM_STEP);
    ui->zoomSlider->setToolTip(tr("Adjust preview zoom level"));
    
    // Configure zoom buttons
    ui->zoomInButton->setIcon(QIcon::fromTheme("zoom-in"));
    ui->zoomInButton->setToolTip(tr("Zoom in (Ctrl++)"));
    ui->zoomInButton->setShortcut(QKeySequence::ZoomIn);
    
    ui->zoomOutButton->setIcon(QIcon::fromTheme("zoom-out"));
    ui->zoomOutButton->setToolTip(tr("Zoom out (Ctrl+-)"));
    ui->zoomOutButton->setShortcut(QKeySequence::ZoomOut);
    
    ui->resetZoomButton->setIcon(QIcon::fromTheme("zoom-original"));
    ui->resetZoomButton->setToolTip(tr("Reset zoom to 100%"));
    
    updateZoomDisplay();
}

void PreviewPage::setupConnections()
{
    // Navigation
    connect(ui->backButton, &QPushButton::clicked, 
            this, &PreviewPage::onBackClicked);
    
    // Export buttons
    connect(ui->exportDocxButton, &QPushButton::clicked, 
            this, &PreviewPage::onExportDocx);
    connect(ui->exportPdfButton, &QPushButton::clicked, 
            this, &PreviewPage::onExportPdf);
    connect(ui->exportHtmlButton, &QPushButton::clicked, 
            this, &PreviewPage::onExportHtml);
    connect(ui->printButton, &QPushButton::clicked, 
            this, &PreviewPage::onPrint);
    
    // Zoom controls
    connect(ui->zoomInButton, &QPushButton::clicked, 
            this, &PreviewPage::onZoomIn);
    connect(ui->zoomOutButton, &QPushButton::clicked, 
            this, &PreviewPage::onZoomOut);
    connect(ui->resetZoomButton, &QPushButton::clicked, 
            this, &PreviewPage::onResetZoom);
    connect(ui->zoomSlider, &QSlider::valueChanged, 
            this, &PreviewPage::onZoomSliderChanged);
}

void PreviewPage::setPaperModel(PaperModel* model)
{
    m_model = model;
    refreshPreview();
    updateButtonStates();
}

PaperModel* PreviewPage::paperModel() const
{
    return m_model;
}

void PreviewPage::refreshPreview()
{
    if (!m_model) {
        ui->previewBrowser->setHtml(
            tr("<h3>No exam paper to preview</h3>"
               "<p>Please create an exam paper first.</p>")
        );
        return;
    }
    
    QString html = m_model->toHtml();
    ui->previewBrowser->setHtml(html);
    applyZoom();
}

void PreviewPage::setZoomLevel(int zoomPercent)
{
    m_zoomLevel = qBound(MIN_ZOOM, zoomPercent, MAX_ZOOM);
    ui->zoomSlider->blockSignals(true);
    ui->zoomSlider->setValue(m_zoomLevel);
    ui->zoomSlider->blockSignals(false);
    
    applyZoom();
    updateZoomDisplay();
    emit zoomChanged(m_zoomLevel);
}

int PreviewPage::getZoomLevel() const
{
    return m_zoomLevel;
}

void PreviewPage::applyZoom()
{
    QFont font = ui->previewBrowser->font();
    font.setPointSizeF(12.0 * m_zoomLevel / 100.0); // Base size 12
    ui->previewBrowser->setFont(font);
}

void PreviewPage::updateZoomDisplay()
{
    ui->zoomLabel->setText(tr("%1%").arg(m_zoomLevel));
}

void PreviewPage::setDefaultExportDirectory(const QString& directory)
{
    m_defaultExportDirectory = directory;
}

QString PreviewPage::getLastExportPath() const
{
    return m_lastExportPath;
}

void PreviewPage::setExportEnabled(bool enabled)
{
    ui->exportDocxButton->setEnabled(enabled && m_model != nullptr);
    ui->exportPdfButton->setEnabled(enabled && m_model != nullptr);
    ui->exportHtmlButton->setEnabled(enabled && m_model != nullptr);
    ui->printButton->setEnabled(enabled && m_model != nullptr);
}

bool PreviewPage::isReadyForExport() const
{
    QString errorMessage;
    return validateModelForExport(errorMessage);
}

void PreviewPage::updateButtonStates()
{
    bool hasModel = (m_model != nullptr);
    ui->exportDocxButton->setEnabled(hasModel);
    ui->exportPdfButton->setEnabled(hasModel);
    ui->exportHtmlButton->setEnabled(hasModel);
    ui->printButton->setEnabled(hasModel);
}

void PreviewPage::exportToDocx()
{
    onExportDocx();
}

void PreviewPage::exportToPdf()
{
    onExportPdf();
}

void PreviewPage::exportToHtml()
{
    onExportHtml();
}

void PreviewPage::printPaper()
{
    onPrint();
}

void PreviewPage::zoomIn()
{
    setZoomLevel(m_zoomLevel + ZOOM_STEP);
}

void PreviewPage::zoomOut()
{
    setZoomLevel(m_zoomLevel - ZOOM_STEP);
}

void PreviewPage::resetZoom()
{
    setZoomLevel(DEFAULT_ZOOM);
}

void PreviewPage::onBackClicked()
{
    emit backClicked();
}

void PreviewPage::onExportDocx()
{
    QString errorMessage;
    if (!validateModelForExport(errorMessage)) {
        showExportError(FormatDocx, errorMessage);
        return;
    }
    
    QString filePath = promptForExportPath(FormatDocx);
    if (filePath.isEmpty()) {
        return; // User cancelled
    }
    
    if (performExport(FormatDocx, filePath)) {
        showExportSuccess(FormatDocx, filePath);
    }
}

void PreviewPage::onExportPdf()
{
    QString errorMessage;
    if (!validateModelForExport(errorMessage)) {
        showExportError(FormatPdf, errorMessage);
        return;
    }
    
    QString filePath = promptForExportPath(FormatPdf);
    if (filePath.isEmpty()) {
        return; // User cancelled
    }
    
    if (performExport(FormatPdf, filePath)) {
        showExportSuccess(FormatPdf, filePath);
    }
}

void PreviewPage::onExportHtml()
{
    QString errorMessage;
    if (!validateModelForExport(errorMessage)) {
        showExportError(FormatHtml, errorMessage);
        return;
    }
    
    QString filePath = promptForExportPath(FormatHtml);
    if (filePath.isEmpty()) {
        return; // User cancelled
    }
    
    if (performExport(FormatHtml, filePath)) {
        showExportSuccess(FormatHtml, filePath);
    }
}

void PreviewPage::onPrint()
{
    if (!m_model) {
        QMessageBox::warning(this, tr("Print"), 
                           tr("No exam paper to print."));
        return;
    }
    
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Exam Paper"));
    
    if (dialog.exec() == QDialog::Accepted) {
        ui->previewBrowser->print(&printer);
    }
}

void PreviewPage::onZoomIn()
{
    zoomIn();
}

void PreviewPage::onZoomOut()
{
    zoomOut();
}

void PreviewPage::onResetZoom()
{
    resetZoom();
}

void PreviewPage::onZoomSliderChanged(int value)
{
    setZoomLevel(value);
}

QString PreviewPage::promptForExportPath(ExportFormat format)
{
    QString filter = getFileFilter(format);
    QString defaultName = getDefaultFilename(format);
    QString defaultPath = m_defaultExportDirectory + "/" + defaultName;
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export %1").arg(formatEnumToString(format)),
        defaultPath,
        filter
    );
    
    return filePath;
}

QString PreviewPage::getFileFilter(ExportFormat format) const
{
    switch (format) {
        case FormatDocx: return DOCX_FILTER;
        case FormatPdf: return PDF_FILTER;
        case FormatHtml: return HTML_FILTER;
        default: return tr("All Files (*)");
    }
}

QString PreviewPage::getDefaultFilename(ExportFormat format) const
{
    QString baseName = "exam_paper";
    
    if (m_model && !m_model->exam.title.isEmpty()) {
        baseName = m_model->exam.title;
        // Remove invalid filename characters
        baseName = baseName.replace(QRegularExpression("[<>:\"/\\|?*]"), "_");
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("_yyyyMMdd_HHmmss");
    
    switch (format) {
        case FormatDocx: return baseName + timestamp + ".docx";
        case FormatPdf: return baseName + timestamp + ".pdf";
        case FormatHtml: return baseName + timestamp + ".html";
        default: return baseName + timestamp;
    }
}

bool PreviewPage::performExport(ExportFormat format, const QString& filePath)
{
    emit exportStarted(format, filePath);
    
    QProgressDialog progress(
        tr("Exporting to %1...").arg(formatEnumToString(format)),
        tr("Cancel"),
        0, 0,
        this
    );
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(500);
    progress.setValue(0);
    
    bool success = false;
    QString errorMessage;
    
    try {
        switch (format) {
            case FormatDocx: {
                DocxExporter exporter;
                success = exporter.exportToDocx(*m_model, filePath, QString(), 12, true);
                if (!success) {
                    errorMessage = tr("Failed to create DOCX file. Check file permissions.");
                }
                break;
            }
            case FormatPdf: {
                PdfExporter exporter;
                success = exporter.exportToPdf(*m_model, filePath, QString(), 12, true);
                if (!success) {
                    errorMessage = tr("Failed to create PDF file. Check file permissions.");
                }
                break;
            }
            case FormatHtml: {
                QFile file(filePath);
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out.setEncoding(QStringConverter::Utf8);
                    out << m_model->toHtml(QString(), 12, true);
                    file.close();
                    success = true;
                } else {
                    errorMessage = tr("Failed to write HTML file: %1").arg(file.errorString());
                }
                break;
            }
        }
    } catch (const std::exception& e) {
        success = false;
        errorMessage = tr("Export error: %1").arg(e.what());
    }
    
    progress.setValue(1);
    
    if (success) {
        m_lastExportPath = filePath;
        m_defaultExportDirectory = QFileInfo(filePath).absolutePath();
        emit exportCompleted(format, filePath);
    } else {
        emit exportFailed(format, errorMessage);
        showExportError(format, errorMessage);
    }
    
    return success;
}

void PreviewPage::showExportSuccess(ExportFormat format, const QString& filePath)
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("Export Successful"));
    msgBox.setText(tr("Exam paper exported successfully to %1 format.").arg(formatEnumToString(format)));
    msgBox.setInformativeText(filePath);
    
    QPushButton* openButton = msgBox.addButton(tr("Open File"), QMessageBox::ActionRole);
    QPushButton* openFolderButton = msgBox.addButton(tr("Open Folder"), QMessageBox::ActionRole);
    msgBox.addButton(QMessageBox::Ok);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == openButton) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    } else if (msgBox.clickedButton() == openFolderButton) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).absolutePath()));
    }
}

void PreviewPage::showExportError(ExportFormat format, const QString& errorMessage)
{
    QMessageBox::critical(
        this,
        tr("Export Failed"),
        tr("Failed to export to %1 format.\n\n%2")
            .arg(formatEnumToString(format))
            .arg(errorMessage)
    );
}

bool PreviewPage::validateModelForExport(QString& errorMessage) const
{
    if (!m_model) {
        errorMessage = tr("No exam paper model available for export.");
        return false;
    }
    
    if (!m_model->isValid()) {
        errorMessage = tr("Exam paper is invalid or incomplete.");
        return false;
    }
    
    if (m_model->sections.isEmpty()) {
        errorMessage = tr("Exam paper has no sections.");
        return false;
    }
    
    return true;
}