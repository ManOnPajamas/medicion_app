#include "mydialog.h"
#include "ui_mydialog.h"
#include <QSerialPort>
#include <QDebug>
#include <QTimer>
#include <QtEndian>
#include <QVector>
#include <QElapsedTimer>
#include <QDateTime>
#include <cmath>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
QSerialPort *serial;
QTimer dataTimer;
QVector<double> vector;
QVector<double> vector_rt;
QVector<double> vec_tiempo;
QVector<double> vec_n;
QVector<double> s_t;
QVector<double> s_n;
QElapsedTimer timer1;



bool start = false;
int en = 0;
int en_rst=0;
int en_tech=0;
qint64 cnt = 0;
double curr_t = 0;
int temperature;
static double lastPointKey = 0;
QString date = QDate::currentDate().currentDate().toString();
double ave1=0;
double ave2=0;
double t_f;
int cnt_fuente = 0;
int en_fuente=0;
double t_s;
double t_b;

QVector<int> getPlaceB(QVector <double> &vec,int n);
QVector<int> getPlaceInt(QVector <int> &vec,int n);



void MyDialog::graph_init()
{
    ui->customPlot->setBackground(QBrush(QColor(46,47,48)));
    ui->customPlot->xAxis->setTickLabelColor(Qt::white);
    ui->customPlot->xAxis->setBasePen(QPen(Qt::white));
    ui->customPlot->xAxis->setLabelColor(Qt::white);
    ui->customPlot->xAxis->setTickPen(QPen(Qt::white));
    ui->customPlot->xAxis->setSubTickPen(QPen(Qt::white));

    ui->customPlot->yAxis->setTickLabelColor(Qt::white);
    ui->customPlot->yAxis->setBasePen(QPen(Qt::white));
    ui->customPlot->yAxis->setLabelColor(Qt::white);
    ui->customPlot->yAxis->setTickPen(QPen(Qt::white));
    ui->customPlot->yAxis->setSubTickPen(QPen(Qt::white));
    ui->lineEdit->setVisible(false);
    ui->label->setVisible(false);
    ui->lineEdit_2->setVisible(false);

    ui->startbtn->setVisible(false);
    ui->stopbtn->setVisible(false);
    ui->clearbtn->setVisible(false);
    ui->luzbtn->setVisible(false);
    ui->savebtn->setVisible(false);
    ui->customPlot->setVisible(false);
    ui->spdbtn->setVisible(true);
    ui->pdlcbtn->setVisible(true);

}

void MyDialog::fuente()
{
    double tmp = timer1.elapsed() / 1000;
    if(tmp - t_f>1)
        en_fuente = 0;

    QVector<double> tmp1(&vector_rt[vector_rt.count()-200],&vector_rt[vector_rt.count()-1]);
    QVector<double> tmp2(&vector_rt[vector_rt.count()-300],&vector_rt[vector_rt.count()-201]);
    double sum = std::accumulate(tmp1.begin(), tmp1.end(), 0.0);
    double mean = sum / tmp1.size();

    std::vector<double> diff(tmp1.size());
    std::transform(tmp1.begin(), tmp1.end(), diff.begin(),
                   std::bind2nd(std::minus<double>(), mean));
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / tmp1.size());
    double rms = (1/tmp1.size()) * sq_sum;

    double sum2 = std::accumulate(tmp2.begin(), tmp2.end(), 0.0);
    double mean2 = sum2 / tmp2.size();

    std::vector<double> diff2(tmp2.size());
    std::transform(tmp2.begin(), tmp2.end(), diff2.begin(),
                   std::bind2nd(std::minus<double>(), mean2));
    double sq_sum2 = std::inner_product(diff2.begin(), diff2.end(), diff2.begin(), 0.0);
//    double stdev2 = std::sqrt(sq_sum2 / tmp2.size());
    double rms2 = (1/tmp2.size()) * sq_sum2;
    bool sw_fuente = false;
    if(abs(rms2-rms)<=1)
        sw_fuente = true;


    switch(en_tech){
    case 5:
        if(sw_fuente && serial->isOpen() && en_fuente ==0){
            cnt_fuente++;
            if(cnt_fuente >= 3){
                on_stopbtn_clicked();
                break;
            }
            serial->write("A");
            qDebug()<<(tmp - t_f);
            qDebug()<<"Linea 43"<<Qt::endl;
        }
            break;
    case 20:
        if(stdev<0.6 && serial->isOpen() && en_fuente ==0){
            cnt_fuente++;
            if(cnt_fuente >= 3){
                on_stopbtn_clicked();
                break;
            }
            serial->write("A");
            qDebug()<<(tmp - t_f);
            qDebug()<<"Linea 43"<<Qt::endl;
         }
            break;
}




        en_fuente = 1;



    tmp1.clear();
    tmp2.clear();

}

MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyDialog)
{
    ui->setupUi(this);
    QWidget::setWindowTitle("Sistema de medición de tiempo AGP");
    QWidget::setWindowIcon(QIcon("C:/Users/tavo_/Documents/untitled/images/logo2_2.png"));
    serial_init();
    serial->write("T");//To send temperature value
         //to initialize the glass in off state
    ui->label_2->setText(date);
    graph_init();

}

MyDialog::~MyDialog()
{

    serial->close();
    vector.clear();
    vec_tiempo.clear();
    delete ui;
}

void MyDialog::serialSent(const QByteArray &data)
{
    serial->write(data);
}

void MyDialog::serialRecieved()
{
    double key1 = timer1.elapsed() /1000.0; // time elapsed since start of demo, in seconds

    QByteArray ba = serial->readAll();
    vector_rt.append((uint8_t) ba.at(0));
    vec_n.append(key1);
    cnt++;
    for(int i=0;i<ba.count();i++)
    {
        vector.append((uint8_t) ba.at(i));
    }

//    ui->label->setNum((uint8_t) ba.at(0));
//    qDebug()<<fmod(key1-fmod(key1,1),2);
    curr_t = key1;
    return;
}

void MyDialog::makePlot()
{
    ui->customPlot->addGraph(); // blue line

    ui->customPlot->graph(0)->setPen(QPen(QColor(Qt::white)));
//    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
//    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
//    ui->customPlot->setBackground(QBrush(QColor(179, 218, 255)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->xAxis->setLabel("tiempo (s)");
    switch (en_tech){
    case 5:
        ui->customPlot->yAxis->setRange(0, 210);
        break;
    case 20:
        ui->customPlot->yAxis->setRange(0, 130);
        break;
    }


    ui->customPlot->yAxis->setLabel("Luz transmitida (V)"); //TODO: change to volts

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));


    // setup a timer that repeatedly calls MyDialog::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible



}

void MyDialog::realtimeDataSlot()
{

    double key = timer1.elapsed()/1000.0; // time elapsed since start of demo, in seconds


    if(start){   //Btn "Start" controls when to start mesuarement
    if (key-lastPointKey > 0.002) // at most add point every 2 ms
    {
      // add data to lines:
      ui->customPlot->graph(0)->setData(vec_n, vector_rt);

//      ui->customPlot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
      // rescale value (vertical) axis to fit the current data:
      //ui->customPlot->graph(0)->rescaleValueAxis();
      //ui->customPlot->graph(1)->rescaleValueAxis(true);
      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):

        ui->customPlot->xAxis->setRange(key, 16, Qt::AlignRight);
        ui->customPlot->replot();
    }

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
      lastFpsKey = key;
      frameCount = 0;
    }

    if(fmod(key-fmod(key,1),2)==1 && vector_rt.count()>300 && start)
    {
        fuente();
        t_f = key;
    }
}
void MyDialog::serial_init()
{
    if(serial!=nullptr)
        serial->close();

    serial = new QSerialPort(this);
    serial->setPortName("com3");
    serial->setBaudRate(QSerialPort::Baud921600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if(serial->open(QIODevice::ReadWrite))
        connect(serial, SIGNAL(readyRead()),this,SLOT(serialRecieved()));
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("=^..^=");
        msgBox.setWindowIcon((QIcon("C:/Users/tavo_/Documents/untitled/images/messagebox_critical.png")));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("No se encontró el dispositivo");
        msgBox.exec();
    }






}

void MyDialog::on_startbtn_clicked()
{


    lastPointKey = 0;
    serial_init();
    start = true;
    en_rst = 0;
    if(en == 0){
        timer1.start();
        temperature = (uint8_t)vector_rt.at(vector_rt.size()-1);
        en=1;
    }
    qDebug()<<vector_rt;
    if(vector_rt.size()>3)
        ui->label->setNum(temperature);

    vector.clear();
    vector_rt.clear();
    vec_n.clear();

    serial->write("O");
    timer1.restart();
    ui->lineEdit->setVisible(true);
    ui->label->setVisible(true);
    ui->lineEdit_2->setVisible(true);

    MyDialog::makePlot();


}



void MyDialog::on_stopbtn_clicked()
{
    start = false;
    cnt_fuente = 0;

    qDebug()<<vector.count();
    qDebug()<<vec_n.count();
    qDebug()<<vec_n.at(vec_n.count()-1);
    qDebug()<<curr_t;
    en = 0;
    ui->luzbtn->setVisible(true);


    serial->close();
}

