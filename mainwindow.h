#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "discordcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    StateSetting stateSetting;
    DiscordController *discordController;

private slots:
    void saveSetting();

};

#endif // MAINWINDOW_H
