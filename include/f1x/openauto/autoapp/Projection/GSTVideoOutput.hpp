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

#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <boost/circular_buffer.hpp>
#include <f1x/openauto/autoapp/Projection/VideoOutput.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
#include <QGlib/Error>
#include <QGlib/Connect>
#include <QGst/Init>
#include <QGst/Bus>
#include <QGst/Pipeline>
#include <QGst/Parse>
#include <QGst/Message>
#include <QGst/Utils/ApplicationSink>
#include <QGst/Utils/ApplicationSource>
#include <QGst/Ui/VideoWidget>
#include <QGst/ElementFactory>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{



class GSTVideoOutput: public VideoOutput
{
public:
    GSTVideoOutput(configuration::IConfiguration::Pointer configuration, QWidget* videoContainer=nullptr);

    bool open() override;
    bool init() override;
    void write(uint64_t timestamp, const aasdk::common::DataConstBuffer& buffer) override;
    void stop() override;

private:
    bool createComponents();
    bool initClock();
    bool setupTunnels();
    bool enablePortBuffers();
    bool setupDisplayRegion();
    static GstPadProbeReturn convert_probe(GstPad *pad, GstPadProbeInfo *info, void *user_data);
    static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer *ptr);
    QGst::ElementPtr m_videoSink;
    std::unique_ptr<QGst::Ui::VideoWidget> videoWidget_;
    GstElement *vid_pipeline = nullptr;
    GstAppSrc *vid_src = nullptr;
    std::mutex mutex_;
    bool isActive_;
    bool portSettingsChanged_;
    std::function<void(bool)> activeCallback_;
    QWidget* videoContainer_;

};

}
}
}
}
