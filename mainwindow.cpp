#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"
#include "mydialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidget::setWindowTitle("Sistema de medición de tiempo AGP");
    QWidget::setWindowIcon(QIcon("C:/Users/tavo_/Documents/untitled/images/logo2_2.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_medirbtn_clicked()
{

   mDialog = new MyDialog(this);
   mDialog->show();

}
