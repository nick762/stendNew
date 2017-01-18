#include "port.h"
#include <qdebug.h>
Port::Port(QObject *parent) :
    QObject(parent)
{
}
Port::~Port()
{
    qDebug("By in Thread!");
    emit finished_Port();
}
void Port :: process_Port()
{
    connect(&thisPort,SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&thisPort, SIGNAL(readyRead()),this,SLOT(ReadInPort()));
}

void Port :: Write_Settings_Port(QString name)
{
    SettingsPort.name = name;
}

void Port :: ConnectPort(void)
{
    if(serial==true){
        thisPort.setPortName(SettingsPort.name);
        if (thisPort.open(QIODevice::ReadWrite)) {
            if (thisPort.setBaudRate(QSerialPort::Baud115200)
                && thisPort.setDataBits(QSerialPort::Data8)
                && thisPort.setParity(QSerialPort::NoParity)
                && thisPort.setStopBits(QSerialPort::OneStop)
                && thisPort.setFlowControl(QSerialPort::NoFlowControl)){
                    if (thisPort.isOpen()){
                        error_((SettingsPort.name+ " Opened!").toLocal8Bit());
                        //WriteToPort("start");
                    }
                } else {
                    thisPort.close();
                    error_(thisPort.errorString().toLocal8Bit()+" Error setup");
                }
        } else {
            thisPort.close();
            error_(thisPort.errorString().toLocal8Bit()+" Error opening");
        }
        serial=false;
    }
}
void Port::handleError(QSerialPort::SerialPortError error)
{
    if ( (thisPort.isOpen()) && (error == QSerialPort::ResourceError)) {
        error_(thisPort.errorString().toLocal8Bit());
        DisconnectPort();
    }
}
void  Port::DisconnectPort()
{
    if(thisPort.isOpen()){
        thisPort.close();
        error_(SettingsPort.name.toLocal8Bit() + " Closed!");
    }
}
void Port :: WriteToPort(QByteArray data)
{
    if(thisPort.isOpen()){
        thisPort.write(data);
        thisPort.write("\n");
        thisPort.flush();
    }else{
        error_(thisPort.errorString().toLocal8Bit()+" Write error");
    }
}


void Port :: ReadInPort()//Чтение данных из порта
{
    QByteArray data;
    while (thisPort.waitForReadyRead(200)){
        data +=thisPort.readAll();
        int read_num = data.indexOf('\r');
        if (read_num > 0){
            data = data.left(read_num + 1);
            qDebug()<<data;
            emit outPort(data);
        }
    }
    data.clear();
}