void MyDialog::on_clearbtn_clicked()
{
//    ui->customPlot->clearItems();
    qDebug()<<"238";
    start = false;
    qDebug()<<"240";
    en_rst = 1;
    qDebug()<<"242";
    vector.clear();

    vector.resize(0);
}

void MyDialog::on_luzbtn_clicked()      //Analizar
{

    double *p = (double *) malloc(1L<<24);
    assert(p);
    double t_muestreo = curr_t / vector.count();
    qDebug()<<t_muestreo;

    *p=0;
    for(int i=0;i<vector.size();i++)
        *(p+(i+1)) = *(p+i) + t_muestreo;


    qDebug()<<vector.size();

    QVector<double> tmp_n(vector.size());
    ::memcpy(tmp_n.data(), p, vector.size() * sizeof(double));
    QVector<double> f_n(vector.size()-50);
    std::copy(vector.begin() + 50, vector.end(), f_n.begin());
    QVector<double> f_t(vector.size()-50);
    std::copy(tmp_n.begin() + 50, tmp_n.end(), f_t.begin());

    qDebug() << qSetRealNumberPrecision( 10 )<<*(p+(vector.size()-2));
    qDebug()<<*(p+(vector.size()-1));
    qDebug()<<*(p+(vector.size()));
    qDebug()<<*(p+(vector.size()-vector.size()+2));
    qDebug()<<*(p+(vector.size()-vector.size()+3));
    qDebug()<<tmp_n[10];

    int min = *std::min_element(f_n.constBegin(), f_n.constEnd());
    int max = *std::max_element(f_n.constBegin(), f_n.constEnd());
    int amp = max - min;
    QVector<double> ones(vector.size()-50);
    QVector<double> ones2(vector.size()-50);
    int n_100 = amp+min;
    int n_90 =  (amp * 0.9)+min;
    int n_20 = (amp * 0.2)+min;
    int n_10 = (amp * 0.1)+min;
    std::fill(ones.begin(), ones.end(), n_90);
    std::fill(ones2.begin(), ones2.end(), n_10);



    QVector<int> vals = getPlaceB(f_n,n_90);
    QVector<int> vals2 = getPlaceB(f_n,n_10);
    QVector<int> vals_20 = getPlaceB(f_n,n_20);
    QVector<int> vals_100 = getPlaceB(f_n,n_100);
//    QVector<int> tmp_idx(abs(vals2.size()-vals.size()));
//    std::copy(vals2.begin() + vals.at(vals.size()-1), vals2.end(), tmp_idx.begin());
//    QVector<int> tmp_tb = getPlaceInt(tmp_idx,n_10);
    t_s =  abs(f_t.at(vals2.at(0)) - f_t.at(vals.at(0)));
//    double t_b =  abs(f_t.at(vals.at(vals.size()-1)) - f_t.at(tmp_tb.at(0)));
    t_b =  abs(f_t.at(vals_100.at(vals_100.size()-1)) - f_t.at(vals_20.at(vals_20.size()-1)));


//    QPen linepen(QColor(255,0,0));      //Setting line color and width
//    linepen.setWidth(5);

    QCPScatterStyle myScatter;
      myScatter.setShape(QCPScatterStyle::ssDisc);
      myScatter.setPen(QPen(Qt::red));
      myScatter.setBrush(Qt::white);
      myScatter.setSize(20);


    ui->customPlot->graph(0)->setData(f_t,f_n);
    QVector<double> pt_n;
    pt_n.push_back(n_10);
    pt_n.push_back(n_20);
    pt_n.push_back(n_90);
    pt_n.push_back(n_100);
    QVector<double> pt_t;
    pt_t.push_back(f_t.at(vals2.at(0)));
    pt_t.push_back(f_t.at(vals_20.at(vals_20.size()-1)));
    pt_t.push_back(f_t.at(vals.at(0)));
    pt_t.push_back(f_t.at(vals_100.at(vals_100.size()-1)));

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setData(pt_t,pt_n);
    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
//    ui->customPlot->graph(1)->setPen(linepen);
    ui->customPlot->graph(1)->setScatterStyle(myScatter);

    QCPItemText *textLabel = new QCPItemText(ui->customPlot);
    textLabel->setColor(Qt::red);
    textLabel->setBrush(Qt::black);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.5, 0.5); // place position at center/top of axis rect
    textLabel->setText("El tiempo de subida del 10% - 90% es: "+QString::number(t_s)+"s"+"\n"+"El tiempo de bajada del 100% - 20% es: "+QString::number(t_b)+"s");
    textLabel->setFont(QFont(font().family(), 10)); // make font a bit larger

    textLabel->setPen(QPen(Qt::red)); // show black border around text




    ui->customPlot->xAxis->setRange(0,curr_t);
    ui->customPlot->yAxis->setRange(min-(amp *0.1),max+(amp *0.1));

    ui->customPlot->replot();

