#include "btservice/btservice.hpp"

namespace openauto
{
namespace btservice
{

btservice::btservice(openauto::configuration::IConfiguration::Pointer config)
    : androidBluetoothService_(cServicePortNumber)
    , androidBluetoothServer_(config)
{
    QBluetoothAddress address;
    auto adapters = QBluetoothLocalDevice::allDevices();
    if(adapters.size() > 0)
    {
        address = adapters.at(0).address();
    }
    else
    {
        OPENAUTO_LOG(error) << "[btservice] No adapter found.";
    }

    if(!androidBluetoothServer_.start(address, cServicePortNumber))
    {
        OPENAUTO_LOG(error) << "[btservice] Server start failed.";
        return;
    }

    OPENAUTO_LOG(info) << "[btservice] Listening for connections, address: " << address.toString().toStdString()
                       << ", port: " << cServicePortNumber;

    if(!androidBluetoothService_.registerService(address))
    {
        OPENAUTO_LOG(error) << "[btservice] Service registration failed.";
    }
    else
    {
        OPENAUTO_LOG(info) << "[btservice] Service registered, port: " << cServicePortNumber;
    }
    connectToBluetooth(QBluetoothAddress(QString::fromStdString(config->getLastBluetoothPair())), address);
}

void btservice::connectToBluetooth(QBluetoothAddress addr, QBluetoothAddress controller)
{
    // The raspberry pi has a really tough time using bluetoothctl (or really anything) to connect to an Android phone
    // even though phone connecting to the pi is fine.
    // I found a workaround where you can make the pi attempt an rfcomm connection to the phone, and it connects immediately
    // This might require setting u+s on rfcomm though
    // Other computers with more sane bluetooth shouldn't have an issue using bluetoothctl
    
    #ifdef RPI
    QString program = QString::fromStdString("sudo stdbuf -oL rfcomm connect hci0 ")+addr.toString()+QString::fromStdString(" 2");
    btConnectProcess = new QProcess();
    OPENAUTO_LOG(info)<<"[btservice] Attempting to connect to last bluetooth device, "<<addr.toString().toStdString()<<" with `"<<program.toStdString();
    btConnectProcess->start(program, QProcess::Unbuffered | QProcess::ReadWrite);
    #else
    btConnectProcess = new QProcess();
    btConnectProcess->setProcessChannelMode(QProcess::SeparateChannels);
    OPENAUTO_LOG(info)<<"[btservice] Attempting to connect to last bluetooth device, "<<addr.toString().toStdString()<<" with bluetoothctl";
    btConnectProcess->start("bluetoothctl");
    btConnectProcess->waitForStarted();
    btConnectProcess->write(QString("select %1\n").arg(controller.toString()).toUtf8());
    btConnectProcess->write(QString("connect %1\n").arg(addr.toString()).toUtf8());
    btConnectProcess->closeWriteChannel();
    btConnectProcess->waitForFinished();
    #endif
}


}
}
