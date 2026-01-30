#include "DocxExporter.h"
#include <QFile>
#include <QTextStream>

bool DocxExporter::exportToDocx(const PaperModel &model, const QString &filePath, const QString &fontFamily, int fontSize)
{
    // Simple strategy: write an HTML file and save with .docx extension.
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f);
    out << model.toHtml(fontFamily, fontSize);
    f.close();
    return true;
}
