#include "mainwindow.h"
#include "ui_material.h"
//#include "ui_mainwindow.h"
#include "port.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile styleF(":css/css/dark.css");

    if(!styleF.open(QFile::ReadOnly)){
        qDebug()<<"can't open style";
    }else{
        QString qssStr = styleF.readAll();
        qApp->setStyleSheet(qssStr);
    }
    ui->dateEdit->setDate(QDate::currentDate());
    serialFind();
    QThread *thread_New = new QThread;
    Port *PortNew = new Port();
    PortNew->moveToThread(thread_New);
    PortNew->thisPort.moveToThread(thread_New);
    connect(PortNew, SIGNAL(error_(QString)), this, SLOT(Print(QString)));
    connect(thread_New, SIGNAL(started()), PortNew, SLOT(process_Port()));
    connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(quit()));
    connect(thread_New, SIGNAL(finished()), PortNew, SLOT(deleteLater()));
    connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(deleteLater()));
    connect(this,SIGNAL(savesettings(QString)),PortNew,SLOT(Write_Settings_Port(QString)));
    connect(ui->pushButton, SIGNAL(clicked()),PortNew,SLOT(ConnectPort()));
    //connect(ui->BtnDisconect, SIGNAL(clicked()),PortNew,SLOT(DisconnectPort()));
    connect(PortNew, SIGNAL(outPort(QString)), this, SLOT(read(QString)));
    connect(this,SIGNAL(writeData(QByteArray)),PortNew,SLOT(WriteToPort(QByteArray)));
    thread_New->start();
}

MainWindow::~MainWindow(){
    port->close();
    delete ui;
}

