#include "PackageTool.h"
#include<QMessageBox>
#include<QFileDialog>
#include<QDirIterator>
#include <QProcess>
#include <QDateTime>
#include <QFileInfo>
#include <QUuid>
#include <QInputDialog>
#include <QTemporaryDir>
#include <QTextStream>
#include <QRegularExpression>

PackageTool::PackageTool(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	build_connect();

    ui.progressBar->setRange(0, 100);
    ui.progressBar->setValue(0);
}

PackageTool::~PackageTool()
{
	
}

void PackageTool::build_connect()
{
	connect(ui.btn_choosePackageSourcePath, &QPushButton::clicked, this, &PackageTool::btn_choosePackageSourcePath_clicked);
	connect(ui.btn_choosePackageTargetPath, &QPushButton::clicked, this, &PackageTool::btn_choosePackageTargetPath_clicked);
	connect(ui.btn_startPackage, &QPushButton::clicked, this, &PackageTool::btn_startPackage_clicked);
	connect(ui.rbtn_toZIP, &QRadioButton::toggled, this, &PackageTool::rbtn_toZIP_checked);
	connect(ui.rbtn_toEXE, &QRadioButton::toggled, this, &PackageTool::rbtn_toEXE_checked);
	connect(ui.rbtn_fast, &QRadioButton::toggled, this, &PackageTool::rbtn_fast_checked);
	connect(ui.rbtn_ultra, &QRadioButton::toggled, this, &PackageTool::rbtn_ultra_checked);
}

QStringList PackageTool::buildCompressionArgs(PackageLevel level, bool solidPreferred) const
{
    // 公共基础：-y 自动 yes，-bb0 简洁日志，-mmt=on 多线程
    QStringList args;
    args << "-y" << "-bb0" << "-mmt=on";

    switch (level) {
    case Fast:
        // 快速：较低压缩 + 关闭 solid（大量小文件会更快）
        args << "-mx=3" << "-ms=off";
        break;
    case Ultra:
        // 高压缩：可根据需要调整字典大小与 fast bytes
        args << "-mx=9" << "-m0=lzma2" << "-md=512m" << "-mfb=64";
        if (solidPreferred)
            args << "-ms=on";
        else
            args << "-ms=off";
        break;
    }
    return args;
}

bool PackageTool::clearUnnecessaryFiles()
{
    if (sourcePath.isEmpty() || targetPath.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择源目录与目标目录。"));
        return false;
    }
    QDir srcDir(sourcePath);
    if (!srcDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("源目录不存在。"));
        return false;
    }
    QDir tgtDir(targetPath);
    if (!tgtDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("目标目录不存在。"));
        return false;
    }

    // 清理
    QString baseDirName = QFileInfo(srcDir.absolutePath()).fileName();
    QString autogenDirPath = srcDir.filePath(baseDirName + "_autogen");
    QStringList errors;
    auto removeDirIfExists = [](const QString& dirPath, QStringList& errors) {
        QDir d(dirPath);
        if (d.exists() && !d.removeRecursively())
            errors << QObject::tr("删除目录失败: %1").arg(dirPath);
        };
    removeDirIfExists(autogenDirPath, errors);
    removeDirIfExists(srcDir.filePath("CMakeFiles"), errors);
    {
        QDirIterator it(srcDir.absolutePath(), QStringList() << "*.h", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QFile f(it.next());
            if (!f.remove())
                errors << tr("删除头文件失败: %1").arg(f.fileName());
        }
    }
    if (!errors.isEmpty()) {
        QMessageBox::warning(this, tr("清理警告"),
            tr("部分文件/目录未能删除：\n%1").arg(errors.join("\n")));
    }
    return true;
}

bool PackageTool::copyFileWithDirs(const QString& srcPath, const QString& dstPath)
{
    QFileInfo fi(srcPath);
    QDir dstDir = QFileInfo(dstPath).dir();
    if (!dstDir.exists()) {
        if (!dstDir.mkpath(".")) return false;
    }
    QFile::remove(dstPath);
    return QFile::copy(srcPath, dstPath);
}

