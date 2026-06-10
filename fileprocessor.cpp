#include "fileprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

FileProcessor::FileProcessor(QObject *parent)
    : QObject(parent)
{
}

bool FileProcessor::isValidExecutable(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return false;
    }
    
    // Check if it's an executable file
    QString extension = fileInfo.suffix().toLower();
    if (extension != "exe" && extension != "dll" && extension != "sys") {
        return false;
    }
    
    // Read file and check PE signature
    QByteArray data = readFile(filePath);
    if (data.isEmpty()) {
        return false;
    }
    
    return isPE32Executable(data);
}

bool FileProcessor::isPE32Executable(const QByteArray &data)
{
    if (data.size() < 64) {
        return false;
    }
    
    return checkPESignature(data);
}

QByteArray FileProcessor::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    return data;
}

bool FileProcessor::writeFile(const QString &filePath, const QByteArray &data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    qint64 bytesWritten = file.write(data);
    file.close();
    
    return bytesWritten == data.size();
}

QString FileProcessor::getFileSizeString(qint64 size)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    
    if (size >= GB) {
        return QString("%1 GB").arg(QString::number(size / (double)GB, 'f', 2));
    } else if (size >= MB) {
        return QString("%1 MB").arg(QString::number(size / (double)MB, 'f', 2));
    } else if (size >= KB) {
        return QString("%1 KB").arg(QString::number(size / (double)KB, 'f', 2));
    } else {
        return QString("%1 bytes").arg(size);
    }
}

bool FileProcessor::backupFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        return false;
    }
    
    QString backupPath = fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_backup." + fileInfo.suffix();
    
    QFile sourceFile(filePath);
    QFile backupFile(backupPath);
    
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    if (!backupFile.open(QIODevice::WriteOnly)) {
        sourceFile.close();
        return false;
    }
    
    QByteArray data = sourceFile.readAll();
    sourceFile.close();
    
    qint64 bytesWritten = backupFile.write(data);
    backupFile.close();
    
    return bytesWritten == data.size();
}

bool FileProcessor::checkPESignature(const QByteArray &data)
{
    // Check DOS header
    if (data.size() < 64 || data.left(2) != QByteArray::fromHex("4D5A")) {
        return false;
    }
    
    // Get PE header offset from DOS header
    if (data.size() < 64) {
        return false;
    }
    
    // Read PE header offset (offset 0x3C)
    quint32 peOffset = *reinterpret_cast<const quint32*>(data.data() + 0x3C);
    
    if (peOffset >= static_cast<quint32>(data.size()) || peOffset < 64) {
        return false;
    }
    
    // Check PE signature
    if (data.size() < peOffset + 4) {
        return false;
    }
    
    QByteArray peSignature = data.mid(peOffset, 4);
    return peSignature == QByteArray::fromHex("50450000"); // "PE\0\0"
} 