#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QActionGroup>
#include <QToolButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QMdiSubWindow>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include <QScreen>
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QVideoSink>
#include <QAudioOutput>
#include <QDebug>
#include <QTextDocumentFragment>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/images/hq.ico"));

    QMenu *editMenu = ui->menuBar->addMenu(tr("编辑(&E)")); // 添加编辑菜单
    QAction *action_Open = editMenu->addAction(             // 添加打开菜单
    QIcon(":/images/open.png"), tr("打开文件(&O)"));
    action_Open->setShortcut(QKeySequence("Ctrl+O"));       // 设置快捷键
    ui->mainToolBar->addAction(action_Open);                // 在工具栏中添加动作
    connect(action_Open, &QAction::triggered, this, &MainWindow::on_action_Open_triggered);

    QAction *action_Open_ascii_img = editMenu->addAction(             // 添加打开菜单
        tr("打开图片 in ascii"));     // 设置快捷键
    ui->mainToolBar->addAction(action_Open_ascii_img);         // 在工具栏中添加动作
    connect(action_Open_ascii_img, &QAction::triggered, this, &MainWindow::on_action_Open_Ascii_Img_triggered);

    QAction *action_Open_ascii_video = editMenu->addAction(             // 添加打开菜单
        tr("打开视频 in ascii"));     // 设置快捷键
    ui->mainToolBar->addAction(action_Open_ascii_video);         // 在工具栏中添加动作
    connect(action_Open_ascii_video, &QAction::triggered, this, &MainWindow::on_action_Open_Ascii_Video_triggered);

    QActionGroup *group = new QActionGroup(this);           // 建立动作组
    QAction *action_L = group->addAction(tr("左对齐(&L)")); // 向动作组中添加动作
    action_L->setCheckable(true);                  // 设置动作checkable属性为true
    QAction *action_R = group->addAction(tr("右对齐(&R)"));
    action_R->setCheckable(true);
    QAction *action_C = group->addAction(tr("居中(&C)"));
    action_C->setCheckable(true);
    action_L->setChecked(true);                    // 最后指定action_L为选中状态
    editMenu->addSeparator();                      // 向菜单中添加间隔器
    editMenu->addAction(action_L);                 // 向菜单中添加动作
    editMenu->addAction(action_R);
    editMenu->addAction(action_C);

    QToolButton *toolBtn = new QToolButton(this);          // 创建QToolButton
    toolBtn->setText(tr("颜色"));
    QMenu *colorMenu = new QMenu(this);                // 创建一个菜单
    colorMenu->addAction(tr("红色"));
    colorMenu->addAction(tr("绿色"));
    toolBtn->setMenu(colorMenu);                        // 添加菜单
    toolBtn->setPopupMode(QToolButton::MenuButtonPopup); // 设置弹出模式
    ui->mainToolBar->addWidget(toolBtn);                 // 向工具栏添加QToolButton按钮

    QSpinBox *spinBox = new QSpinBox(this);             // 创建QSpinBox
    ui->mainToolBar->addWidget(spinBox);                // 向工具栏添加QSpinBox部件


    // // 显示临时消息，显示2000毫秒即2秒
    // ui->statusBar->showMessage(tr("欢迎使用多文档编辑器"), 2000);
    // 创建标签，设置标签样式并显示信息，然后将其以永久部件的形式添加到状态栏
    QLabel *permanent = new QLabel(this);
    permanent->setFrameStyle(QFrame::Box | QFrame::Sunken);
    permanent->setText("www.qter.org");
    ui->statusBar->addPermanentWidget(permanent);

  //  ui->statusBar->setSizeGripEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_N_triggered()
{
    // 新建文本编辑器部件
    QTextEdit *edit = new QTextEdit(this);
    // 使用QMdiArea类的addSubWindow()函数创建子窗口，以文本编辑器为中心部件
    QMdiSubWindow *child = ui->mdiArea->addSubWindow(edit);
    child->setWindowTitle(tr("多文档编辑器子窗口"));
    child->show();
}


void MainWindow::on_action_Dock_triggered()
{
    ui->dockWidget->show();
}

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), QDir::homePath(), tr("文本文件 (*.txt);;"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("无法打开文件"), tr("无法打开文件 '%1': %2").arg(fileName).arg(file.errorString()));
        return;
    }

    // 读取文件内容
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    QTextEdit *edit = new QTextEdit(this);
    edit->setText(content);
    QMdiSubWindow *child = ui->mdiArea->addSubWindow(edit);
    child->setWindowTitle(tr("多文档编辑器子窗口"));
    child->show();
}

// 定义ASCII字符集，字符的灰度值从低到高排列
const char* asciiChars = "@%#*+=-:. ";
const int charLen = strlen(asciiChars);