void PackageTool::packageToZip()
{
    if (sourcePath.isEmpty() || targetPath.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择源目录与目标目录。"));
        return;
    }
    QDir srcDir(sourcePath);
    if (!srcDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("源目录不存在。"));
        return;
    }
    QDir tgtDir(targetPath);
    if (!tgtDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("目标目录不存在。"));
        return;
    }

    if (!clearUnnecessaryFiles())
        return;

    // 寻找 7z.exe
    QProcess which;
    which.start("where", QStringList() << "7z.exe");
    if (!which.waitForStarted(3000) || !which.waitForFinished(5000)) {
        QMessageBox::critical(this, tr("错误"), tr("无法定位 7z.exe (where 失败)。"));
        return;
    }
    QString sevenZPath = QString::fromLocal8Bit(which.readAllStandardOutput())
        .split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts).value(0);
    if (sevenZPath.isEmpty() || !QFile::exists(sevenZPath)) {
        QMessageBox::critical(this, tr("错误"), tr("未找到 7z.exe，请将 7-Zip 添加到 PATH。"));
        return;
    }

    QString baseName = QFileInfo(srcDir.absolutePath()).fileName();
    if (baseName.isEmpty()) baseName = "package";
    QString timePart = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString zipFilePath = tgtDir.filePath(baseName + "_" + timePart + ".zip");
    if (QFile::exists(zipFilePath))
        QFile::remove(zipFilePath);

    // 使用 7z 压缩
    int exitCode = -1;
    QByteArray so, se;
    {
        QProcess p;
        QStringList args;
        // 修正: 原先使用了无效的 -mt，应为 -mmt 或 -mmt=on
        // 说明:
        //  -tzip   目标格式 zip
        //  -mx=9   最高压缩
        //  -r      递归
        //  -y      全部 yes
        //  -bb0    最低日志级别
        //  -mmt=on 启用多线程 (可写 -mmt 或 -mmt=N)
        //  "."     以当前目录为根打包（配合 -r 包含子目录）
        args << "a" << "-t7z"
            << "-mx=5"
            << "-r"
            << "-y"
            << "-bb0"
            << "-mmt=on"
            << zipFilePath
            << ".";

        p.setWorkingDirectory(sourcePath);
        p.start(sevenZPath, args);
        if (!p.waitForStarted(5000)) {
            QMessageBox::critical(this, tr("错误"), tr("启动 7z.exe 失败。"));
            return;
        }
        p.waitForFinished(-1);
        exitCode = p.exitCode();
        so = p.readAllStandardOutput();
        se = p.readAllStandardError();
    }

    // 7-Zip 返回码参考:
    // 0 正常; 1 有警告(仍可用); 2 及以上为错误（2 致命错误）
    if ((exitCode != 0 && exitCode != 1) || !QFile::exists(zipFilePath)) {
        QString reason;
        switch (exitCode) {
        case 2: reason = tr("致命错误 (Fatal error)"); break;
        case 7: reason = tr("命令行参数错误 (Command line error)"); break;
        case 8: reason = tr("内存不足 (Not enough memory)"); break;
        case 255: reason = tr("被用户中断 (User break)"); break;
        default: reason = tr("未知错误"); break;
        }
        QMessageBox::critical(this, tr("压缩失败"),
            tr("退出码: %1 (%2)\nstdout:\n%3\nstderr:\n%4")
            .arg(exitCode)
            .arg(reason)
            .arg(QString::fromLocal8Bit(so))
            .arg(QString::fromLocal8Bit(se)));
        if (QFile::exists(zipFilePath))
            QFile::remove(zipFilePath);
        return;
    }

    if (exitCode == 1) {
        QMessageBox::information(this, tr("完成 (含警告)"),
            tr("压缩已生成(存在警告)。\n文件: %1").arg(zipFilePath));
    }
    else {
        QMessageBox::information(this, tr("成功"), tr("已生成: %1").arg(zipFilePath));
    }
}

