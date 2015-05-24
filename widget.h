#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
private slots:
    void Browse();
    void openSerial();
    void sendData();
private:
    Ui::Widget *ui;
    QByteArray Rxdata;
    QSerialPort *myport;
    bool tranok;

};








#endif // WIDGET_H
