#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_N_triggered();

    void on_action_Dock_triggered();

    void on_action_Open_triggered();

    void on_action_Open_Ascii_Img_triggered();

    void on_action_Open_Ascii_Video_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