void PackageTool::packageToExe()
{
    if (sourcePath.isEmpty() || targetPath.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择源目录与目标目录。"));
        return;
    }
    QDir srcDir(sourcePath);
    if (!srcDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("源目录不存在。"));
        return;
    }
    QDir tgtDir(targetPath);
    if (!tgtDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("目标目录不存在。"));
        return;
    }

    clearUnnecessaryFiles();

    QProcess which;
    which.start("where", QStringList() << "7z.exe");
    if (!which.waitForStarted(3000) || !which.waitForFinished(5000)) {
        QMessageBox::critical(this, tr("错误"), tr("无法定位 7z.exe (where 7z.exe 失败)。"));
        return;
    }
    QString sevenZPath = QString::fromLocal8Bit(which.readAllStandardOutput())
        .split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts).value(0);
    if (sevenZPath.isEmpty() || !QFile::exists(sevenZPath)) {
        QMessageBox::critical(this, tr("错误"), tr("未找到 7z.exe，请将 7-Zip 添加到 PATH。"));
        return;
    }
    QString sevenZDir = QFileInfo(sevenZPath).absolutePath();
    QString sfxModule = QDir(sevenZDir).filePath("7z.sfx");
    if (!QFile::exists(sfxModule)) {
        QMessageBox::critical(this, tr("错误"), tr("未找到 7z.sfx（应位于 7z.exe 同目录）。"));
        return;
    }

    // 入口程序选择（允许为空）
    QString mainExe;
    QStringList exeCandidates;
    {
        QDir root(sourcePath);
        QFileInfoList lis = root.entryInfoList(QStringList() << "*.exe", QDir::Files);
        for (auto& fi : lis) exeCandidates << fi.fileName();
    }
    if (exeCandidates.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("未发现入口 EXE，将生成纯解压自解压包。"));
    }
    else if (exeCandidates.size() == 1) {
        mainExe = exeCandidates.first();
    }
    else {
        bool ok = false;
        mainExe = QInputDialog::getItem(this, tr("选择主程序"),
            tr("请选择安装后要自动运行的主程序（可取消表示不自动运行）："),
            exeCandidates, 0, false, &ok);
        if (!ok) mainExe.clear();
    }

    QString baseName = QFileInfo(srcDir.absolutePath()).fileName();
    if (baseName.isEmpty()) baseName = "App";
    QString timePart = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString outExePath = tgtDir.filePath(baseName + "_" + timePart + "_installer.exe");

    // 父目录 + 仅打包 baseName 目录
    QDir parentDir = QFileInfo(srcDir.absolutePath()).dir();

    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        QMessageBox::critical(this, tr("错误"), tr("创建临时目录失败。"));
        return;
    }
    QString archivePath = QDir(tempDir.path()).filePath("payload.7z");
    QString configPath = QDir(tempDir.path()).filePath("config.txt");

    {   // 生成 payload.7z
        QProcess p;
        QStringList args;
        args << "a"
            << "-r"
            << "-y"
            << "-bb0"
            << "-mmt=on"
            // 可降低压缩级别提升速度: 例如再加 "-mx=5" 或 "-mx=3"
			<< "-mx=7"
            << archivePath
            << baseName;
        p.setWorkingDirectory(parentDir.absolutePath());
        p.start(sevenZPath, args);
        if (!p.waitForStarted(5000)) {
            QMessageBox::critical(this, tr("错误"), tr("启动 7z.exe 失败。"));
            return;
        }
        p.waitForFinished(-1);
        if (p.exitCode() != 0 || !QFile::exists(archivePath)) {
            QMessageBox::critical(this, tr("错误"),
                tr("7z 打包失败。\nstdout:\n%1\nstderr:\n%2")
                .arg(QString::fromLocal8Bit(p.readAllStandardOutput()))
                .arg(QString::fromLocal8Bit(p.readAllStandardError())));
            return;
        }
    }

    {   // 写 SFX 配置
        QFile f(configPath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            QMessageBox::critical(this, tr("错误"), tr("写入配置文件失败。"));
            return;
        }
        QString cfg;
        cfg += u8";!@Install@!UTF-8!\n";
        cfg += "Title=\"" + baseName + (mainExe.isEmpty() ? u8" 自解压包" : u8" 安装程序") + "\"\n";
        cfg += "BeginPrompt=\"是否" + QString(mainExe.isEmpty() ? u8"解压 " : u8"安装 ") + baseName + u8"?\"\n";
    	cfg += "GUIMode=\"2\"\n";
        cfg += "OverwriteMode=\"1\"\n";
        if (!mainExe.isEmpty())
            cfg += "RunProgram=\"" + mainExe + "\"\n";
        cfg += ";!@InstallEnd@!";
        QTextStream ts(&f);
        ts << cfg;
    }

    auto appendFile = [](QFile& out, const QString& inPath) -> bool {
        QFile in(inPath);
        if (!in.open(QIODevice::ReadOnly)) return false;
        constexpr qint64 BUF = 1 << 20;
        QByteArray buffer;
        buffer.resize(BUF);
        while (true) {
            qint64 r = in.read(buffer.data(), buffer.size());
            if (r < 0) return false;
            if (r == 0) break;
            if (out.write(buffer.constData(), r) != r) return false;
        }
        return true;
        };

    QFile out(outExePath);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(this, tr("错误"), tr("无法创建输出文件: %1").arg(outExePath));
        return;
    }
    if (!appendFile(out, sfxModule) ||
        !appendFile(out, configPath) ||
        !appendFile(out, archivePath)) {
        out.close();
        QFile::remove(outExePath);
        QMessageBox::critical(this, tr("错误"), tr("拼接自解压安装包失败。"));
        return;
    }
    out.close();

    if (!QFile::exists(outExePath)) {
        QMessageBox::critical(this, tr("错误"), tr("输出安装包未生成。"));
        return;
    }

    QMessageBox::information(this, tr("成功"),
        tr("已生成%1: %2")
        .arg(mainExe.isEmpty() ? tr("自解压包") : tr("安装包"))
        .arg(outExePath));
}

