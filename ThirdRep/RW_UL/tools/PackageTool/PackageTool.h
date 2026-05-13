#pragma once

#include <QtWidgets/QMainWindow>
#include <QProcess>
#include <QScopedPointer>
#include "ui_PackageTool.h"

class QTemporaryDir;

enum PackageType {
    ToZip,
    ToExe
};

enum PackageLevel {
    Fast,
    Ultra
};

class PackageTool : public QMainWindow
{
    Q_OBJECT

public:
    PackageTool(QWidget *parent = nullptr);
    ~PackageTool();
private:
    void build_connect();
    // 构造压缩参数（不包含前缀命令 "a"、目标文件/目录）
    QStringList buildCompressionArgs(PackageLevel level, bool solidPreferred = true) const;

private:
    bool clearUnnecessaryFiles();
    bool copyFileWithDirs(const QString& srcPath, const QString& dstPath);

    void packageToZip();
    void packageToExe();
private slots:
    void btn_choosePackageSourcePath_clicked();
    void btn_choosePackageTargetPath_clicked();
    void rbtn_toZIP_checked(bool ischecked);
    void rbtn_toEXE_checked(bool ischecked);
    void rbtn_fast_checked(bool ischecked);
    void rbtn_ultra_checked(bool ischecked);

    void btn_startPackage_clicked();

    // 新增异步打包槽
    void startZipAsync();
    void onZipProcessStdOut();
    void onZipProcessStdErr();
    void onZipProcessFinished(int exitCode, QProcess::ExitStatus status);

    // EXE 异步新增
    void startExeAsync();
    void onExeProcessStdOut();
    void onExeProcessStdErr();
    void onExeProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    Ui::PackageToolClass ui;
    QString sourcePath{};
    QString targetPath{};

    // 当前用户选择
    PackageType  m_packageType = ToZip;
    PackageLevel m_packageLevel = Fast;

    // ZIP 异步相关
    QProcess* m_zipProcess = nullptr;
    QString     m_zipOutputFile;
    QByteArray  m_zipStdOutBuf;
    QByteArray  m_zipStdErrBuf;

    // EXE 异步相关
    QProcess* m_exeProcess = nullptr;
    QString     m_exeOutputFile;      // 最终自解压安装包
    QString     m_exeArchivePath;     // 临时 payload.7z
    QString     m_exeConfigPath;      // 临时 config.txt
    QScopedPointer<QTemporaryDir> m_exeTempDir;
    QString     m_exeSfxModule;       // 7z.sfx 路径
    QString     m_exeMainProgram;     // 入口主程序
    QByteArray  m_exeStdOutBuf;
    QByteArray  m_exeStdErrBuf;

    bool m_isPackaging = false;
};

