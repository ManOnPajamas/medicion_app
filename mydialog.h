#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>

namespace Ui {
class MyDialog;
}

class MyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MyDialog(QWidget *parent = nullptr);
    ~MyDialog();

private slots:
    void serialRecieved();
    void makePlot();
    void realtimeDataSlot();
    void on_startbtn_clicked();

    void on_stopbtn_clicked();

    void on_clearbtn_clicked();

    void on_luzbtn_clicked();

    void serial_init();

    void serialSent(const QByteArray &data);
    void fuente();
    void graph_init();
    void on_spdbtn_clicked();

    void on_pdlcbtn_clicked();

    void on_savebtn_clicked();

private:
    Ui::MyDialog *ui;
};

#endif // MYDIALOG_H