void PackageTool::btn_choosePackageSourcePath_clicked()
{
	QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), "");
	if (!folderPath.isEmpty())
	{
		ui.lineEdit_sourcePath->setText(folderPath);
		sourcePath = folderPath;
	}
	else
	{
		ui.lineEdit_sourcePath->clear();
		sourcePath.clear();
	}
}

void PackageTool::btn_choosePackageTargetPath_clicked()
{
	QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), "");
	if (!folderPath.isEmpty())
	{
		ui.lineEdit_targetPath->setText(folderPath);
		targetPath = folderPath;
	}
	else
	{
		ui.lineEdit_targetPath->clear();
		targetPath.clear();
	}
}

void PackageTool::btn_startPackage_clicked()
{
    if (m_isPackaging) {
        QMessageBox::information(this, tr("提示"), tr("正在打包，请稍候..."));
        return;
    }

    ui.progressBar->setRange(0, 0);

    switch (m_packageType) {
    case ToZip: startZipAsync(); break;
    case ToExe: startExeAsync(); break;
    }
}

// ---------------- 单选按钮槽 ----------------
void PackageTool::rbtn_toZIP_checked(bool ischecked)
{
    if (ischecked) {
        m_packageType = ToZip;
        ui.btn_startPackage->setText(tr("打包为 ZIP"));
        if (ui.rbtn_toEXE) ui.rbtn_toEXE->setChecked(false);
    }
}

