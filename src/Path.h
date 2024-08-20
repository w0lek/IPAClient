#pragma once

class QString;

class Path {
public:
    static Path& instance();

    void copyResourceFileToRuntimePath(const QString& srcFilePath) const;
    QString defaultWorkingPath() const;

private:
    Path()=default;
};

