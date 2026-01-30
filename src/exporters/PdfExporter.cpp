#include "PdfExporter.h"
#include <QPageLayout>
#include <QPainter>
#include <QPdfWriter>
#include <QTextDocument>

bool PdfExporter::exportToPdf(const PaperModel &model, const QString &filePath,
                              const QString &fontFamily, int fontSize, bool portrait) {
  QPdfWriter writer(filePath);
  writer.setPageSize(QPageSize(QPageSize::A4));
  writer.setPageOrientation(portrait ? QPageLayout::Portrait : QPageLayout::Landscape);
  writer.setPageMargins(QMarginsF(15, 15, 15, 15)); // 15mm margins
  QTextDocument doc;
  doc.setHtml(model.toHtml(fontFamily, fontSize, portrait));
  doc.print(&writer);
  return true;
}