void PackageTool::rbtn_toEXE_checked(bool ischecked)
{
    if (ischecked) {
        m_packageType = ToExe;
        ui.btn_startPackage->setText(tr("打包为 EXE"));
        if (ui.rbtn_toZIP) ui.rbtn_toZIP->setChecked(false);
    }
}

void PackageTool::rbtn_fast_checked(bool ischecked)
{
    if (ischecked) {
        m_packageLevel = Fast;
        if (ui.rbtn_ultra) ui.rbtn_ultra->setChecked(false);
    }
}

void PackageTool::rbtn_ultra_checked(bool ischecked)
{
    if (ischecked) {
        m_packageLevel = Ultra;
        if (ui.rbtn_fast) ui.rbtn_fast->setChecked(false);
    }
}

// ================= ZIP 异步 =================
void PackageTool::startZipAsync()
{
    if (sourcePath.isEmpty() || targetPath.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择源目录与目标目录。"));
        return;
    }
    QDir srcDir(sourcePath);
    if (!srcDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("源目录不存在。"));
        return;
    }
    QDir tgtDir(targetPath);
    if (!tgtDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("目标目录不存在。"));
        return;
    }

    // 清理（同步执行；若这里也很慢，可再放线程）
    if (!clearUnnecessaryFiles())
        return;

    // 定位 7z.exe
    QProcess which;
    which.start("where", QStringList() << "7z.exe");
    if (!which.waitForStarted(3000) || !which.waitForFinished(5000)) {
        QMessageBox::critical(this, tr("错误"), tr("无法定位 7z.exe。"));
        return;
    }
    QString sevenZPath = QString::fromLocal8Bit(which.readAllStandardOutput())
        .split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts).value(0);
    if (sevenZPath.isEmpty() || !QFile::exists(sevenZPath)) {
        QMessageBox::critical(this, tr("错误"), tr("未找到 7z.exe，请配置 PATH。"));
        return;
    }

    QString baseName = QFileInfo(srcDir.absolutePath()).fileName();
    if (baseName.isEmpty()) baseName = "package";
    QString timePart = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    m_zipOutputFile = tgtDir.filePath(baseName + "_" + timePart + ".zip");
    if (QFile::exists(m_zipOutputFile))
        QFile::remove(m_zipOutputFile);

    // 准备 QProcess
    if (!m_zipProcess) {
        m_zipProcess = new QProcess(this);
        connect(m_zipProcess, &QProcess::readyReadStandardOutput, this, &PackageTool::onZipProcessStdOut);
        connect(m_zipProcess, &QProcess::readyReadStandardError, this, &PackageTool::onZipProcessStdErr);
        connect(m_zipProcess, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this, &PackageTool::onZipProcessFinished);
    }

    // 重置状态
    m_zipStdOutBuf.clear();
    m_zipStdErrBuf.clear();
    m_isPackaging = true;
    ui.btn_startPackage->setEnabled(false);

    QStringList args;
    args << "a";

    // 根据压缩级别附加
    args << buildCompressionArgs(m_packageLevel, /*solidPreferred=*/true);

    // 指定格式（你原逻辑用了 -t7z，想改 zip 可改 -tzip）
    args << "-t7z";

    // 递归
    args << "-r";

    // 输出进度
    args << "-bsp1";

    args << m_zipOutputFile
        << "."; // 仍在源目录下打包其内容（如需包含顶层目录可调整 workingDirectory + baseName）


    m_zipProcess->setWorkingDirectory(sourcePath);
    m_zipProcess->start(sevenZPath, args);
    if (!m_zipProcess->waitForStarted(3000)) {
        m_isPackaging = false;
        ui.btn_startPackage->setEnabled(true);
        QMessageBox::critical(this, tr("错误"), tr("启动 7z 失败。"));
        return;
    }
}

