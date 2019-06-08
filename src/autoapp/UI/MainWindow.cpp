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

    // Set default font and size
    int id = QFontDatabase::addApplicationFont(":/Roboto-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont _font(family, 11);
    qApp->setFont(_font);

    this->configuration_ = configuration;

    // trigger files
    this->nightModeEnabled = check_file_exist(this->nightModeFile);
    this->devModeEnabled = check_file_exist(this->devModeFile);
    this->wifiButtonForce = check_file_exist(this->wifiButtonFile);
    this->cameraButtonForce = check_file_exist(this->cameraButtonFile);
    this->brightnessButtonForce = check_file_exist(this->brightnessButtonFile);
    this->systemDebugmode = check_file_exist(this->debugModeFile);
    this->lightsensor = check_file_exist(this->lsFile);
    this->c1ButtonForce = check_file_exist(this->custom_button_file_c1);
    this->c2ButtonForce = check_file_exist(this->custom_button_file_c2);
    this->c3ButtonForce = check_file_exist(this->custom_button_file_c3);
    this->c4ButtonForce = check_file_exist(this->custom_button_file_c4);
    this->c5ButtonForce = check_file_exist(this->custom_button_file_c5);
    this->c6ButtonForce = check_file_exist(this->custom_button_file_c6);

    // wallpaper stuff
    this->wallpaperDayFileExists = check_file_exist("wallpaper.png");
    this->wallpaperNightFileExists = check_file_exist("wallpaper-night.png");
    this->wallpaperClassicDayFileExists = check_file_exist("wallpaper-classic.png");
    this->wallpaperClassicNightFileExists = check_file_exist("wallpaper-classic-night.png");
    this->wallpaperEQFileExists = check_file_exist("wallpaper-eq.png");

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

void f1x::openauto::autoapp::ui::MainWindow::hostModeStateChanged(QBluetoothLocalDevice::HostMode mode)
{
    if (mode != QBluetoothLocalDevice::HostPoweredOff) {
        this->bluetoothEnabled = true;
        ui_->pushButtonBluetooth->show();
        if (std::ifstream("/tmp/bluetooth_pairable")) {
            ui_->labelBluetoothPairable->show();
            ui_->pushButtonBluetooth->hide();
        } else {
            ui_->labelBluetoothPairable->hide();
        }
    } else {
        this->bluetoothEnabled = false;
        ui_->pushButtonBluetooth->hide();
        ui_->labelBluetoothPairable->hide();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::updateNetworkInfo()
{
    QNetworkInterface wlan0if = QNetworkInterface::interfaceFromName("wlan0");
    if (wlan0if.flags().testFlag(QNetworkInterface::IsUp)) {
        QList<QNetworkAddressEntry> entrieswlan0 = wlan0if.addressEntries();
        if (!entrieswlan0.isEmpty()) {
            QNetworkAddressEntry wlan0 = entrieswlan0.first();
            //qDebug() << "wlan0: " << wlan0.ip();
            ui_->value_ip->setText(wlan0.ip().toString().simplified());
            ui_->value_mask->setText(wlan0.netmask().toString().simplified());
            if (std::ifstream("/tmp/hotspot_active")) {
                ui_->value_ssid->setText(configuration_->getParamFromFile("/etc/hostapd/hostapd.conf","ssid"));
            } else {
                ui_->value_ssid->setText(configuration_->readFileContent("/tmp/wifi_ssid"));
            }
            ui_->value_gw->setText(configuration_->readFileContent("/tmp/gateway_wlan0"));
        }
    } else {
        //qDebug() << "wlan0: down";
        ui_->value_ip->setText("");
        ui_->value_mask->setText("");
        ui_->value_gw->setText("");
        ui_->value_ssid->setText("wlan0: down");
    }
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed1()
{
    system(qPrintable(this->custom_button_command_c1 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed2()
{
    system(qPrintable(this->custom_button_command_c2 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed3()
{
    system(qPrintable(this->custom_button_command_c3 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed4()
{
    system(qPrintable(this->custom_button_command_c4 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed5()
{
    system(qPrintable(this->custom_button_command_c5 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed6()
{
    system(qPrintable(this->custom_button_command_c6 + " &"));
}


void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonBrightness_clicked()
{
    this->brightnessFile = new QFile(this->brightnessFilename);
    this->brightnessFileAlt = new QFile(this->brightnessFilenameAlt);

    // Get the current brightness value
    if (!this->customBrightnessControl) {
        if (this->brightnessFile->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFile->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            QString bri=QString::number(brightness_val);
            ui_->brightnessValueLabel->setText(bri);
            this->brightnessFile->close();
        }
    } else {
        if (this->brightnessFileAlt->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFileAlt->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            QString bri=QString::number(brightness_val);
            ui_->brightnessValueLabel->setText(bri);
            this->brightnessFileAlt->close();
        }
    }
    ui_->BrightnessSliderControl->show();
    ui_->VolumeSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonBrightness2_clicked()
{
    this->brightnessFile = new QFile(this->brightnessFilename);
    this->brightnessFileAlt = new QFile(this->brightnessFilenameAlt);

    // Get the current brightness value
    if (!this->customBrightnessControl) {
        if (this->brightnessFile->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFile->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            QString bri=QString::number(brightness_val);
            ui_->brightnessValueLabel->setText(bri);
            this->brightnessFile->close();
        }
    } else {
        if (this->brightnessFileAlt->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFileAlt->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            QString bri=QString::number(brightness_val);
            ui_->brightnessValueLabel->setText(bri);
            this->brightnessFileAlt->close();
        }
    }
    ui_->BrightnessSliderControl->show();
    ui_->VolumeSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonVolume_clicked()
{
    ui_->horizontalSliderVolume->show();
    ui_->volumeValueLabel->show();
    if (this->toggleMute) {
        ui_->pushButtonUnMute->show();
    } else {
        ui_->pushButtonMute->show();
    }
    ui_->VolumeSliderControl->show();
    ui_->BrightnessSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonVolume2_clicked()
{
    ui_->horizontalSliderVolume->show();
    ui_->volumeValueLabel->show();
    if (this->toggleMute) {
        ui_->pushButtonUnMute->show();
    } else {
        ui_->pushButtonMute->show();
    }
    ui_->VolumeSliderControl->show();
    ui_->BrightnessSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderBrightness_valueChanged(int value)
{
    int n = snprintf(this->brightness_str, 5, "%d", value);
    this->brightnessFile = new QFile(this->brightnessFilename);
    this->brightnessFileAlt = new QFile(this->brightnessFilenameAlt);

    if (!this->customBrightnessControl) {
        if (this->brightnessFile->open(QIODevice::WriteOnly)) {
            this->brightness_str[n] = '\n';
            this->brightness_str[n+1] = '\0';
            this->brightnessFile->write(this->brightness_str);
            this->brightnessFile->close();
        }
    } else {
        if (this->brightnessFileAlt->open(QIODevice::WriteOnly)) {
            this->brightness_str[n] = '\n';
            this->brightness_str[n+1] = '\0';
            this->brightnessFileAlt->write(this->brightness_str);
            this->brightnessFileAlt->close();
        }
    }
    QString bri=QString::number(value);
    ui_->brightnessValueLabel->setText(bri);
}

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderVolume_valueChanged(int value)
{
    int n = snprintf(this->volume_str, 5, "%d", value);
    QString vol=QString::number(value);
    ui_->volumeValueLabel->setText(vol+"%");
    system(("/usr/local/bin/autoapp_helper setvolume " + std::to_string(value) + "&").c_str());
}

void f1x::openauto::autoapp::ui::MainWindow::updateAlpha()
{
    int value = configuration_->getAlphaTrans();
    //int n = snprintf(this->alpha_str, 5, "%d", value);

    if (value != this->alpha_current_str) {
        this->alpha_current_str = value;
        double alpha = value/100.0;
        QString alp=QString::number(alpha);
        ui_->pushButtonExit->setStyleSheet( "background-color: rgba(164, 0, 0, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonShutdown->setStyleSheet( "background-color: rgba(239, 41, 41, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonReboot->setStyleSheet( "background-color: rgba(252, 175, 62, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonCancel->setStyleSheet( "background-color: rgba(32, 74, 135, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonBrightness->setStyleSheet( "background-color: rgba(245, 121, 0, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonVolume->setStyleSheet( "background-color: rgba(64, 191, 191, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonLock->setStyleSheet( "background-color: rgba(15, 54, 5, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonSettings->setStyleSheet( "background-color: rgba(138, 226, 52, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonDay->setStyleSheet( "background: rgba(252, 233, 79, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonNight->setStyleSheet( "background-color: rgba(114, 159, 207, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonCameraShow->setStyleSheet( "background-color: rgba(100, 62, 4, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonWifi->setStyleSheet( "background-color: rgba(252, 175, 62, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonToggleGUI->setStyleSheet( "background-color: rgba(237, 164, 255, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButton_c1->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c1 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c2->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c2 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c3->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c3 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c4->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c4 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c5->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c5 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c6->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c6 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButtonDummy1->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonDummy2->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonDummy3->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonDebug->setStyleSheet( "background-color: rgba(85, 87, 83, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonMusic->setStyleSheet( "background-color: rgba(78, 154, 6, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButtonAndroidAuto->setStyleSheet( "background-color: rgba(48, 140, 198, " + alp + " ); border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255); border-bottom: 0px; border-top: 0px;");
        ui_->labelAndroidAutoBottom->setStyleSheet( "background-color: rgba(48, 140, 198, " + alp + " ); border-bottom-left-radius: 4px; border-bottom-right-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255); border-top: 0px;");
        ui_->labelAndroidAutoTop->setStyleSheet( "background-color: rgba(48, 140, 198, " + alp + " ); border-top-left-radius: 4px; border-top-right-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255); border-bottom: 0px;");
        ui_->pushButtonNoDevice->setStyleSheet( "background-color: rgba(48, 140, 198, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButtonNoWiFiDevice->setStyleSheet( "background-color: rgba(252, 175, 62, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        // old style
        ui_->pushButtonSettings2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonLock2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonMusic2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonBrightness2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonToggleGUI2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonExit2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonShutdown2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonReboot2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonCancel2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonAndroidAuto2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonNoDevice2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonWifi2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonNoWiFiDevice2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonDay2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonNight2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonCameraShow2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonVolume2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
        ui_->pushButtonDebug2->setStyleSheet( "background-color: rgba(136, 138, 133, " + alp + " ); color: rgb(255, 255, 255); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); outline: none;");
    }
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToNight()
{
    //MainWindow::on_pushButtonVolume_clicked();
    f1x::openauto::autoapp::ui::MainWindow::updateBG();
    ui_->pushButtonDay->show();
    ui_->pushButtonDay2->show();
    ui_->pushButtonNight->hide();
    ui_->pushButtonNight2->hide();
    ui_->BrightnessSliderControl->hide();
    if (ui_->mediaWidget->isVisible() == true) {
        ui_->VolumeSliderControl->hide();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToDay()
{
    //MainWindow::on_pushButtonVolume_clicked();
    f1x::openauto::autoapp::ui::MainWindow::updateBG();
    ui_->pushButtonNight->show();
    ui_->pushButtonNight2->show();
    ui_->pushButtonDay->hide();
    ui_->pushButtonDay2->hide();
    ui_->BrightnessSliderControl->hide();
    if (ui_->mediaWidget->isVisible() == true) {
        ui_->VolumeSliderControl->hide();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::cameraControlHide()
{
    if (this->cameraButtonForce) {
        ui_->cameraWidget->hide();
        if (!this->oldGUIStyle) {
            ui_->menuWidget->show();
        } else {
            ui_->oldmenuWidget->show();
        }
    }
}

void f1x::openauto::autoapp::ui::MainWindow::cameraControlShow()
{
    if (this->cameraButtonForce) {
        if (!this->oldGUIStyle) {
            ui_->menuWidget->hide();
        } else {
            ui_->oldmenuWidget->hide();
        }
        ui_->cameraWidget->show();

        // check if dashcam is recording
        if (std::ifstream("/tmp/dashcam_is_recording")) {
            if (ui_->pushButtonRecordActive->isVisible() == false) {
                ui_->pushButtonRecordActive->show();
                ui_->pushButtonRecord->hide();
            }
        } else {
            if (ui_->pushButtonRecordActive->isVisible() == true) {
                ui_->pushButtonRecordActive->hide();
                ui_->pushButtonRecord->show();
            }
        }
    }
}

void f1x::openauto::autoapp::ui::MainWindow::playerShow()
{
    if (this->wallpaperEQFileExists) {
        this->setStyleSheet("QMainWindow { background: url(wallpaper-eq.png); background-repeat: no-repeat; background-position: center; }");
    } else {
        this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
    }

    if (!this->oldGUIStyle) {
        ui_->menuWidget->hide();
    } else {
        ui_->oldmenuWidget->hide();
    }
    ui_->mediaWidget->show();
    ui_->VolumeSliderControlPlayer->show();
    ui_->VolumeSliderControl->hide();
    ui_->BrightnessSliderControl->hide();
    ui_->networkInfo->hide();
    if (player->state() == QMediaPlayer::PlayingState) {
        on_pushButtonBackToPlayer_clicked();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::playerHide()
{
    if (!this->oldGUIStyle) {
        ui_->menuWidget->show();
    } else {
        ui_->oldmenuWidget->show();
    }
    ui_->mediaWidget->hide();
    ui_->VolumeSliderControl->show();
    ui_->VolumeSliderControlPlayer->hide();
    ui_->BrightnessSliderControl->hide();
    if (configuration_->showNetworkinfo()) {
        ui_->networkInfo->hide();
    }
    f1x::openauto::autoapp::ui::MainWindow::updateBG();
    f1x::openauto::autoapp::ui::MainWindow::tmpChanged();
}

void f1x::openauto::autoapp::ui::MainWindow::toggleExit()
{
    if (!this->exitMenuVisible) {
        ui_->exitWidget->show();
        ui_->buttonWidget->hide();
        ui_->horizontalWidgetButtons->hide();
        ui_->horizontalWidgetPower->show();
        this->exitMenuVisible = true;
    } else {
        ui_->buttonWidget->show();
        ui_->exitWidget->hide();
        ui_->horizontalWidgetButtons->show();
        ui_->horizontalWidgetPower->hide();
        this->exitMenuVisible = false;
    }
}

void f1x::openauto::autoapp::ui::MainWindow::toggleMuteButton()
{
    if (!this->toggleMute) {
        ui_->pushButtonMute->hide();
        ui_->pushButtonUnMute->show();
        this->toggleMute = true;
    } else {
        ui_->pushButtonUnMute->hide();
        ui_->pushButtonMute->show();
        this->toggleMute = false;
    }
}

void f1x::openauto::autoapp::ui::MainWindow::toggleGUI()
{
    if (!this->oldGUIStyle) {
        ui_->oldmenuWidget->show();
        ui_->menuWidget->hide();
        this->oldGUIStyle = true;
        if (!this->NoClock) {
            if (UseBigClock) {
                ui_->Digital_clock->hide();
                ui_->bigClock->show();
            } else {
                ui_->Digital_clock->show();
                ui_->bigClock->hide();
            }
        }
        MainWindow::on_pushButtonVolume_clicked();
    } else {
        ui_->menuWidget->show();
        ui_->oldmenuWidget->hide();
        this->oldGUIStyle = false;
        if (!this->NoClock) {
            ui_->Digital_clock->show();
        }
    }
    f1x::openauto::autoapp::ui::MainWindow::updateBG();
    f1x::openauto::autoapp::ui::MainWindow::tmpChanged();
}

void f1x::openauto::autoapp::ui::MainWindow::updateBG()
{
    if (this->date_text == "12/24") {
        this->setStyleSheet("QMainWindow { background: url(:/christmas.png); background-repeat: no-repeat; background-position: center; }");
        this->holidaybg = true;
    }
    else if (this->date_text == "12/31") {
        this->setStyleSheet("QMainWindow { background: url(:/firework.png); background-repeat: no-repeat; background-position: center; }");
        this->holidaybg = true;
    }
    if (!this->nightModeEnabled) {
        if (ui_->mediaWidget->isVisible() == true) {
            if (this->wallpaperEQFileExists) {
                this->setStyleSheet("QMainWindow { background: url(wallpaper-eq.png); background-repeat: no-repeat; background-position: center; }");
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        } else {
            if (this->oldGUIStyle) {
                if (this->wallpaperClassicDayFileExists) {
                    this->setStyleSheet("QMainWindow { background: url(wallpaper-classic.png); background-repeat: no-repeat; background-position: center; }");
                } else {
                    this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                }
            } else {
                if (this->wallpaperDayFileExists) {
                    this->setStyleSheet("QMainWindow { background: url(wallpaper.png); background-repeat: no-repeat; background-position: center; }");
                } else {
                    this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                }
            }
        }
    } else {
        if (ui_->mediaWidget->isVisible() == true) {
            if (this->wallpaperEQFileExists) {
                this->setStyleSheet("QMainWindow { background: url(wallpaper-eq.png); background-repeat: no-repeat; background-position: center; }");
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        } else {
            if (this->oldGUIStyle) {
                if (this->wallpaperClassicNightFileExists) {
                    this->setStyleSheet( "QMainWindow { background: url(wallpaper-classic-night.png); background-repeat: no-repeat; background-position: center; }");
                } else {
                    this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                }
            } else {
                if (this->wallpaperNightFileExists) {
                    this->setStyleSheet("QMainWindow { background: url(wallpaper-night.png); background-repeat: no-repeat; background-position: center; }");
                } else {
                    this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                }
            }
        }
    }
}

void f1x::openauto::autoapp::ui::MainWindow::createDebuglog()
{
    system("/usr/local/bin/crankshaft debuglog &");
}

void f1x::openauto::autoapp::ui::MainWindow::setPairable()
{
    system("/usr/local/bin/crankshaft bluetooth pairable &");
}

void f1x::openauto::autoapp::ui::MainWindow::setMute()
{
    system("/usr/local/bin/autoapp_helper setmute &");
}

void f1x::openauto::autoapp::ui::MainWindow::setUnMute()
{
    system("/usr/local/bin/autoapp_helper setunmute &");
}

void f1x::openauto::autoapp::ui::MainWindow::showTime()
{
    QTime time=QTime::currentTime();
    QDate date=QDate::currentDate();
    QString time_text=time.toString("hh : mm : ss");
    this->date_text=date.toString("MM/dd");

    if ((time.second() % 2) == 0) {
        time_text[3] = ' ';
        time_text[8] = ' ';
    }

    ui_->Digital_clock->setText(time_text);
    ui_->bigClock->setText(time_text);
    ui_->bigClock2->setText(time_text);

    if (!this->holidaybg) {
        if (this->date_text == "12/24") {
            MainWindow::updateBG();
        }
        else if (this->date_text == "12/31") {
            MainWindow::updateBG();
        }
    }

    // check connected devices
    if (localDevice->isValid()) {
        QString localDeviceName = localDevice->name();
        QString localDeviceAddress = localDevice->address().toString();
        QList<QBluetoothAddress> btdevices;
        btdevices = localDevice->connectedDevices();

        int count = btdevices.count();
        if (count > 0) {
            //QBluetoothAddress btdevice = btdevices[0];
            //QString btmac = btdevice.toString();
            //ui_->btDeviceCount->setText(QString::number(count));
            if (ui_->btDevice->isVisible() == false) {
                ui_->btDevice->show();
            }
            if (std::ifstream("/tmp/btdevice")) {
                ui_->btDevice->setText(configuration_->readFileContent("/tmp/btdevice"));
            }
        } else {
            if (ui_->btDevice->isVisible() == true) {
                ui_->btDevice->hide();
                ui_->btDevice->setText("BT-Device");
            }
        }
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderProgressPlayer_sliderMoved(int position)
{
    player->setPosition(position);
}

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderVolumePlayer_sliderMoved(int position)
{
    player->setVolume(position);
    ui_->volumeValueLabelPlayer->setText(QString::number(position) + "%");
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonList_clicked()
{
    ui_->mp3selectWidget->show();
    ui_->PlayerPlayingWidget->hide();
    ui_->pushButtonList->hide();
    ui_->pushButtonPlayerPlayList->show();

    if (playlist->currentIndex() == -1) {
        ui_->pushButtonPlayerStop->hide();
        ui_->pushButtonPlayerPause->hide();
        ui_->pushButtonBackToPlayer->hide();
    } else {
        ui_->pushButtonBackToPlayer->show();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerStop_clicked()
{
    ui_->mp3List->setCurrentRow(playlist->currentIndex());
    player->stop();
    ui_->pushButtonBack->setIcon(QPixmap("://coverlogo.png"));
    ui_->pushButtonPlayerPause->setStyleSheet( "background-color: rgb(233, 185, 110); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(0,0,0);");
    ui_->mp3selectWidget->show();
    ui_->PlayerPlayingWidget->hide();
    ui_->pushButtonBackToPlayer->hide();
    ui_->pushButtonPlayerPlayList->show();
    ui_->pushButtonPlayerStop->hide();
    ui_->pushButtonList->hide();
    ui_->pushButtonPlayerPause->hide();
    ui_->playerPositionTime->setText("00:00 / 00:00");
    ui_->labelCurrentPlaying->setText("");
    ui_->labelTrack->setText("");
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerPause_clicked()
{
    {
        if(player->state() == QMediaPlayer::PlayingState){
            player->pause();
            ui_->pushButtonPlayerPause->setStyleSheet( "background-color: rgb(218, 143, 143); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(0,0,0);");
        }else{
            ui_->pushButtonPlayerPause->setStyleSheet( "background-color: rgb(233, 185, 110); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(0,0,0);");
            player->play();
            player->setPosition(player->position());
        }

    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_positionChanged(qint64 position)
{
    ui_->horizontalSliderProgressPlayer->setValue(position);

    //Setting the time
    QString time_elapsed, time_total;

    int total_seconds, total_minutes;

    total_seconds = (player->duration()/1000) % 60;
    total_minutes = (player->duration()/1000) / 60;

    if(total_minutes >= 60){
        int total_hours = (total_minutes/60);
        total_minutes = total_minutes - (total_hours*60);
        time_total = QString("%1").arg(total_hours, 2,10,QChar('0')) + ':' + QString("%1").arg(total_minutes, 2,10,QChar('0')) + ':' + QString("%1").arg(total_seconds, 2,10,QChar('0'));

    }else{
        time_total = QString("%1").arg(total_minutes, 2,10,QChar('0')) + ':' + QString("%1").arg(total_seconds, 2,10,QChar('0'));

    }

    //calculate time elapsed
    int seconds, minutes;

    seconds = (position/1000) % 60;
    minutes = (position/1000) / 60;

    //if minutes is over 60 then we should really display hours
    if(minutes >= 60){
        int hours = (minutes/60);
        minutes = minutes - (hours*60);
        time_elapsed = QString("%1").arg(hours, 2,10,QChar('0')) + ':' + QString("%1").arg(minutes, 2,10,QChar('0')) + ':' + QString("%1").arg(seconds, 2,10,QChar('0'));
    }else{
        time_elapsed = QString("%1").arg(minutes, 2,10,QChar('0')) + ':' + QString("%1").arg(seconds, 2,10,QChar('0'));
    }
    ui_->playerPositionTime->setText(time_elapsed + " / " + time_total);
}

void f1x::openauto::autoapp::ui::MainWindow::on_durationChanged(qint64 position)
{
    ui_->horizontalSliderProgressPlayer->setMaximum(position);
}

void f1x::openauto::autoapp::ui::MainWindow::on_mp3List_itemClicked(QListWidgetItem *item)
{
    this->selectedMp3file = item->text();
}

void f1x::openauto::autoapp::ui::MainWindow::metaDataChanged()
{
    QString fullpathplaying = player->currentMedia().canonicalUrl().toString();
    QString filename = QFileInfo(fullpathplaying).fileName();

    QImage img = player->metaData(QMediaMetaData::CoverArtImage).value<QImage>();
    QImage imgscaled = img.scaled(270,270,Qt::IgnoreAspectRatio);
    if (!imgscaled.isNull()) {
        ui_->pushButtonBack->setIcon(QPixmap::fromImage(imgscaled));
    } else {
        if (playlist->currentIndex() != -1 && fullpathplaying != "") {
            QString filename = ui_->mp3List->item(playlist->currentIndex())->text();
            QString cover = this->musicfolder + "/" + this->albumfolder + "/" + filename + ".png";
            if (check_file_exist(cover.toStdString().c_str())) {
                QPixmap img = cover;
                ui_->pushButtonBack->setIcon(img.scaled(270,270,Qt::KeepAspectRatio));
            } else {
                ui_->pushButtonBack->setIcon(QPixmap("://coverlogo.png"));
            }
        } else {
            ui_->pushButtonBack->setIcon(QPixmap("://coverlogo.png"));
        }
    }

    try {
        // use metadata from mp3list widget (prescanned id3 by taglib)
        if (playlist->currentIndex() != -1 && fullpathplaying != "") {
            QString currentsong = ui_->mp3List->item(playlist->currentIndex())->text();
            ui_->labelCurrentPlaying->setText(currentsong);
            if (currentsong.length() > 48) {
                int id = QFontDatabase::addApplicationFont(":/Roboto-Regular.ttf");
                QString family = QFontDatabase::applicationFontFamilies(id).at(0);
                QFont _font(family, 12, QFont::Bold);
                _font.setItalic(true);
                ui_->labelCurrentPlaying->setFont(_font);
            } else {
                int id = QFontDatabase::addApplicationFont(":/Roboto-Regular.ttf");
                QString family = QFontDatabase::applicationFontFamilies(id).at(0);
                QFont _font(family, 16, QFont::Bold);
                _font.setItalic(true);
                ui_->labelCurrentPlaying->setFont(_font);
            }
        }
    } catch (...) {
        // use metadata from player
        QString AlbumInterpret = player->metaData(QMediaMetaData::AlbumArtist).toString();
        QString Title = player->metaData(QMediaMetaData::Title).toString();

        if (AlbumInterpret == "" && ui_->comboBoxAlbum->currentText() != ".") {
            AlbumInterpret = ui_->comboBoxAlbum->currentText();
        }
        QString currentPlaying;

        if (AlbumInterpret != "") {
            currentPlaying.append(AlbumInterpret);
        }
        if (Title != "" && AlbumInterpret != "") {
            currentPlaying.append(" - ");
        }
        if (Title != "") {
            currentPlaying.append(Title);
        }
        ui_->labelCurrentPlaying->setText(currentPlaying);
    }
    ui_->labelTrack->setText(QString::number(playlist->currentIndex()+1));
    ui_->labelTrackCount->setText(QString::number(playlist->mediaCount()));

    if (playlist->currentIndex() == -1) {
        // check for folder icon
        QString cover = this->musicfolder + "/" + this->albumfolder + "/folder.png";
        QString coverjpg = this->musicfolder + "/" + this->albumfolder + "/folder.jpg";
        if (check_file_exist(cover.toStdString().c_str())) {
            QPixmap img = cover;
            ui_->pushButtonBack->setIcon(img.scaled(270,270,Qt::KeepAspectRatio));
        } else if (check_file_exist(coverjpg.toStdString().c_str())) {
            QPixmap img = coverjpg;
            ui_->pushButtonBack->setIcon(img.scaled(270,270,Qt::KeepAspectRatio));
        } else {
            ui_->pushButtonBack->setIcon(QPixmap("://coverlogo.png"));
        }
        ui_->labelCurrentPlaying->setText(ui_->comboBoxAlbum->currentText());
        ui_->pushButtonPlayerStop->hide();
        ui_->pushButtonPlayerPause->hide();
        ui_->pushButtonPlayerPlayList->show();
    }

    // Write current playing album and track to config
    this->configuration_->setMp3Track(playlist->currentIndex());
    this->configuration_->setMp3SubFolder(ui_->comboBoxAlbum->currentText().toStdString());
    this->configuration_->save();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerPlayList_clicked()
{
    player->setPlaylist(this->playlist);
    playlist->setCurrentIndex(this->currentPlaylistIndex);
    player->play();
    ui_->pushButtonBack->setIcon(QPixmap("://coverlogo.png"));
    ui_->mp3selectWidget->hide();
    ui_->PlayerPlayingWidget->show();
    ui_->pushButtonPlayerPlayList->hide();
    ui_->pushButtonList->show();
    ui_->pushButtonBackToPlayer->hide();
    ui_->pushButtonPlayerStop->show();
    ui_->pushButtonPlayerPause->setStyleSheet( "background-color: rgb(233, 185, 110); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(0,0,0);");
    ui_->pushButtonPlayerPause->show();
    int currentalbum = ui_->comboBoxAlbum->currentIndex();
    ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
}

void f1x::openauto::autoapp::ui::MainWindow::on_comboBoxAlbum_currentIndexChanged(const QString &arg1)
{
    this->albumfolder = arg1;
    MainWindow::scanFiles();
    ui_->pushButtonPlayerPause->hide();
    ui_->pushButtonPlayerStop->hide();
    ui_->pushButtonList->hide();
    ui_->pushButtonBackToPlayer->hide();
    ui_->labelCurrentPlaying->setText("");
    ui_->playerPositionTime->setText("");
}

void f1x::openauto::autoapp::ui::MainWindow::setTrigger()
{
    this->mediacontentchanged = true;

    ui_->SysinfoTopLeft->setText("Media changed - Scanning ...");
    ui_->SysinfoTopLeft->show();

    QTimer::singleShot(10000, this, SLOT(scanFolders()));
}

void f1x::openauto::autoapp::ui::MainWindow::setRetryUSBConnect()
{
    ui_->SysinfoTopLeft->setText("Trying USB connect ...");
    ui_->SysinfoTopLeft->show();

    QTimer::singleShot(10000, this, SLOT(resetRetryUSBMessage()));
}

void f1x::openauto::autoapp::ui::MainWindow::resetRetryUSBMessage()
{
    ui_->SysinfoTopLeft->setText("");
    ui_->SysinfoTopLeft->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::scanFolders()
{
    try {
        if (this->mediacontentchanged == true) {
            this->mediacontentchanged = false;
            int cleaner = ui_->comboBoxAlbum->count();
            while (cleaner > -1) {
                ui_->comboBoxAlbum->removeItem(cleaner);
                cleaner--;
            }
            QDir directory(this->musicfolder);
            QStringList folders = directory.entryList(QStringList() << "*", QDir::AllDirs, QDir::Name);
            foreach (QString foldername, folders) {
                if (foldername != "..") {
                    ui_->comboBoxAlbum->addItem(foldername);
                    ui_->labelAlbumCount->setText(QString::number(ui_->comboBoxAlbum->count()));
                }
            }
            this->currentPlaylistIndex = 0;
            ui_->SysinfoTopLeft->hide();
        }
    }
    catch(...) {
        ui_->SysinfoTopLeft->hide();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::scanFiles()
{
    if (this->mediacontentchanged == false) {
        int cleaner = ui_->mp3List->count();
        while (cleaner > -1) {
            ui_->mp3List->takeItem(cleaner);
            cleaner--;
        }
        this->playlist->clear();

        QList<QMediaContent> content;
        QDir directory(this->musicfolder + "/" + this->albumfolder);
        QStringList mp3s = directory.entryList(QStringList() << "*.mp3" << "*.flac" << "*.aac" << "*.ogg" << "*.mp4" << "*.mp4a" << "*.wma" << "*.strm",QDir::Files, QDir::Name);
        foreach (QString filename, mp3s) {
            // add to mediacontent
            if (filename.endsWith(".strm")) {
                QString url=configuration_->readFileContent(this->musicfolder + "/" + this->albumfolder + "/" + filename);
                content.push_back(QMediaContent(QUrl(url)));
                ui_->mp3List->addItem(filename.replace(".strm",""));
            } else {
                // add items to gui
                content.push_back(QMediaContent(QUrl::fromLocalFile(this->musicfolder + "/" + this->albumfolder + "/" + filename)));
                // read metadata using taglib
                try {
                    TagLib::FileRef file((this->musicfolder + "/" + this->albumfolder + "/" + filename).toUtf8(),true);
                    TagLib::String artist_string = file.tag()->artist();
                    TagLib::String title_string = file.tag()->title();
                    TagLib::uint track_string = file.tag()->track();
                    QString artistid3 = QString::fromStdWString(artist_string.toCWString());
                    QString titleid3 = QString::fromStdWString(title_string.toCWString());
                    QString trackid3 = QString::number(track_string);
                    int tracklength = trackid3.length();
                    if (tracklength < 2) {
                        trackid3 = "0" + trackid3;
                    }
                    QString ID3Entry = trackid3 + ": " + artistid3 + " - " + titleid3;
                    ui_->mp3List->addItem(ID3Entry);
                } catch (...) {
                    // old way only adding filename to list
                    ui_->mp3List->addItem(filename);
                }
            }
        }
        // set playlist
        this->playlist->addMedia(content);
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_mp3List_currentRowChanged(int currentRow)
{
    ui_->labelFolderpath->setText(QString::number(currentRow));
    this->currentPlaylistIndex = currentRow;
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerNextBig_clicked()
{
    playlist->next();
    if (playlist->currentIndex() != -1) {
        player->play();
        ui_->pushButtonPlayerStop->show();
        ui_->pushButtonPlayerPause->show();
        ui_->pushButtonPlayerPlayList->hide();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerPrevBig_clicked()
{
    playlist->previous();
    if (playlist->currentIndex() != -1) {
        player->play();
        ui_->pushButtonPlayerStop->show();
        ui_->pushButtonPlayerPause->show();
        ui_->pushButtonPlayerPlayList->hide();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerPrevAlbum_clicked()
{
    int albumcount = ui_->comboBoxAlbum->count();
    int currentalbum = ui_->comboBoxAlbum->currentIndex();
    if (currentalbum >= 1) {
        currentalbum = currentalbum-1;
        ui_->comboBoxAlbum->setCurrentIndex(currentalbum);
        ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
        player->play();
    } else {
        currentalbum = albumcount-1;
        ui_->comboBoxAlbum->setCurrentIndex(currentalbum);
        ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
        player->play();
    }
    if (ui_->mp3selectWidget->isVisible() == false) {
        ui_->pushButtonPlayerBack->show();
        ui_->pushButtonList->show();
        ui_->pushButtonPlayerPause->show();
        ui_->pushButtonPlayerStop->show();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerNextAlbum_clicked()
{
    int albumcount = ui_->comboBoxAlbum->count();
    int currentalbum = ui_->comboBoxAlbum->currentIndex();
    if (currentalbum < albumcount-1) {
        currentalbum = currentalbum + 1;
        ui_->comboBoxAlbum->setCurrentIndex(currentalbum);
        ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
        player->play();
    } else {
        currentalbum = 0;
        ui_->comboBoxAlbum->setCurrentIndex(currentalbum);
        ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
        player->play();
    }
    if (ui_->mp3selectWidget->isVisible() == false) {
        ui_->pushButtonPlayerBack->show();
        ui_->pushButtonList->show();
        ui_->pushButtonPlayerPause->show();
        ui_->pushButtonPlayerStop->show();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonBackToPlayer_clicked()
{
    ui_->PlayerPlayingWidget->show();
    ui_->mp3selectWidget->hide();
    ui_->pushButtonBackToPlayer->hide();
    ui_->pushButtonPlayerPlayList->hide();
    ui_->pushButtonList->show();
    ui_->pushButtonPlayerPause->show();
    ui_->pushButtonPlayerStop->show();
}

void f1x::openauto::autoapp::ui::MainWindow::on_StateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState || state == QMediaPlayer::PausedState) {
        std::remove("/tmp/media_playing");
    } else {
        std::ofstream("/tmp/media_playing");
    }
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

void f1x::openauto::autoapp::ui::MainWindow::KeyPress(QString key)
{
    if (configuration_->playerButtonControl()) {
        if (key == "C") {
            on_pushButtonPlayerPause_clicked();
        }
        if (key == "X") {
            on_pushButtonPlayerPlayList_clicked();
        }
        if (key == "V") {
            on_pushButtonPlayerPrevBig_clicked();
        }
        if (key == "N") {
            on_pushButtonPlayerNextBig_clicked();
        }
        if (key == "B") {
            on_pushButtonPlayerPause_clicked();
        }
        if (key == "A") {
            on_pushButtonPlayerPrevAlbum_clicked();
        }
        if (key == "L") {
            on_pushButtonPlayerNextAlbum_clicked();
        }
        if (key == "H") {
            if (ui_->mediaWidget->isVisible() == false) {
                playerShow();
            } else {
                playerHide();
            }
        }
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

    // check if system is in display off mode (tap2wake)
    if (std::ifstream("/tmp/blankscreen")) {
        if (ui_->centralWidget->isVisible() == true) {
            CloseAllDialogs();
            ui_->centralWidget->hide();
        }
    } else {
        if (ui_->centralWidget->isVisible() == false) {
            ui_->centralWidget->show();
        }
    }

    // check if system is in display off mode (tap2wake/screensaver)
    if (std::ifstream("/tmp/screensaver")) {
        if (ui_->menuWidget->isVisible() == true) {
            ui_->menuWidget->hide();
        }
        if (ui_->oldmenuWidget->isVisible() == true) {
            ui_->oldmenuWidget->hide();
        }
        if (ui_->headerWidget->isVisible() == true) {
            ui_->headerWidget->hide();
            CloseAllDialogs();
        }
        if (ui_->mediaWidget->isVisible() == true) {
            ui_->mediaWidget->hide();
        }
        if (ui_->cameraWidget->isVisible() == true) {
            ui_->cameraWidget->hide();
        }
        if (ui_->VolumeSliderControlPlayer->isVisible() == true) {
            ui_->VolumeSliderControlPlayer->hide();
        }
        if (ui_->VolumeSliderControl->isVisible() == true) {
            ui_->VolumeSliderControl->hide();
        }
        if (ui_->BrightnessSliderControl->isVisible() == true) {
            ui_->BrightnessSliderControl->hide();
        }
        cameraHide();
        if (ui_->clockOnlyWidget->isVisible() == false) {
            ui_->clockOnlyWidget->show();
        }
    } else {
        if (ui_->headerWidget->isVisible() == false) {
            ui_->headerWidget->show();
        }
        if (ui_->VolumeSliderControl->isVisible() == false) {
            if (ui_->mediaWidget->isVisible() == false) {
                ui_->VolumeSliderControl->show();
            }
        }
        if (ui_->clockOnlyWidget->isVisible() == true) {
            ui_->clockOnlyWidget->hide();
            toggleGUI();
            toggleGUI();
            updateBG();
        }
    }

    // check if custom command needs black background
    if (std::ifstream("/tmp/blackscreen")) {
        if (ui_->centralWidget->isVisible() == true) {
            ui_->centralWidget->hide();
            this->setStyleSheet("QMainWindow {background-color: rgb(0,0,0);}");
            this->background_set = false;
        }
    } else {
        if (this->background_set == false) {
            f1x::openauto::autoapp::ui::MainWindow::updateBG();
            this->background_set = true;
        }
    }

    // check if phone is conencted to usb
    if (std::ifstream("/tmp/android_device")) {
        if (ui_->ButtonAndroidAuto->isVisible() == false) {
            ui_->ButtonAndroidAuto->show();
            ui_->pushButtonNoDevice->hide();
        }
        if (ui_->pushButtonAndroidAuto2->isVisible() == false) {
            ui_->pushButtonAndroidAuto2->show();
            ui_->pushButtonNoDevice2->hide();
        }
        try {
            QFile deviceData(QString("/tmp/android_device"));
            deviceData.open(QIODevice::ReadOnly);
            QTextStream data_date(&deviceData);
            QString linedate = data_date.readAll().split("\n")[1];
            deviceData.close();
            ui_->labelAndroidAutoBottom->setText(linedate.simplified().replace("_"," "));
        } catch (...) {
            ui_->labelAndroidAutoBottom->setText("");
        }
    } else {
        if (ui_->ButtonAndroidAuto->isVisible() == true) {
            ui_->pushButtonNoDevice->show();
            ui_->ButtonAndroidAuto->hide();
        }
        if (ui_->pushButtonAndroidAuto2->isVisible() == true) {
            ui_->pushButtonNoDevice2->show();
            ui_->pushButtonAndroidAuto2->hide();
        }
        ui_->labelAndroidAutoBottom->setText("");
    }

    // check if bluetooth pairable
    if (this->bluetoothEnabled) {
        if (std::ifstream("/tmp/bluetooth_pairable")) {
            if (ui_->labelBluetoothPairable->isVisible() == false) {
                ui_->labelBluetoothPairable->show();
            }
            if (ui_->pushButtonBluetooth->isVisible() == true) {
                ui_->pushButtonBluetooth->hide();
            }
        } else {
            if (ui_->labelBluetoothPairable->isVisible() == true) {
                ui_->labelBluetoothPairable->hide();
            }
            if (ui_->pushButtonBluetooth->isVisible() == false) {
                ui_->pushButtonBluetooth->show();
            }
        }
    } else {
        if (ui_->labelBluetoothPairable->isVisible() == true) {
            ui_->labelBluetoothPairable->hide();
        }
        if (ui_->pushButtonBluetooth->isVisible() == true) {
            ui_->pushButtonBluetooth->hide();
        }
    }

    if (std::ifstream("/tmp/config_in_progress") || std::ifstream("/tmp/debug_in_progress") || std::ifstream("/tmp/enable_pairing")) {
        if (ui_->SysinfoTopLeft2->isVisible() == false) {
            if (std::ifstream("/tmp/config_in_progress")) {
                ui_->pushButtonSettings->hide();
                ui_->pushButtonSettings2->hide();
                ui_->pushButtonLock->show();
                ui_->pushButtonLock2->show();
                ui_->SysinfoTopLeft2->setText("Config in progress ...");
                ui_->SysinfoTopLeft2->show();
            }
            if (std::ifstream("/tmp/debug_in_progress")) {
                ui_->pushButtonSettings->hide();
                ui_->pushButtonSettings2->hide();
                ui_->pushButtonDebug->hide();
                ui_->pushButtonDebug2->hide();
                ui_->pushButtonLock->show();
                ui_->pushButtonLock2->show();
                ui_->SysinfoTopLeft2->setText("Creating debug.zip ...");
                ui_->SysinfoTopLeft2->show();
            }
            if (std::ifstream("/tmp/enable_pairing")) {
                ui_->pushButtonDebug->hide();
                ui_->pushButtonDebug2->hide();
                ui_->SysinfoTopLeft2->setText("Pairing enabled for 120 seconds!");
                ui_->SysinfoTopLeft2->show();
            }
        }
    } else {
        if (ui_->SysinfoTopLeft2->isVisible() == true) {
            ui_->SysinfoTopLeft2->setText("");
            ui_->SysinfoTopLeft2->hide();
            ui_->pushButtonSettings->show();
            ui_->pushButtonSettings2->show();
            ui_->pushButtonLock->hide();
            ui_->pushButtonLock2->hide();
            if (this->systemDebugmode) {
                ui_->pushButtonDebug->show();
                ui_->pushButtonDebug2->show();
            }
        }
    }

    // update day/night state
    this->nightModeEnabled = check_file_exist("/tmp/night_mode_enabled");

    if (this->nightModeEnabled) {
        if (!this->DayNightModeState) {
            this->DayNightModeState = true;
            f1x::openauto::autoapp::ui::MainWindow::switchGuiToNight();
        }
    } else {
        if (this->DayNightModeState) {
            this->DayNightModeState = false;
            f1x::openauto::autoapp::ui::MainWindow::switchGuiToDay();
        }
    }

    // camera stuff
    if (this->cameraButtonForce) {

        // check if dashcam is recording
        this->dashCamRecording = check_file_exist("/tmp/dashcam_is_recording");

        if (this->dashCamRecording) {
            if (ui_->dcRecording->isVisible() == false) {
                ui_->dcRecording->show();
            }
        } else {
            if (ui_->dcRecording->isVisible() == true) {
                ui_->dcRecording->hide();
            }
        }

        // show recording state if dashcam is visible
        if (ui_->cameraWidget->isVisible() == true) {
            if (this->dashCamRecording) {
                if (ui_->pushButtonRecord->isVisible() == true) {
                    ui_->pushButtonRecordActive->show();
                    ui_->pushButtonRecord->hide();
                }
            } else {
                if (ui_->pushButtonRecordActive->isVisible() == true) {
                    ui_->pushButtonRecord->show();
                    ui_->pushButtonRecordActive->hide();
                }
            }
        }
    }

    // check if shutdown is external triggered and init clean app exit
    if (std::ifstream("/tmp/external_exit")) {
        f1x::openauto::autoapp::ui::MainWindow::MainWindow::exit();
    }

    this->hotspotActive = check_file_exist("/tmp/hotspot_active");

    // hide wifi if hotspot disabled and force wifi unselected
    if (!this->hotspotActive && !std::ifstream("/tmp/mobile_hotspot_detected")) {
        if ((ui_->AAWIFIWidget->isVisible() == true) || (ui_->AAWIFIWidget2->isVisible() == true)){
            ui_->AAWIFIWidget->hide();
            ui_->AAWIFIWidget2->hide();
            ui_->AAUSBWidget->show();
            ui_->AAUSBWidget2->show();
        }
    } else {
        if ((ui_->AAWIFIWidget->isVisible() == false) || (ui_->AAWIFIWidget2->isVisible() == false)) {
            ui_->AAWIFIWidget->show();
            ui_->AAWIFIWidget2->show();
            ui_->AAUSBWidget->hide();
            ui_->AAUSBWidget2->hide();
        }
    }

    if (std::ifstream("/tmp/temp_recent_list") || std::ifstream("/tmp/mobile_hotspot_detected")) {
        if (ui_->pushButtonWifi->isVisible() == false) {
            ui_->pushButtonWifi->show();
        }
        if (ui_->pushButtonNoWiFiDevice->isVisible() == true) {
            ui_->pushButtonNoWiFiDevice->hide();
        }
        if (ui_->pushButtonWifi2->isVisible() == false) {
            ui_->pushButtonWifi2->show();
        }
        if (ui_->pushButtonNoWiFiDevice2->isVisible() == true) {
            ui_->pushButtonNoWiFiDevice2->hide();
        }
    } else {
        if (ui_->pushButtonWifi->isVisible() == true) {
            ui_->pushButtonWifi->hide();
        }
        if (ui_->pushButtonNoWiFiDevice->isVisible() == false) {
            ui_->pushButtonNoWiFiDevice->show();
        }
        if (ui_->pushButtonWifi2->isVisible() == true) {
            ui_->pushButtonWifi2->hide();
        }
        if (ui_->pushButtonNoWiFiDevice2->isVisible() == false) {
            ui_->pushButtonNoWiFiDevice2->show();
        }
    }

    // handle dummys in classic menu
    int button_count = 0;
    if (ui_->pushButtonCameraShow2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->AAWIFIWidget2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->pushButtonDebug2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->pushButtonVolume2->isVisible() == true) {
        button_count = button_count + 1;
    }

    // Hide auto day/night if needed
    if (this->lightsensor || std::ifstream("/tmp/daynight_gpio")) {
        ui_->pushButtonDay->hide();
        ui_->pushButtonNight->hide();
        ui_->pushButtonDay2->hide();
        ui_->pushButtonNight2->hide();
        ui_->pushButtonBrightness->hide();
        ui_->pushButtonBrightness2->hide();
    }

    // use big clock in classic gui?
    if (this->configuration_->showBigClock()) {
        this->UseBigClock = true;
    } else {
        this->UseBigClock = false;
    }

    // clock viibility by settings
    if (!this->configuration_->showClock()) {
        ui_->Digital_clock->hide();
        ui_->oldmenuDummy->show();
        ui_->bigClock->hide();
        this->NoClock = true;
    } else {
        this->NoClock = false;
        if (this->UseBigClock && ui_->oldmenuWidget->isVisible() == true) {
            ui_->oldmenuDummy->hide();
            ui_->bigClock->show();
            if (oldGUIStyle) {
                ui_->Digital_clock->hide();
            }
        } else {
            ui_->oldmenuDummy->show();
            ui_->Digital_clock->show();
            ui_->bigClock->hide();
        }
    }

    if (!this->configuration_->showNetworkinfo()) {
        if (ui_->networkInfo->isVisible() == true) {
            ui_->networkInfo->hide();
        }
    } else {
        if (ui_->networkInfo->isVisible() == false) {
            if (ui_->mediaWidget->isVisible() == false) {
                ui_->networkInfo->show();
            }
        }
    }

    // hide gui toggle if enabled in settings
    if (this->configuration_->hideMenuToggle()) {
        ui_->pushButtonToggleGUI->hide();
        ui_->pushButtonToggleGUI2->hide();
    } else {
        ui_->pushButtonToggleGUI->show();
        ui_->pushButtonToggleGUI2->show();
    }

    // hide brightness button if eanbled in settings
    if (configuration_->hideBrightnessControl()) {
        if ((ui_->pushButtonBrightness->isVisible() == true) || (ui_->pushButtonBrightness->isVisible() == true) || (ui_->BrightnessSliderControl->isVisible() == true)) {
            ui_->pushButtonBrightness->hide();
            ui_->pushButtonBrightness2->hide();
            ui_->BrightnessSliderControl->hide();
            // also hide volume button if brightness hidden
            ui_->pushButtonVolume->hide();
            ui_->pushButtonVolume2->hide();
            if (ui_->mediaWidget->isVisible() == false) {
                ui_->VolumeSliderControl->show();
            }
        }
    } else {
        if (!this->lightsensor) {
            if ((ui_->pushButtonBrightness->isVisible() == false) || (ui_->pushButtonBrightness->isVisible() == false)) {
                ui_->pushButtonBrightness->show();
                ui_->pushButtonBrightness2->show();
                // also show volume button if brightness visible
                ui_->pushButtonVolume->show();
                ui_->pushButtonVolume2->show();
                ui_->BrightnessSliderControl->hide();
            }
        }
    }

    // read value from tsl2561
    if (std::ifstream("/tmp/tsl2561") && this->configuration_->showLux()) {
        if (ui_->label_left->isVisible() == false) {
            ui_->label_left->show();
            ui_->label_right->show();
        }
        ui_->label_left->setText("Lux: " + configuration_->readFileContent("/tmp/tsl2561"));
    } else {
        if (ui_->label_left->isVisible() == true) {
            ui_->label_left->hide();
            ui_->label_right->hide();
            ui_->label_left->setText("");
            ui_->label_right->setText("");
        }
    }
    MainWindow::updateAlpha();

    // update notify
    this->csmtupdate = check_file_exist("/tmp/csmt_update_available");
    this->udevupdate = check_file_exist("/tmp/udev_update_available");
    this->openautoupdate = check_file_exist("/tmp/openauto_update_available");
    this->systemupdate = check_file_exist("/tmp/system_update_available");

    if (this->csmtupdate || this->udevupdate || this->openautoupdate || this->systemupdate) {
        if (ui_->pushButtonUpdate->isVisible() == false) {
            ui_->pushButtonUpdate->show();
            ui_->label_left->show();
            ui_->label_right->show();
            if (this->devModeEnabled) {
                ui_->devlabel_right->hide();
            } else {
                ui_->label_dummy_right->show();
            }
        }
    } else {
        if (ui_->pushButtonUpdate->isVisible() == true) {
            ui_->pushButtonUpdate->hide();
            ui_->label_left->hide();
            ui_->label_right->hide();
            ui_->label_dummy_right->hide();
            if (this->devModeEnabled) {
                ui_->devlabel_right->show();
            }
        }
    }

    if (std::ifstream("/tmp/btdevice") || std::ifstream("/tmp/media_playing") || std::ifstream("/tmp/dev_mode_enabled") || std::ifstream("/tmp/android_device")) {
        if (ui_->labelLock->isVisible() == false) {
            ui_->labelLock->show();
            ui_->labelLockDummy->show();
        }
    } else {
        if (ui_->labelLock->isVisible() == true) {
            ui_->labelLock->hide();
            ui_->labelLockDummy->hide();
        }
    }
    updateNetworkInfo();
}