void displayAscii(QTextDocument *document, QImage *image)
{
    document->clear();

    QTextCharFormat format;
    QTextCursor cursor(document);
    cursor.beginEditBlock();

    for (int y = 0; y < image->height(); ++y)
    {
        for (int x = 0; x < image->width(); ++x)
        {
            QRgb pixel = image->pixel(x, y);

            int grayValue = qGray(pixel);
            int index = (grayValue * (charLen - 1)) / 255;
            char asciiChar = asciiChars[index];

            format.setForeground(QColor(pixel));
            cursor.insertText(QString(asciiChar), format);
        }
        cursor.insertText("\n");
    }

    cursor.endEditBlock();
}

void MainWindow::on_action_Open_Ascii_Img_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择图片"), QDir::homePath(), tr("图片文件 (*.jpg *.png)"));
    if (fileName.isEmpty()) {
        return;
    }

    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::warning(this, tr("错误"), tr("无法加载图片 '%1'").arg(fileName));
        return;
    }

    // 获取屏幕尺寸
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    int originalWidth = image.width();
    int originalHeight = image.height();

    double screenAspect = static_cast<double>(screenWidth) / screenHeight;
    double imageAspect = static_cast<double>(originalWidth) / originalHeight;

    double scaleFactor;
    if (screenAspect >= imageAspect) {
        scaleFactor = static_cast<double>(screenHeight * 0.8) / originalHeight;
    } else {
        scaleFactor = static_cast<double>(screenWidth * 0.8) / originalWidth;
    }

    int targetWidth = static_cast<int>(originalWidth / 8 * scaleFactor);
    int targetHeight = static_cast<int>(originalHeight / 16 * scaleFactor);

    QImage scaledImage = image.scaled(targetWidth, targetHeight);

    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    QTextDocument *document = textEdit->document();

    QFont font = QFont("Courier");
    font.setPointSize(8);
    textEdit->setFont(font);
    textEdit->setAcceptRichText(false);
    textEdit->setReadOnly(true);

    displayAscii(document, &scaledImage);

    QMdiSubWindow *child = ui->mdiArea->addSubWindow(textEdit);
    child->setWindowTitle(tr("ASCII 图像 - %1").arg(QFileInfo(fileName).fileName()));
    child->show();
    child->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QSizeF size = document->size();
    child->resize(size.width() + 32, size.height() + 32);
}

void MainWindow::on_action_Open_Ascii_Video_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择视频"), QDir::homePath(), tr("视频文件 (*.mp4 *.avi *.mov)"));
    if (fileName.isEmpty()) {
        return;
    }

    // 创建 QMediaPlayer
    QMediaPlayer *player = new QMediaPlayer(this);

    // 创建 QVideoSink
    QVideoSink *videoSink = new QVideoSink(this);
    player->setVideoSink(videoSink);

    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    QTextDocument *document = textEdit->document();

    QFont font = QFont("Courier");
    font.setPointSize(8);
    textEdit->setFont(font);
    textEdit->setAcceptRichText(false);
    textEdit->setReadOnly(true);

    // 获取屏幕尺寸
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    QMdiSubWindow *child = ui->mdiArea->addSubWindow(textEdit);
    child->setWindowTitle(tr("ASCII 视频 - %1").arg(QFileInfo(fileName).fileName()));
    child->show();
    child->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 连接 QVideoSink 的 videoFrameChanged 信号
    connect(videoSink, &QVideoSink::videoFrameChanged, this, [this, player, videoSink, textEdit, document, child, screenWidth, screenHeight](const QVideoFrame &frame) {
        disconnect(videoSink, &QVideoSink::videoFrameChanged, this, nullptr);
        player->stop();

        QImage image = frame.toImage();

        int originalWidth = image.width();
        int originalHeight = image.height();

        double screenAspect = static_cast<double>(screenWidth) / screenHeight;
        double imageAspect = static_cast<double>(originalWidth) / originalHeight;

        double scaleFactor;
        if (screenAspect >= imageAspect) {
            scaleFactor = static_cast<double>(screenHeight * 0.8) / originalHeight;
        } else {
            scaleFactor = static_cast<double>(screenWidth * 0.8) / originalWidth;
        }

        int targetWidth = static_cast<int>(originalWidth / 8 * scaleFactor);
        int targetHeight = static_cast<int>(originalHeight / 16 * scaleFactor);

        QImage scaledImage = image.scaled(targetWidth, targetHeight);

        displayAscii(document, &scaledImage);

        QSizeF size = document->size();
        child->resize(size.width() + 32, size.height() + 32);

        connect(videoSink, &QVideoSink::videoFrameChanged, this, [document, targetWidth, targetHeight](const QVideoFrame &frame) {
            QImage image = frame.toImage().scaled(targetWidth, targetHeight);
            displayAscii(document, &image);
        });

        player->play();
    });

    player->setSource(QUrl::fromLocalFile(fileName));
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->play();

    connect(child, &QMdiSubWindow::destroyed, this, [player, videoSink, this]() {
        disconnect(videoSink, &QVideoSink::videoFrameChanged, this, nullptr);
        player->stop();
        player->deleteLater();
    });

    connect(this, &QMainWindow::destroyed, this, [player, videoSink, this]() {
        disconnect(videoSink, &QVideoSink::videoFrameChanged, this, nullptr);
        player->stop();
        player->deleteLater();
    });
}