void PackageTool::onZipProcessStdOut()
{
    m_zipStdOutBuf += m_zipProcess->readAllStandardOutput();

    // 按行解析
    QList<QByteArray> lines = m_zipStdOutBuf.split('\n');
    // 保留最后一行（可能是半行）
    if (!m_zipStdOutBuf.endsWith('\n'))
        m_zipStdOutBuf = lines.takeLast();
    else
        m_zipStdOutBuf.clear();

    QRegularExpression re(R"((\d+)%\s*)");
    for (const auto& line : lines) {
        QString s = QString::fromLocal8Bit(line).trimmed();
        auto m = re.match(s);
        if (m.hasMatch()) {
            int percent = m.captured(1).toInt();
            if (percent >= 0 && percent <= 100) {
                ui.progressBar->setValue(percent);
            }
        }
        // 可根据需要收集/显示其它日志
    }
}

void PackageTool::onZipProcessStdErr()
{
    m_zipStdErrBuf += m_zipProcess->readAllStandardError();
    // 一般错误输出可在结束时统一显示
}

void PackageTool::onZipProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    ui.btn_startPackage->setEnabled(true);
    bool ok = (status == QProcess::NormalExit) && (exitCode == 0 || exitCode == 1) && QFile::exists(m_zipOutputFile);

    if (!ok) {
        QString reason;
        switch (exitCode) {
        case 0: reason = tr("未知（标记失败但文件缺失）"); break;
        case 1: reason = tr("警告"); break;
        case 2: reason = tr("致命错误"); break;
        case 7: reason = tr("参数错误"); break;
        case 8: reason = tr("内存不足"); break;
        case 255: reason = tr("用户中断"); break;
        default: reason = tr("其它错误");
        }
        QMessageBox::critical(this, tr("打包失败"),
            tr("退出码: %1 (%2)\nstdout:\n%3\nstderr:\n%4")
            .arg(exitCode)
            .arg(reason)
            .arg(QString::fromLocal8Bit(m_zipStdOutBuf))
            .arg(QString::fromLocal8Bit(m_zipStdErrBuf)));
        if (QFile::exists(m_zipOutputFile))
            QFile::remove(m_zipOutputFile);
        ui.progressBar->setRange(0, 100);
        ui.progressBar->setValue(0);
    }
    else {
        if (exitCode == 1) {
            QMessageBox::information(this, tr("完成(含警告)"),
                tr("文件: %1\nstdout:\n%2")
                .arg(m_zipOutputFile)
                .arg(QString::fromLocal8Bit(m_zipStdOutBuf)));
        }
        else {
            ui.progressBar->setValue(100);
            QMessageBox::information(this, tr("成功"), tr("已生成: %1").arg(m_zipOutputFile));
        }
    }

    m_isPackaging = false;

    ui.btn_startPackage->setEnabled(true);
    ui.progressBar->setRange(0, 100);
}

