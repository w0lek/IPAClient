#include "Path.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

Path& Path::instance() {
    static Path path;
    return path;
}

void Path::copyResourceFileToRuntimePath(const QString& srcFilePath) const
{
    // read
    QFile in(srcFilePath);
    if (!in.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open source file from resources";
        return;
    }
    QByteArray data{in.readAll()};
    in.close();

    // write
    QString fileName{QFileInfo(srcFilePath).fileName()};
    QString destinationFilePath{defaultWorkingPath()+"/"+fileName};
    QFile out(destinationFilePath);
    if (!out.open(QIODevice::WriteOnly)) {
        qDebug() << QString("Failed to open destination file %1 for writing").arg(destinationFilePath);
        return;
    }
    out.write(data);
    out.close();
}

QString Path::defaultWorkingPath() const
{
    return QCoreApplication::applicationDirPath();
}
