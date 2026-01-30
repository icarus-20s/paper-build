#pragma once

#include <QString>

namespace FileUtils {
    inline QString ensureExtension(const QString &path, const QString &ext) {
        if (path.endsWith(ext)) return path;
        return path + ext;
    }
}