// ================= EXE 异步 =================
void PackageTool::startExeAsync()
{
    if (sourcePath.isEmpty() || targetPath.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择源目录与目标目录。"));
        return;
    }
    QDir srcDir(sourcePath);
    if (!srcDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("源目录不存在。"));
        return;
    }
    QDir tgtDir(targetPath);
    if (!tgtDir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("目标目录不存在。"));
        return;
    }

    if (!clearUnnecessaryFiles())
        return;

    QProcess which;
    which.start("where", QStringList() << "7z.exe");
    if (!which.waitForStarted(3000) || !which.waitForFinished(5000)) {
        QMessageBox::critical(this, tr("错误"), tr("无法定位 7z.exe。"));
        return;
    }
    QString sevenZPath = QString::fromLocal8Bit(which.readAllStandardOutput())
        .split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts).value(0);
    if (sevenZPath.isEmpty() || !QFile::exists(sevenZPath)) {
        QMessageBox::critical(this, tr("错误"), tr("未找到 7z.exe，请配置 PATH。"));
        return;
    }
    QString sevenZDir = QFileInfo(sevenZPath).absolutePath();
    m_exeSfxModule = QDir(sevenZDir).filePath("7z.sfx");
    if (!QFile::exists(m_exeSfxModule)) {
        QMessageBox::critical(this, tr("错误"), tr("未找到 7z.sfx（应与 7z.exe 同目录）。"));
        return;
    }

    // 入口 EXE（允许为空）
    m_exeMainProgram.clear();
    QStringList exeCandidates;
    {
        QDir root(sourcePath);
        QFileInfoList lis = root.entryInfoList(QStringList() << "*.exe", QDir::Files);
        for (auto& fi : lis) exeCandidates << fi.fileName();
    }
    if (exeCandidates.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("未发现入口 EXE，将生成纯解压包。"));
    }
    else if (exeCandidates.size() == 1) {
        m_exeMainProgram = exeCandidates.first();
    }
    else {
        bool ok = false;
        m_exeMainProgram = QInputDialog::getItem(this, tr("选择主程序"),
            tr("请选择安装后要自动运行的主程序（取消则不自动运行）："),
            exeCandidates, 0, false, &ok);
        if (!ok) m_exeMainProgram.clear();
    }

    QString baseName = QFileInfo(srcDir.absolutePath()).fileName();
    if (baseName.isEmpty()) baseName = "App";
    QString timePart = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    m_exeOutputFile = tgtDir.filePath(baseName + "_" + timePart + "_installer.exe");

    // 改：工作目录设为父目录，参数使用 baseName
    QDir parentDir = QFileInfo(srcDir.absolutePath()).dir();

    m_exeTempDir.reset(new QTemporaryDir);
    if (!m_exeTempDir->isValid()) {
        QMessageBox::critical(this, tr("错误"), tr("创建临时目录失败。"));
        return;
    }
    m_exeArchivePath = QDir(m_exeTempDir->path()).filePath("payload.7z");
    m_exeConfigPath = QDir(m_exeTempDir->path()).filePath("config.txt");

    {   // 写 config （RunProgram 可选）
        QFile f(m_exeConfigPath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            QMessageBox::critical(this, tr("错误"), tr("写入配置文件失败。"));
            return;
        }
        QString cfg;
        cfg += u8";!@Install@!UTF-8!\n";
        cfg += "Title=\"" + baseName + (m_exeMainProgram.isEmpty() ? u8" 自解压包" : u8" 安装程序") + "\"\n";
        cfg += "BeginPrompt=\"是否" + QString(m_exeMainProgram.isEmpty() ? u8"解压 " : u8"安装 ") + baseName + u8"?\"\n";
        cfg += "GUIMode=\"2\"\n";
        cfg += "OverwriteMode=\"1\"\n";
        if (!m_exeMainProgram.isEmpty())
            cfg += "RunProgram=\"" + m_exeMainProgram + "\"\n";
        cfg += ";!@InstallEnd@!";
        QTextStream ts(&f);
        ts << cfg;
    }

    if (!m_exeProcess) {
        m_exeProcess = new QProcess(this);
        connect(m_exeProcess, &QProcess::readyReadStandardOutput, this, &PackageTool::onExeProcessStdOut);
        connect(m_exeProcess, &QProcess::readyReadStandardError, this, &PackageTool::onExeProcessStdErr);
        connect(m_exeProcess, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this, &PackageTool::onExeProcessFinished);
    }

    m_exeStdOutBuf.clear();
    m_exeStdErrBuf.clear();
    m_isPackaging = true;
    ui.btn_startPackage->setEnabled(false);

    QStringList args;
    args << "a";

    // Ultra/Fast 压缩档位参数
    // 对 EXE 我们希望 solid 一般开启（被固实打包可提升比率），故固实策略固化为 true
    args << buildCompressionArgs(m_packageLevel, /*solidPreferred=*/true);

    // 递归
    args << "-r";

    // 输出进度
    args << "-bsp1";

    // 目标 + 要打包的顶层目录名
    args << m_exeArchivePath
        << baseName;

	m_exeProcess->setWorkingDirectory(parentDir.absolutePath());
    m_exeProcess->start(sevenZPath, args);
    if (!m_exeProcess->waitForStarted(3000)) {
        m_isPackaging = false;
        ui.btn_startPackage->setEnabled(true);
        QMessageBox::critical(this, tr("错误"), tr("启动 7z 失败。"));
        return;
    }
}

