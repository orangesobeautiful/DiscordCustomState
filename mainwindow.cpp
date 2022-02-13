#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "discordcontroller.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->centralLayout);
    discordController = new DiscordController(this);
    discordController->start();

    stateSetting = ReadSetting();
    ui->clientIDLineEdit->setText(QString::number(stateSetting.clientID));
    ui->detailsLineEdit->setText(stateSetting.details);
    ui->stateLineEdit->setText(stateSetting.state);
    ui->startTimestampLineEdit->setText(QString::number(stateSetting.startTimeStamp));
    ui->endTimestampLineEdit->setText(QString::number(stateSetting.endTimeStamp));
    ui->largeImageLineEdit->setText(stateSetting.largeImage);
    ui->largeTooltipLineEdit->setText(stateSetting.largeTooltip);
    ui->smallImageLineEdit->setText(stateSetting.smallImage);
    ui->smallTooltipLineEdit->setText(stateSetting.smallTooltip);
    connect(ui->changePushButton, SIGNAL(clicked()), this, SLOT(saveSetting()));
}

MainWindow::~MainWindow()
{
    discordController->deleteLater();
    delete ui;
}

void MainWindow::saveSetting(){
    stateSetting.clientID = ui->clientIDLineEdit->text().toLongLong();

    stateSetting.details = ui->detailsLineEdit->text();
    stateSetting.state = ui->stateLineEdit->text();
    stateSetting.startTimeStamp = ui->startTimestampLineEdit->text().toLongLong();
    stateSetting.endTimeStamp = ui->endTimestampLineEdit->text().toLongLong();

    stateSetting.largeImage = ui->largeImageLineEdit->text();
    stateSetting.largeTooltip = ui->largeTooltipLineEdit->text();
    stateSetting.smallImage = ui->smallImageLineEdit->text();
    stateSetting.smallTooltip = ui->smallTooltipLineEdit->text();

    ChangeSetting(stateSetting);
}



