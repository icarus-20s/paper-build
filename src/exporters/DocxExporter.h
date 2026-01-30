#pragma once

#include <QString>
#include "../models/PaperModel.h"

/**
 * DocxExporter: Writes a simple HTML-based .docx file (Word can open HTML-wrapped DOC files).
 */
class DocxExporter
{
public:
    DocxExporter() = default;
    bool exportToDocx(const PaperModel &model, const QString &filePath, const QString &fontFamily = "Times New Roman", int fontSize = 12);
};