//    qDebug()<<getPlaceS(f_n,n_90);
//    qDebug()<<f_t.at(getPlaceS(f_n,n_90));
    qDebug()<<n_90;
    qDebug()<<n_10;
    qDebug()<<t_b;
    qDebug()<<vals.size();
    qDebug()<<vals2.size();
    qDebug()<<vals.at(vals.size()-vals.size());
    qDebug()<<vals2.at(vals2.size()-vals2.size());
    qDebug()<<"Valor de vector en mayor"<<f_n.at(vals.at(0));
    qDebug()<<"Valor de vector en menor pos"<<f_n.at(vals2.at(0));
    qDebug()<<f_t.at(vals.at(0));
    qDebug()<<f_t.at(vals2.at(0));
//    qDebug()<<f_n;
//    qDebug()<<t_b;
    s_n=f_n;
    s_t=f_t;
//    ui->customPlot->graph(1)->data()->clear();
//    ui->customPlot->graph(2)->data()->clear();
    free(p);
    tmp_n.clear();
    f_n.clear();
    f_t.clear();
    vals.clear();
    vals2.clear();
    ones.clear();
    ones2.clear();

    ui->savebtn->setVisible(true);


}


QVector<int> getPlaceB(QVector <double> &vec,int n)
{
    QVector<int> res;

    for(int i=0;i<vec.size()-1;i++)
    {
        if (vec.at(i) == n+1 ||vec.at(i)==n ||vec.at(i)==n-1 ||vec.at(i)==n-2 ||vec.at(i)==n+2)
            res.push_back(i);

    }
    return res;
}

QVector<int> getPlaceInt(QVector <int> &vec,int n)
{
    QVector<int> ret;

    for(int i=0;i<vec.size()-1;i++)
    {
        if (vec.at(i) == n+1 ||vec.at(i)==n ||vec.at(i)==n-1 ||vec.at(i)==n-2 ||vec.at(i)==n+2)
            ret.push_back(i);

    }
    return ret;
}

void MyDialog::on_spdbtn_clicked()
{
//    serial_init();
    serial->write("S");
    en_tech = 20;
    ui->label_3->setText("SPD");
    ui->spdbtn->setVisible(false);
    ui->pdlcbtn->setVisible(false);
    ui->startbtn->setVisible(true);
    ui->stopbtn->setVisible(true);
    ui->clearbtn->setVisible(true);
//    ui->luzbtn->setVisible(true);
    ui->customPlot->setVisible(true);
}

void MyDialog::on_pdlcbtn_clicked()
{
//    serial_init();
    serial->write("P");
    en_tech = 5;
    ui->label_3->setText("PDLC");
    ui->spdbtn->setVisible(false);
    ui->pdlcbtn->setVisible(false);
    ui->startbtn->setVisible(true);
    ui->stopbtn->setVisible(true);
    ui->clearbtn->setVisible(true);
//    ui->luzbtn->setVisible(true);
    ui->customPlot->setVisible(true);
}

void MyDialog::on_savebtn_clicked()
{
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString(QLatin1String("yyyyMMdd-hhmm"));
    QString filename = QString::fromLatin1("C:/medicion/%1.csv").arg(timestamp);
    QString filename_img = QString::fromLatin1("C:/medicion/%1.jpg").arg(timestamp);
    QFile file(filename);
    QFile file_img(filename_img);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text) && !file_img.open(QIODevice::WriteOnly | QFile::WriteOnly))
        return;
    ui->customPlot->saveJpg(filename_img,0, 0, 1.0, -1,96);


    QTextStream stream(&file);
    stream << date << "\n";
    stream<<"T: "<<","<< temperature <<"°C"<< "\n";
    stream<<"El tiempo de subida es: "<<","<<t_s<<"s"<< "\n";
    stream<<"El tiempo de bajada es: "<<","<<t_b<<"s"<< "\n";
    stream <<"tiempo(s)"<<","<<"Amplitud"<<"\n";
    for(int i=0;i<s_n.size();i++)
        stream<<s_t.at(i)<<","<<s_n.at(i)<<"\n";


    file.flush();
    file.close();
    QMessageBox msgBox;
    msgBox.setWindowTitle("=^..^=");
    msgBox.setWindowIcon((QIcon("C:/Users/tavo_/Documents/untitled/images/action_success.png")));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("Se creó el archivo: "+filename+" y la imagen: "+filename_img);
    msgBox.exec();
}
