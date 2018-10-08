#ifndef PORT_H
#define PORT_H
#include <QObject>
#include <QtSerialPort/QSerialPort>

struct Settings {
    QString name;
};
class Port : public QObject
{
    Q_OBJECT
public:
    explicit Port(QObject *parent = 0);
    ~Port();
    QSerialPort thisPort;
    Settings SettingsPort;
signals:
    void finished_Port();
    void error_(QString err);
    void outPort(QString data);
public slots:
    void  DisconnectPort();
    void ConnectPort(void);
    void Write_Settings_Port(QString name);
    void process_Port();
    void WriteToPort(QByteArray data);
private slots:
    void handleError(QSerialPort::SerialPortError error);
    void ReadInPort();
public:
private:
    bool serial=true;
};
#endif // PORT_H
