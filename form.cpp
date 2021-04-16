#include "form.h"
#include "ui_form.h"
#include <QSerialPort>
#include <QDebug>
#include <QTimer>
#include <QtEndian>
#include <QVector>
#include <QElapsedTimer>
QSerialPort *serial;
QTimer dataTimer;
QVector<double> vector;
QVector<double> vector_rt;
QVector<double> vec_tiempo;
QVector<double> vec_n;
QElapsedTimer timer1;


bool start = false;
int en = 0;
int en_rst=0;
qint64 cnt = 0;
double curr_t = 0;

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
    serial->close();
    vector.clear();
    vec_tiempo.clear();
}
