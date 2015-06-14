#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QBuffer>
#include <QImage>
#include <QPixmap>
#include <QByteArray>
#include <QApplication>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QFile>
#include <QLabel>
#include <QPixmap>
#include <QComboBox>

typedef char CHAR ;
typedef short SHORT ;
typedef long LONG ;
typedef unsigned long DWORD ;
typedef int BOOL ;
typedef unsigned char BYTE ;
typedef unsigned short WORD ;
typedef struct tagBITMAPINFOHEADER  //位图信息头
{
    DWORD biSize ;
    LONG  biWidth ;
    LONG  biHeight ;
    WORD  biPlanes ;
    WORD  biBitCount ;
    DWORD biCompression ;
    DWORD biSizeImage ;
    LONG  biXPelsPerMeter ;
    LONG  biYPelsPerMeter ;
    DWORD biClrUsed ;
    DWORD biClrImportant ;
}BITMAPINFOHEADER;
typedef  struct tagBITMAPFILEHEADER //位图文件头
{
    short bfType ;
    DWORD bfSize ;
    WORD  bfReserved1 ;
    WORD  bfReserved2 ;
    DWORD bfOffBits ;
}BITMAPFILEHEADER ;
typedef struct tagRGBQUAD //实际图像数据
{
    BYTE rgbBlue ;
    BYTE rgbGreen ;
    BYTE rgbRed ;
    BYTE rgbReserved ;
}RGBQUAD ;

#define FALSE 0
#define TRUE 1
/* constants for the biCompression field */
#define BI_RGB       0L
#define BI_RLE8      1L
#define BI_RLE4      2L
#define BI_BITFIELDS 3L
#define M_SOF0  0xc0
#define M_DHT   0xc4
#define M_EOI   0xd9
#define M_SOS   0xda
#define M_DQT   0xdb
#define M_DRI   0xdd
#define M_APP0  0xe0

#define W1 2841 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565  /* 2048*sqrt(2)*cos(7*pi/16) */

#define MAKEWORD(a,b) ((WORD)(((BYTE)(a))|((WORD)((BYTE)(b)))<<8))

//macro definition
#define WIDTHBYTES(i) ((i+31)/32*4)
#define PI 3.1415926535
//define return value of function
#define FUNC_OK             0
#define FUNC_MEMORY_ERROR   1
#define FUNC_FILE_ERROR     2
#define FUNC_FORMAT_ERROR   3

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void LoadJpegFile();
    void showerror(int funcret);
    int InitTag(void);
    void InitTable(void);
    int Decode(void);
    int DecodeMCUBlock(void);
    int HufBlock(BYTE dchufindex,BYTE achufindex);
    int DecodeElement(void);
    void IQtIZzMCUComponent(short flag);
    void IQtIZzBlock(short*s,int*d,short flag);
    void GetYUV(short flag);
    void StoreBuffer(void);
    BYTE ReadByte(void);
    void Initialize_Fast_IDCT(void);
    void Fast_IDCT(int*block);
    void idctrow(int*blk);
    void idctcol(int*blk);
    void workwithdata();
private slots:
    void Browse();
    void openSerial();
    void sendData();
    void readData();
private:
    Ui::Widget *ui;
    QByteArray Rxdata;
    QSerialPort *myport;
    bool tranok;


    BITMAPFILEHEADER bf ;
    BITMAPINFOHEADER bi ;
    unsigned char  *hImgData ;
    DWORD NumColors ;
    DWORD LineBytes ;
    DWORD ImgWidth,ImgHeight ;
    short SampRate_Y_H,SampRate_Y_V ;
    short SampRate_U_H,SampRate_U_V ;
    short SampRate_V_H,SampRate_V_V ;
    short H_YtoU,V_YtoU,H_YtoV,V_YtoV ;
    short Y_in_MCU,U_in_MCU,V_in_MCU ;
    unsigned char *lpJpegBuf ;
    unsigned char *lp ;
    short qt_table[3][64];
    short comp_num ;
    BYTE comp_index[3];
    BYTE YDcIndex,YAcIndex,UVDcIndex,UVAcIndex ;
    BYTE HufTabIndex ;
    short *YQtTable,*UQtTable,*VQtTable ;
    short code_pos_table[4][16],code_len_table[4][16];
    unsigned short code_value_table[4][256];
    unsigned short huf_max_value[4][16],huf_min_value[4][16];
    short BitPos,CurByte ;
    short rrun,vvalue ;
    short MCUBuffer[10*64];
    int QtZzMCUBuffer[10*64];
    short BlockBuffer[64];
    short ycoef,ucoef,vcoef ;
    BOOL IntervalFlag ;
    short interval ;
    int Y[4*64],U[4*64],V[4*64];
    DWORD sizei,sizej ;
    short restart ;
    QByteArray ba;
    QByteArray hh;
    QByteArray res;
    QByteArray subdata[9];
    unsigned char cntpos;

};








#endif // WIDGET_H