void MainWindow::serialFind(){
    QSerialPortInfo info;
    foreach (info ,QSerialPortInfo::availablePorts()) {
        ui->chooseSerial->addItem(info.portName());
        qDebug() << info.portName();
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(serial==true){
        savesettings(ui->chooseSerial->currentText());
        ui->Discharge->setEnabled(true);
        ui->Charge->setEnabled(true);
        ui->cycling->setEnabled(true);
        ui->graph->setEnabled(true);
        ui->seal->setEnabled(true);
        ui->unseal->setEnabled(true);
        ui->fullAccess->setEnabled(true);
        ui->setSerial->setEnabled(true);
        ui->setDate->setEnabled(true);
        ui->graph->setEnabled(true);
        serial=false;
    }//else{
       // writeData("start");
    //}
}

void MainWindow::read(QString data)
{
    QList<QString> dates;
    for(int i=0;i<15;i++){
        QString vl = data.split(",")[i];
        dates.append(vl);
    }
    if(!dates.isEmpty()){
        emit hasData(dates);
        dates[14].truncate(1);
        if((dates[0]!=dates[1])&&(dates[0]!=dates[3])&&(dates[0]!=dates[4])&&(dates[0]!=dates[5])&&(dates[0]!=dates[6])&&(dates[0]!=dates[7])){
            ui->Vol->display(dates[0]);
        }else{
            qDebug() << "Voltage: " << dates[0] <<endl;
        }
        if((dates[1]!=dates[0])&&(dates[1]!=dates[3])&&(dates[1]!=dates[4])&&(dates[1]!=dates[5])&&(dates[1]!=dates[6])&&(dates[1]!=dates[7])){
            ui->Cur->display(dates[1]);
        }else{
            qDebug() << "Current: " << dates[1] <<endl;
        }
        ui->Temp->display(dates[2]);
        ui->Cap->display(dates[3]);
        ui->Cell1_V->display(dates[4]);
        ui->Cell2_V->display(dates[5]);
        ui->Cell3_V->display(dates[6]);
        ui->Cell4_V->display(dates[7]);

        if(ser == true){
            //qDebug()<<"serial true";
            ui->serial->setText(dates[8]);
            ser=false;
        }

        if((dates[9]=="1")||(dates[9]=="2")){
            //qDebug()<<"blocked";
            ui->FA->setStyleSheet("QLabel {background: #FF5722}");
            ui->SS->setStyleSheet("QLabel {background: #FF5722}");
        }else{
            status(decToBin(wordToByte(dates[9]))[1]);
        }

        if(dat == true){
            //qDebug()<<"date true";
            intToYear(dates[10]);
            dat = false;
        }

        if((dates[11]=="1")&&(dates[12]=="0")){
            ui->Status->setStyleSheet("QLabel {background-color : #00E676;}");
            ui->Status->setText("Идет заряд");
        }else if((dates[11]=="0")&&(dates[12]=="1")){
            ui->Status->setStyleSheet("QLabel {background-color : #FF5722;}");
            ui->Status->setText("Идет разряд");
        }else if ((dates[11]=="0")&&(dates[12]=="0")&&(dates[13]=="0")&&(dates[14]=="0")){
            ui->Status->setStyleSheet("QLabel {background-color : #FF9100; color: #212121}");
            ui->Status->setText("Состояние");
        }

        if((dates[13]=="1")&&(dates[14]=="0")){
            ui->Status->setText("Идет заряд");
            ui->Status->setStyleSheet("QLabel {background-color : #00E676;}");
        }else if((dates[13]=="0")&&(dates[14]=="1")){
            ui->Status->setText("Идет разряд");
            ui->Status->setStyleSheet("QLabel {background-color : #FF5722;}");
        }
    }
   dates.clear();
}

void MainWindow::on_cycling_clicked()
{
    if(cycled==false){
        writeData("y");
        cycled = true;
        ui->cycling->setText("Остановить циклирование");
    }else{
        writeData("y");
        cycled=false;
        ui->cycling->setText("Начать циклирование");
    }
}

void MainWindow::on_graph_clicked()
{
    QJsonDocument jDoc;
    QJsonObject jOb;
    QVector<double> vol,time,current,temperature;
    QString in;
    //QString str = QFileDialog::getOpenFileName(0, "Open Dialog", "C:/Users/zhdanovnv/Documents/stend/", "*.json");
    QString str = QFileDialog::getOpenFileName(0, "Open Dialog", QApplication::applicationDirPath()+"/log/", "*.json");
    if(!str.isNull()){
    QFile file(str);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
        while(!file.atEnd()){
            in = file.readLine();
            jDoc = QJsonDocument::fromJson(in.toUtf8());
            jOb = jDoc.object();
            vol.append(jOb.value(QString("Voltage")).toString().toDouble());
            time.append(jOb.value(QString("Time")).toDouble());
            current.append(jOb.value(QString("Current")).toString().toDouble());
            temperature.append(jOb.value(QString("Temperature")).toString().toDouble());
        }
    }
    file.close();

    widget = new QWidget;
    widget->resize(1000,350);

    customPlot = new QCustomPlot(widget);
    customPlot->resize(1000,350);
    customPlot->move(0,0);

    graphic = new QCPGraph(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(graphic);
    graphic->setPen(QPen(QColor(Qt::red)));;

    customPlot->setInteraction(QCP::iRangeZoom,true);
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    customPlot->axisRect()->setupFullAxesBox();

    customPlot->graph(0)->setData(time,vol);
    customPlot->graph(0)->setAntialiasedFill(false);

    customPlot->yAxis->setAutoTickStep(true);
    customPlot->yAxis->setAutoTicks(true);
    customPlot->xAxis->setLabel("Time");
    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("hh:mm:ss.zzz");
    customPlot->xAxis->setRange(minElem(time),maxElem(time));

    customPlot->yAxis->setLabel("Voltage");
    customPlot->yAxis->setRange(minElem(vol),maxElem(vol)+200);

    customPlot->rescaleAxes();
    customPlot->replot();
    widget->show();
}
}

void MainWindow::on_Charge_clicked()
{
    if(c_checked==true){
        ui->Charge->setChecked(false);
        ui->Discharge->setChecked(true);
        c_checked=false;
        d_checked=false;
    }else{
        ui->Discharge->setChecked(false);
        c_checked=true;
        d_checked=false;
    }
        writeData("c");
}

void MainWindow::on_Discharge_clicked()
{
    if(d_checked==true){
        ui->Discharge->setChecked(false);
        ui->Charge->setChecked(false);
        d_checked=false;
        c_checked=false;
    }else{
        ui->Charge->setChecked(false);
        d_checked=true;
        c_checked=false;
    }
        writeData("d");
}

void MainWindow::on_setSerial_clicked()
{
    QString res = "s"+ui->serial->text();
    writeData(res.toLatin1());
    QFile fileName(QApplication::applicationDirPath()+"/log/"+ui->serial->text()+".json");
    fileName.open(QIODevice::WriteOnly);
    fileName.close();
    qDebug()<<res;
    ser=true;
}

void MainWindow::on_setDate_clicked()
{
    uint16_t year,month,day;
    uint16_t cDate;
    year = ui->dateEdit->date().year();
    month = ui->dateEdit->date().month();
    day = ui->dateEdit->date().day();
    year = (year-1980)*512;
    month = month*32;
    cDate = year+month+day;
    QString res = "e"+QString::number(cDate);
    writeData(res.toLatin1());
    //qDebug()<<res;
    dat = true;
    connect(this, &MainWindow::hasData,this, &MainWindow::toFile);
}

void MainWindow::intToYear(QString date){
    uint16_t year,month,day,fdat;
    QDate aDate;
    fdat = date.toInt();
    year = fdat >> 9;
    year = ((year << 9)/512)+1980;
    month = fdat << 7;
    month = month >> 12;
    month =  (month << 5)/32;
    day = fdat << 11;
    day=day >> 11;
    aDate.setDate(year,month,day);
    qDebug() << year << "/" << month << "/" << day << endl << aDate;
    ui->dateEdit->setDate(aDate);
    //dat = true;
}

/*void MainWindow::createFile(QList<QString> value){
    if(hasFile==true){
        QFile file(QApplication::applicationDirPath()+"/"+value[8]+".json");
        file.open(QIODevice::WriteOnly);
        file.close;
        hasFile==false;
    }
}*/

void MainWindow::toFile(QList<QString> value)
{
    QtJson::JsonObject setData;
    QByteArray status;
    if((value[0]!=value[1])&&(value[0]!=value[3])&&(value[0]!=value[4])&&(value[0]!=value[5])&&(value[0]!=value[6])&&(value[0]!=value[7])&&(value[0]!="65535")&&(value[0]!="0")){
        setData["Voltage"] = value[0];
    }
    setData["Current"] = value[1];
    setData["Temperature"] = value[2];
    setData["Capacity"] = value[3];
    setData["Time"] = QDateTime::currentDateTime().toTime_t();
    setData["S/N"] = value[8];
    status = QtJson::serialize(setData);
    QFile file(QApplication::applicationDirPath()+"/log/"+value[8]+".json");
    if(file.exists()){
        //if(file.open(QIODevice::WriteOnly|QIODevice::Text)){
        if(file.open(QIODevice::Append|QIODevice::Text)){
            QTextStream out(&file);
            out<<status<<"\r\n";
            file.close();
        }
    }/*else{
        if(file.open(QIODevice::Append|QIODevice::Text)){
            QTextStream out(&file);
            out<<status<<"\r\n";
            file.close();
        }
    }*/
}

double MainWindow::minElem(QVector<double> input)
{
    int m=0;
    int i=1;
    while(i<input.length()){
        if(input[i]<input[m]){
            m=i;
        }
        i+=1;
    }
    return input[m];
}

double MainWindow::maxElem(QVector<double> input)
{
    int m=0;
    int i=1;
    while(i<input.length()){
        if(input[i]>input[m]){
            m=i;
        }
        i+=1;
    }
    return input[m];
}

void MainWindow::Print(QString  str)
{
    qDebug()<<str<<endl;
}

QList<QString> MainWindow::wordToByte(QString dat){
    bool ok;
    QList<QString> mlsb;
    uint16_t iVal = dat.toInt(&ok,10);
    mlsb.append(QString::number(iVal&255)); //младший байт
    mlsb.append(QString::number(iVal>>8)); // старший байт
//    qDebug()<<mlsb;
    return mlsb;
}

QList<QString> MainWindow::decToBin(QList<QString> dat){
    QList<QString> binDat;
    bool ok;
    uint8_t msbBin,lsbBin;
    lsbBin = dat[0].toInt(&ok,10);
    msbBin = dat[1].toInt(&ok,10);
    binDat.append(QString::number(lsbBin,2));
    binDat.append(QString::number(msbBin,2));
//    qDebug()<<binDat;
    return binDat;
}

void MainWindow::status(QString stat){
    if(stat[1]=='0'){
        ui->FA->setStyleSheet("QLabel {background: #00E676}");
    }else{
        ui->FA->setStyleSheet("QLabel {background: #FF5722}");
    }
    if(stat[2]=='0'){
        ui->SS->setStyleSheet("QLabel {background: #00E676}");
    }else{
        ui->SS->setStyleSheet("QLabel {background: #FF5722}");
    }
}

void MainWindow::on_seal_clicked()
{
    writeData("l");
}

void MainWindow::on_unseal_clicked()
{
    writeData("u");
}

void MainWindow::on_fullAccess_clicked()
{
    writeData("f");
}
