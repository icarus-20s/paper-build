#include "PdfExporter.h"
#include <QPageLayout>
#include <QPainter>
#include <QPdfWriter>
#include <QTextDocument>

bool PdfExporter::exportToPdf(const PaperModel &model, const QString &filePath,
                              const QString &fontFamily, int fontSize) {
  QPdfWriter writer(filePath);
  writer.setPageSize(QPageSize(QPageSize::A4));
  if (model.exam.isLandscape) {
    writer.setPageOrientation(QPageLayout::Landscape);
  }
  writer.setPageMargins(QMarginsF(10, 10, 10, 10));
  QTextDocument doc;
  doc.setHtml(model.toHtml(fontFamily, fontSize));
  doc.print(&writer);
  return true;
}
