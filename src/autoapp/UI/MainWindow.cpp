/*
*  This file is part of openauto project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  openauto is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  openauto is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with openauto. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <f1x/openauto/autoapp/UI/MainWindow.hpp>
#include <QFileInfo>
#include <QFile>
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QTextStream>
#include <QFontDatabase>
#include <QFont>
#include <QScreen>
#include <QRect>
#include <QVideoWidget>
#include <QNetworkInterface>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <f1x/openauto/Common/Log.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

MainWindow::MainWindow(configuration::IConfiguration::Pointer configuration, QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
    , localDevice(new QBluetoothLocalDevice)
{
    // set default bg color to black
    this->setStyleSheet("QMainWindow {background-color: rgb(0,0,0);}");

    // // Set default font and size
    // int id = QFontDatabase::addApplicationFont(":/Roboto-Regular.ttf");
    // QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    // QFont _font(family, 11);
    // qApp->setFont(_font);

    this->configuration_ = configuration;

    // trigger files

    ui_->setupUi(this);
    connect(ui_->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::openSettings);
    // connect(ui_->pushButtonExit, &QPushButton::clicked, this, &MainWindow::exit);
    connect(ui_->pushButtonToggleCursor, &QPushButton::clicked, this, &MainWindow::toggleCursor);
    connect(ui_->pushButtonWirelessConnection, &QPushButton::clicked, this, &MainWindow::openConnectDialog);
    // QVideoWidget* aaOut;
    // aaOut = new QVideoWidget();
    // aaOut->setVideoOutput(ui_->aaVideoWidget);
    // ui_->aaVideoWidget = aaOut;
    // ui_->addWidget(aaOut);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

}
}
}
}



void f1x::openauto::autoapp::ui::MainWindow::updateAlpha()
{
  int value = 0;
    // int value = configuration_->getAlphaTrans();
    //int n = snprintf(this->alpha_str, 5, "%d", value);

    if (value != this->alpha_current_str) {
        this->alpha_current_str = value;
        double alpha = value/100.0;
        QString alp=QString::number(alpha);
        ui_->pushButtonSettings->setStyleSheet( "background-color: rgba(138, 226, 52, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    }
}


void f1x::openauto::autoapp::ui::MainWindow::setRetryUSBConnect()
{

    QTimer::singleShot(10000, this, SLOT(resetRetryUSBMessage()));
}


bool f1x::openauto::autoapp::ui::MainWindow::check_file_exist(const char *fileName)
{
    std::ifstream ifile(fileName, std::ios::in);
    // file not ok - checking if symlink
    if (!ifile.good()) {
        QFileInfo linkFile = QString(fileName);
        if (linkFile.isSymLink()) {
            QFileInfo linkTarget(linkFile.symLinkTarget());
            return linkTarget.exists();
        } else {
            return ifile.good();
        }
    } else {
        return ifile.good();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::tmpChanged()
{
    try {
        if (std::ifstream("/tmp/entityexit")) {
            MainWindow::TriggerAppStop();
            std::remove("/tmp/entityexit");
        }
    } catch (...) {
        OPENAUTO_LOG(error) << "[OpenAuto] Error in entityexit";
    }






    // update day/night state
    // check if shutdown is external triggered and init clean app exit
    if (std::ifstream("/tmp/external_exit")) {
        f1x::openauto::autoapp::ui::MainWindow::MainWindow::exit();
    }

    this->hotspotActive = check_file_exist("/tmp/hotspot_active");


    MainWindow::updateAlpha();


}
