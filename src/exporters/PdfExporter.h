#pragma once

#include <QString>
#include "../models/PaperModel.h"

/**
 * PdfExporter: Renders the PaperModel HTML to a PDF using Qt's QPdfWriter.
 */
class PdfExporter
{
public:
    PdfExporter() = default;
    bool exportToPdf(const PaperModel &model, const QString &filePath, const QString &fontFamily = "Times New Roman", int fontSize = 12, bool portrait = true);
};
