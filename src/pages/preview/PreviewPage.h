#pragma once

#include <QWidget>
#include <QString>

// Forward declarations
class PaperModel;
namespace Ui { 
    class PreviewPage; 
}

/**
 * @file PreviewPage.h
 * @brief Defines the PreviewPage class for previewing and exporting exam papers.
 */

/**
 * @class PreviewPage
 * @brief Page for previewing exam papers and exporting to various formats.
 * 
 * PreviewPage provides a comprehensive interface for:
 * - Live HTML preview of the exam paper
 * - Export to DOCX format
 * - Export to PDF format
 * - Print functionality
 * - Zoom controls
 * - Format selection options
 * - Export history tracking
 * 
 * The page uses a QTextBrowser for rich HTML preview and supports
 * various export formats through dedicated exporter classes.
 * 
 * @note This is typically the final page in a wizard-style interface.
 */
class PreviewPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Export format enumeration.
     */
    enum ExportFormat {
        FormatDocx,     ///< Microsoft Word format
        FormatPdf,      ///< PDF format
        FormatHtml      ///< HTML format
    };
    Q_ENUM(ExportFormat)

    /**
     * @brief Constructs a PreviewPage.
     * @param parent Parent widget (default: nullptr)
     */
    explicit PreviewPage(QWidget* parent = nullptr);

    /**
     * @brief Destructor - cleans up UI resources.
     */
    ~PreviewPage() override;

    /**
     * @brief Sets the paper model to preview and export.
     * @param model Pointer to PaperModel (ownership not transferred)
     */
    void setPaperModel(PaperModel* model);

    /**
     * @brief Gets the current paper model.
     * @return Pointer to current PaperModel, or nullptr if not set
     */
    PaperModel* paperModel() const;

    /**
     * @brief Refreshes the preview with current model data.
     */
    void refreshPreview();

    /**
     * @brief Sets the zoom level for the preview.
     * @param zoomPercent Zoom percentage (50-200)
     */
    void setZoomLevel(int zoomPercent);

    /**
     * @brief Gets the current zoom level.
     * @return Zoom percentage
     */
    int getZoomLevel() const;

    /**
     * @brief Sets the default export directory.
     * @param directory Path to default export directory
     */
    void setDefaultExportDirectory(const QString& directory);

    /**
     * @brief Gets the last export file path.
     * @return Path to last exported file
     */
    QString getLastExportPath() const;

    /**
     * @brief Enables or disables export buttons.
     * @param enabled Enable/disable state
     */
    void setExportEnabled(bool enabled);

    /**
     * @brief Checks if the preview is ready for export.
     * @return true if model is set and valid
     */
    bool isReadyForExport() const;

signals:
    /**
     * @brief Emitted when the user clicks the back button.
     */
    void backClicked();

    /**
     * @brief Emitted when export starts.
     * @param format Export format
     * @param filePath Destination file path
     */
    void exportStarted(ExportFormat format, const QString& filePath);

    /**
     * @brief Emitted when export completes successfully.
     * @param format Export format
     * @param filePath Destination file path
     */
    void exportCompleted(ExportFormat format, const QString& filePath);

    /**
     * @brief Emitted when export fails.
     * @param format Export format
     * @param errorMessage Error description
     */
    void exportFailed(ExportFormat format, const QString& errorMessage);

    /**
     * @brief Emitted when the zoom level changes.
     * @param zoomPercent New zoom percentage
     */
    void zoomChanged(int zoomPercent);

public slots:
    /**
     * @brief Exports to DOCX format with user file selection.
     */
    void exportToDocx();

    /**
     * @brief Exports to PDF format with user file selection.
     */
    void exportToPdf();

    /**
     * @brief Exports to HTML format with user file selection.
     */
    void exportToHtml();

    /**
     * @brief Prints the exam paper.
     */
    void printPaper();

    /**
     * @brief Zooms in the preview.
     */
    void zoomIn();

    /**
     * @brief Zooms out the preview.
     */
    void zoomOut();

    /**
     * @brief Resets zoom to 100%.
     */
    void resetZoom();

private slots:
    /**
     * @brief Handles the back button click.
     */
    void onBackClicked();

    /**
     * @brief Handles the export DOCX button click.
     */
    void onExportDocx();

    /**
     * @brief Handles the export PDF button click.
     */
    void onExportPdf();

    /**
     * @brief Handles the export HTML button click.
     */
    void onExportHtml();

    /**
     * @brief Handles the print button click.
     */
    void onPrint();

    /**
     * @brief Handles zoom in button click.
     */
    void onZoomIn();

    /**
     * @brief Handles zoom out button click.
     */
    void onZoomOut();

    /**
     * @brief Handles reset zoom button click.
     */
    void onResetZoom();

    /**
     * @brief Handles zoom slider value change.
     * @param value Zoom value
     */
    void onZoomSliderChanged(int value);

private:
    /**
     * @brief UI object created from .ui file.
     */
    Ui::PreviewPage* ui;

    /**
     * @brief Pointer to the paper model being previewed.
     */
    PaperModel* m_model;

    /**
     * @brief Current zoom level (percentage).
     */
    int m_zoomLevel;

    /**
     * @brief Default export directory.
     */
    QString m_defaultExportDirectory;

    /**
     * @brief Last export file path.
     */
    QString m_lastExportPath;

    /**
     * @brief Sets up signal-slot connections.
     */
    void setupConnections();

    /**
     * @brief Configures UI components with default settings.
     */
    void setupUi();

    /**
     * @brief Sets up the toolbar with zoom and export controls.
     */
    void setupToolbar();

    /**
     * @brief Prompts user for export file path.
     * @param format Export format
     * @return Selected file path, or empty string if cancelled
     */
    QString promptForExportPath(ExportFormat format);

    /**
     * @brief Gets the file filter string for a format.
     * @param format Export format
     * @return File filter string for file dialog
     */
    QString getFileFilter(ExportFormat format) const;

    /**
     * @brief Gets the default filename for export.
     * @param format Export format
     * @return Suggested filename
     */
    QString getDefaultFilename(ExportFormat format) const;

    /**
     * @brief Performs the actual export operation.
     * @param format Export format
     * @param filePath Destination file path
     * @return true if export succeeded
     */
    bool performExport(ExportFormat format, const QString& filePath);

    /**
     * @brief Shows export success message.
     * @param format Export format
     * @param filePath Exported file path
     */
    void showExportSuccess(ExportFormat format, const QString& filePath);

    /**
     * @brief Shows export error message.
     * @param format Export format
     * @param errorMessage Error description
     */
    void showExportError(ExportFormat format, const QString& errorMessage);

    /**
     * @brief Updates the zoom display.
     */
    void updateZoomDisplay();

    /**
     * @brief Validates the paper model before export.
     * @param errorMessage Output parameter for error message
     * @return true if model is valid for export
     */
    bool validateModelForExport(QString& errorMessage) const;

    /**
     * @brief Updates button states based on current state.
     */
    void updateButtonStates();

    /**
     * @brief Applies the current zoom level to the preview.
     */
    void applyZoom();
};