#pragma once

#include <QString>

namespace Validation {
    inline bool isNonEmpty(const QString &s) { return !s.trimmed().isEmpty(); }
}
