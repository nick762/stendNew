#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QList>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "qcustomplot.h"
#include "json.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Charge_clicked();
    void on_Discharge_clicked();
    void serialFind();
    void on_pushButton_clicked();
    void read(QString data);
    void intToYear(QString date);
    void on_cycling_clicked();
    void on_graph_clicked();
    void toFile(QList<QString> value);
    void Print(QString);
    void on_setSerial_clicked();
    QList<QString> wordToByte(QString dat);
    QList<QString> decToBin(QList<QString> dat);
    void status(QString stat);
    void on_setDate_clicked();
    void on_seal_clicked();
    void on_unseal_clicked();
    void on_fullAccess_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *port;
    QTimer *tmr;
    QWidget *widget;
    QCustomPlot *customPlot;
    QCPGraph *graphic, *graphic2;
    QFileDialog *fDialog;
    bool c_checked=false;
    bool d_checked=false;
    bool cycled = false;
    bool ser = true;
    bool dat = true;
    bool serial = true;
    bool hasFile = false;
    double minElem(QVector<double> input);
    double maxElem(QVector<double> input);
signals:
    void hasData(QList<QString> val);
    void savesettings(QString name);
    void writeData(QByteArray data);
    void error_(QString err);
};

#endif // MAINWINDOW_H