void PackageTool::onExeProcessStdOut()
{
    m_exeStdOutBuf += m_exeProcess->readAllStandardOutput();
    QList<QByteArray> lines = m_exeStdOutBuf.split('\n');
    if (!m_exeStdOutBuf.endsWith('\n'))
        m_exeStdOutBuf = lines.takeLast();
    else
        m_exeStdOutBuf.clear();

    QRegularExpression re(R"((\d+)%\s*)");
    for (const auto& line : lines) {
        QString s = QString::fromLocal8Bit(line).trimmed();
        auto m = re.match(s);
        if (m.hasMatch()) {
            int percent = m.captured(1).toInt();
            if (percent >= 0 && percent <= 100)
                ui.progressBar->setValue(percent);
        }
    }
}

void PackageTool::onExeProcessStdErr()
{
    m_exeStdErrBuf += m_exeProcess->readAllStandardError();
}

void PackageTool::onExeProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    auto fail = [&](const QString& msg) {
        QMessageBox::critical(this, tr("打包失败"), msg);
        if (QFile::exists(m_exeOutputFile))
            QFile::remove(m_exeOutputFile);
        m_isPackaging = false;
        ui.btn_startPackage->setEnabled(true);
        ui.progressBar->setRange(0, 100);
        };

    bool okArchive = (status == QProcess::NormalExit) && (exitCode == 0) && QFile::exists(m_exeArchivePath);
    if (!okArchive) {
        QString reason;
        switch (exitCode) {
        case 0: reason = tr("未知错误/文件缺失"); break;
        case 1: reason = tr("警告"); break;
        case 2: reason = tr("致命错误"); break;
        case 7: reason = tr("参数错误"); break;
        case 8: reason = tr("内存不足"); break;
        case 255: reason = tr("用户中断"); break;
        default: reason = tr("其它错误");
        }
        fail(tr("压缩阶段失败。退出码: %1 (%2)\nstdout:\n%3\nstderr:\n%4")
            .arg(exitCode)
            .arg(reason)
            .arg(QString::fromLocal8Bit(m_exeStdOutBuf))
            .arg(QString::fromLocal8Bit(m_exeStdErrBuf)));
        return;
    }

    auto appendFile = [](QFile& out, const QString& inPath) -> bool {
        QFile in(inPath);
        if (!in.open(QIODevice::ReadOnly)) return false;
        constexpr qint64 BUF = 1 << 20;
        QByteArray buffer;
        buffer.resize(BUF);
        while (true) {
            qint64 r = in.read(buffer.data(), buffer.size());
            if (r < 0) return false;
            if (r == 0) break;
            if (out.write(buffer.constData(), r) != r) return false;
        }
        return true;
        };

    QFile out(m_exeOutputFile);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        fail(tr("无法创建输出文件: %1").arg(m_exeOutputFile));
        return;
    }
    bool ok =
        appendFile(out, m_exeSfxModule) &&
        appendFile(out, m_exeConfigPath) &&
        appendFile(out, m_exeArchivePath);
    out.close();

    if (!ok || !QFile::exists(m_exeOutputFile)) {
        fail(tr("拼接自解压安装包失败。"));
        return;
    }

    ui.progressBar->setValue(100);
    QMessageBox::information(this, tr("成功"),
        tr("已生成%1: %2")
        .arg(m_exeMainProgram.isEmpty() ? tr("自解压包") : tr("安装包"))
        .arg(m_exeOutputFile));

    m_isPackaging = false;
    ui.btn_startPackage->setEnabled(true);
    ui.progressBar->setRange(0, 100);
}
