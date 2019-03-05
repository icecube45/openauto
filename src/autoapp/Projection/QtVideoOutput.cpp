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
#include <f1x/openauto/autoapp/Projection/QtVideoOutput.hpp>
#include <f1x/openauto/Common/Log.hpp>
#include <QFrame>
namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

QtVideoOutput::QtVideoOutput(configuration::IConfiguration::Pointer configuration)
    : VideoOutput(std::move(configuration))
{
    mainWindow = QApplication::activeWindow(); //Honestly this might break things but it hasn't yet in testing - is there a better way?
    this->moveToThread(QApplication::instance()->thread());
    connect(this, &QtVideoOutput::startPlayback,  this, &QtVideoOutput::onStartPlayback, Qt::QueuedConnection);
    connect(this, &QtVideoOutput::stopPlayback, this, &QtVideoOutput::onStopPlayback, Qt::QueuedConnection);

    QMetaObject::invokeMethod(this, "createVideoOutput", Qt::BlockingQueuedConnection);
}

void QtVideoOutput::createVideoOutput()
{
    OPENAUTO_LOG(debug) << "[QtVideoOutput] create.";
    // videoWidget_ = std::make_unique<QVideoWidget>();
    mediaPlayer_ = std::make_unique<QMediaPlayer>(nullptr, QMediaPlayer::StreamPlayback);
}


bool QtVideoOutput::open()
{
    return videoBuffer_.open(QIODevice::ReadWrite);
}

bool QtVideoOutput::init()
{
    emit startPlayback();
    return true;
}

void QtVideoOutput::stop()
{
    emit stopPlayback();
}

void QtVideoOutput::write(uint64_t, const aasdk::common::DataConstBuffer& buffer)
{
    videoBuffer_.write(reinterpret_cast<const char*>(buffer.cdata), buffer.size);
}

void QtVideoOutput::onStartPlayback()
{
    QVideoWidget *aaVideo = mainWindow->findChild<QVideoWidget*>("aaVideoWidget");
    aaVideo->setBrightness(VideoOutput::getVideoBrightness());
    aaVideo->setContrast(VideoOutput::getVideoContrast());
    aaVideo->setAspectRatioMode(Qt::IgnoreAspectRatio);
    aaVideo->setFocus();
    aaVideo->setWindowFlags(Qt::WindowStaysOnTopHint);
    aaVideo->raise();
    aaVideo->show();
    mainWindow->findChild<QFrame*>("idleFrame")->hide();
    mainWindow->findChild<QFrame*>("androidAutoFrame")->show();

    // mainWindow->setFullScreen(true);

    // videoWidget_->setAspectRatioMode(Qt::IgnoreAspectRatio);
    // videoWidget_->setFocus();
    // videoWidget_->setWindowFlags(Qt::WindowStaysOnTopHint);
    // videoWidget_->setFullScreen(false);
    // videoWidget_->hide();
    // videoWidget_->show();

    mediaPlayer_->setVideoOutput(aaVideo);
    mediaPlayer_->setMedia(QMediaContent(), &videoBuffer_);
    mediaPlayer_->play();
}

void QtVideoOutput::onStopPlayback()
{
    // videoWidget_->hide();
    mediaPlayer_->stop();
    mainWindow->findChild<QFrame*>("idleFrame")->show();
    mainWindow->findChild<QFrame*>("androidAutoFrame")->hide();
}

}
}
}
}
