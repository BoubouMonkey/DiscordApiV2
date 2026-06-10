#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QFile>

class FileProcessor : public QObject
{
    Q_OBJECT

public:
    explicit FileProcessor(QObject *parent = nullptr);
    
    static bool isValidExecutable(const QString &filePath);
    static bool isPE32Executable(const QByteArray &data);
    static QByteArray readFile(const QString &filePath);
    static bool writeFile(const QString &filePath, const QByteArray &data);
    static QString getFileSizeString(qint64 size);
    static bool backupFile(const QString &filePath);

private:
    static bool checkPESignature(const QByteArray &data);
};

#endif // FILEPROCESSOR_H 