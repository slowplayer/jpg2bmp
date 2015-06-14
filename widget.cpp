#include "widget.h"
#include "ui_widget.h"

static int Zig_Zag[8][8]={
                          {0,1,5,6,14,15,27,28},
              {2,4,7,13,16,26,29,42},
              {3,8,12,17,25,30,41,43},
              {9,11,18,24,37,40,44,53},
              {10,19,23,32,39,45,52,54},
              {20,22,33,38,46,51,55,60},
              {21,34,37,47,50,56,59,61},
              {35,36,48,49,57,58,62,63}
             };
static long iclip[1024];
static long *iclp ;
BYTE And[9]=
{
    0,1,3,7,0xf,0x1f,0x3f,0x7f,0xff
};
void Widget::idctrow(int*blk)
{
   int x0,x1,x2,x3,x4,x5,x6,x7,x8 ;
   //intcut
   if(!((x1=blk[4]<<11)|(x2=blk[6])|(x3=blk[2])|
   (x4=blk[1])|(x5=blk[7])|(x6=blk[5])|(x7=blk[3])))
   {
       blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]<<3 ;
       return ;
   }
   x0=(blk[0]<<11)+128 ;
   // for proper rounding in the fourth stage
   //first stage
   x8=W7*(x4+x5);
   x4=x8+(W1-W7)*x4 ;
   x5=x8-(W1+W7)*x5 ;
   x8=W3*(x6+x7);
   x6=x8-(W3-W5)*x6 ;
   x7=x8-(W3+W5)*x7 ;
   //second stage
   x8=x0+x1 ;
   x0-=x1 ;
   x1=W6*(x3+x2);
   x2=x1-(W2+W6)*x2 ;
   x3=x1+(W2-W6)*x3 ;
   x1=x4+x6 ;
   x4-=x6 ;
   x6=x5+x7 ;
   x5-=x7 ;
   //third stage
   x7=x8+x3 ;
   x8-=x3 ;
   x3=x0+x2 ;
   x0-=x2 ;
   x2=(181*(x4+x5)+128)>>8 ;
   x4=(181*(x4-x5)+128)>>8 ;
   //fourth stage
   blk[0]=(x7+x1)>>8 ;
   blk[1]=(x3+x2)>>8 ;
   blk[2]=(x0+x4)>>8 ;
   blk[3]=(x8+x6)>>8 ;
   blk[4]=(x8-x6)>>8 ;
   blk[5]=(x0-x4)>>8 ;
   blk[6]=(x3-x2)>>8 ;
   blk[7]=(x7-x1)>>8 ;
}
//////////////////////////////////////////////////////////////////////////////
void Widget::idctcol(int*blk)
{
   int x0,x1,x2,x3,x4,x5,x6,x7,x8 ;
   //intcut
   if(!((x1=(blk[8*4]<<8))|(x2=blk[8*6])|(x3=blk[8*2])|
   (x4=blk[8*1])|(x5=blk[8*7])|(x6=blk[8*5])|(x7=blk[8*3])))
   {
       blk[8*0]=blk[8*1]=blk[8*2]=blk[8*3]=blk[8*4]=blk[8*5]
       =blk[8*6]=blk[8*7]=iclp[(blk[8*0]+32)>>6];
       return ;
   }
   x0=(blk[8*0]<<8)+8192 ;
   //first stage
   x8=W7*(x4+x5)+4 ;
   x4=(x8+(W1-W7)*x4)>>3 ;
   x5=(x8-(W1+W7)*x5)>>3 ;
   x8=W3*(x6+x7)+4 ;
   x6=(x8-(W3-W5)*x6)>>3 ;
   x7=(x8-(W3+W5)*x7)>>3 ;
   //second stage
   x8=x0+x1 ;
   x0-=x1 ;
   x1=W6*(x3+x2)+4 ;
   x2=(x1-(W2+W6)*x2)>>3 ;
   x3=(x1+(W2-W6)*x3)>>3 ;
   x1=x4+x6 ;
   x4-=x6 ;
   x6=x5+x7 ;
   x5-=x7 ;
   //third stage
   x7=x8+x3 ;
   x8-=x3 ;
   x3=x0+x2 ;
   x0-=x2 ;
   x2=(181*(x4+x5)+128)>>8 ;
   x4=(181*(x4-x5)+128)>>8 ;
   //fourth stage
   blk[8*0]=iclp[(x7+x1)>>14];
   blk[8*1]=iclp[(x3+x2)>>14];
   blk[8*2]=iclp[(x0+x4)>>14];
   blk[8*3]=iclp[(x8+x6)>>14];
   blk[8*4]=iclp[(x8-x6)>>14];
   blk[8*5]=iclp[(x0-x4)>>14];
   blk[8*6]=iclp[(x3-x2)>>14];
   blk[8*7]=iclp[(x7-x1)>>14];
}
void Widget::Fast_IDCT(int*block)
{
   short i ;

   for(i=0;i<8;i++)
   idctrow(block+8*i);

   for(i=0;i<8;i++)
   idctcol(block+i);
}
void Widget::IQtIZzBlock(short*s,int*d,short flag)
{
   short i,j ;
   short tag ;
   short*pQt ;
   int buffer2[8][8];
   int*buffer1 ;
   short offset ;

   switch(flag)
   {
       case 0 :
       pQt=YQtTable ;
       offset=128 ;
       break ;
       case 1 :
       pQt=UQtTable ;
       offset=0 ;
       break ;
       case 2 :
       pQt=VQtTable ;
       offset=0 ;
       break ;
   }

   for(i=0;i<8;i++)
   for(j=0;j<8;j++)
   {
       tag=Zig_Zag[i][j];
       buffer2[i][j]=(int)s[tag]*(int)pQt[tag];
   }
   buffer1=(int*)buffer2 ;
   Fast_IDCT(buffer1);
   for(i=0;i<8;i++)
   for(j=0;j<8;j++)
   d[i*8+j]=buffer2[i][j]+offset ;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void Widget::Initialize_Fast_IDCT(void)
{
   short i ;

   iclp=iclip+512 ;
   for(i=-512;i<512;i++)
   iclp[i]=(i<-256)?-256:((i>255)?255:i);
}
////////////////////////////////////////////////////////////////////////

void Widget::InitTable(void)
{
    short i,j ;
    sizei=sizej=0 ;
    ImgWidth=ImgHeight=0 ;
    rrun=vvalue=0 ;
    BitPos=0 ;
    CurByte=0 ;
    IntervalFlag=FALSE ;
    restart=0 ;
    for(i=0;i<3;i++)
    for(j=0;j<64;j++)
    qt_table[i][j]=0 ;
    comp_num=0 ;
    HufTabIndex=0 ;
    for(i=0;i<3;i++)
    comp_index[i]=0 ;
    for(i=0;i<4;i++)
    for(j=0;j<16;j++)
    {
        code_len_table[i][j]=0 ;
        code_pos_table[i][j]=0 ;
        huf_max_value[i][j]=0 ;
        huf_min_value[i][j]=0 ;
    }
    for(i=0;i<4;i++)
    for(j=0;j<256;j++)
    code_value_table[i][j]=0 ;

    for(i=0;i<10*64;i++) { MCUBuffer[i]=0 ; QtZzMCUBuffer[i]=0 ; }
    for(i=0;i<64;i++) { Y[i]=0 ; U[i]=0 ; V[i]=0 ; BlockBuffer[i]=0 ; }
    ycoef=ucoef=vcoef=0 ;
}
int Widget::InitTag(void)
{
    BOOL finish=FALSE ;
    BYTE id ;
    short llength ;
    short i,j,k ;
    short huftab1,huftab2 ;
    short huftabindex ;
    BYTE hf_table_index ;
    BYTE qt_table_index ;
    BYTE comnum ;

    unsigned char  *lptemp ;
    short ccount ;

    lp=lpJpegBuf+2 ;

    while(!finish)
    {
        id=*(lp+1);
        lp+=2 ;
      //  qDebug()<<*lp<<" "<<*(lp+1);
        switch(id)
        {
            case M_APP0 :
            qDebug()<<"APP0";
            llength=MAKEWORD(*(lp+1),*lp);
         //   qDebug()<<llength;
            lp+=llength ;
            break ;
            case M_DQT :
            qDebug()<<"DQT";
            llength=MAKEWORD(*(lp+1),*lp);
            qt_table_index=(*(lp+2))&0x0f ;
            lptemp=lp+3 ;
            if(llength<80)
            {
                for(i=0;i<64;i++)
                qt_table[qt_table_index][i]=(short)*(lptemp++);
            }
            else
            {
                for(i=0;i<64;i++)
                qt_table[qt_table_index][i]=(short)*(lptemp++);
                qt_table_index=(*(lptemp++))&0x0f ;
                for(i=0;i<64;i++)
                qt_table[qt_table_index][i]=(short)*(lptemp++);
            }
            lp+=llength ;
            break ;
            case M_SOF0 :
            qDebug()<<"SOF0";
            llength=MAKEWORD(*(lp+1),*lp);
            ImgHeight=MAKEWORD(*(lp+4),*(lp+3));
            ImgWidth=MAKEWORD(*(lp+6),*(lp+5));
            comp_num=*(lp+7);
            if((comp_num!=1)&&(comp_num!=3))
            return FUNC_FORMAT_ERROR ;
            if(comp_num==3)
            {
                comp_index[0]=*(lp+8);
                SampRate_Y_H=(*(lp+9))>>4 ;
                SampRate_Y_V=(*(lp+9))&0x0f ;
                YQtTable=(short*)qt_table[*(lp+10)];

                comp_index[1]=*(lp+11);
                SampRate_U_H=(*(lp+12))>>4 ;
                SampRate_U_V=(*(lp+12))&0x0f ;
                UQtTable=(short*)qt_table[*(lp+13)];

                comp_index[2]=*(lp+14);
                SampRate_V_H=(*(lp+15))>>4 ;
                SampRate_V_V=(*(lp+15))&0x0f ;
                VQtTable=(short*)qt_table[*(lp+16)];
            }
            else
            {
                comp_index[0]=*(lp+8);
                SampRate_Y_H=(*(lp+9))>>4 ;
                SampRate_Y_V=(*(lp+9))&0x0f ;
                YQtTable=(short*)qt_table[*(lp+10)];

                comp_index[1]=*(lp+8);
                SampRate_U_H=1 ;
                SampRate_U_V=1 ;
                UQtTable=(short*)qt_table[*(lp+10)];

                comp_index[2]=*(lp+8);
                SampRate_V_H=1 ;
                SampRate_V_V=1 ;
                VQtTable=(short*)qt_table[*(lp+10)];
            }
            lp+=llength ;
            break ;
            case M_DHT :
            qDebug()<<"DHT";
            llength=MAKEWORD(*(lp+1),*lp);
            if(llength<0xd0)
            {
                huftab1=(short)(*(lp+2))>>4 ;                //huftab1=0,1
                huftab2=(short)(*(lp+2))&0x0f ;              //huftab2=0,1
                huftabindex=huftab1*2+huftab2 ;
                lptemp=lp+3 ;
                for(i=0;i<16;i++)
                code_len_table[huftabindex][i]=(short)(*(lptemp++));
                j=0 ;
                for(i=0;i<16;i++)
                if(code_len_table[huftabindex][i]!=0)
                {
                    k=0 ;
                    while(k<code_len_table[huftabindex][i])
                    {
                        code_value_table[huftabindex][k+j]=(short)(*(lptemp++));
                        k++;
                    }
                    j+=k ;
                }
                i=0 ;
                while(code_len_table[huftabindex][i]==0)
                i++;
                for(j=0;j<i;j++)
                {
                    huf_min_value[huftabindex][j]=0 ;
                    huf_max_value[huftabindex][j]=0 ;
                }
                huf_min_value[huftabindex][i]=0 ;
                huf_max_value[huftabindex][i]=code_len_table[huftabindex][i]-1 ;
                for(j=i+1;j<16;j++)
                {
                    huf_min_value[huftabindex][j]=(huf_max_value[huftabindex][j-1]+1)<<1 ;
                    huf_max_value[huftabindex][j]=huf_min_value[huftabindex][j]+code_len_table[huftabindex][j]-1 ;
                }
                code_pos_table[huftabindex][0]=0 ;
                for(j=1;j<16;j++)
                code_pos_table[huftabindex][j]=code_len_table[huftabindex][j-1]+code_pos_table[huftabindex][j-1];
                lp+=llength ;
            }
            //if
            else
            {
                hf_table_index=*(lp+2);
                lp+=2 ;
                while(hf_table_index!=0xff)
                {
                    huftab1=(short)hf_table_index>>4 ;                    //huftab1=0,1
                    huftab2=(short)hf_table_index&0x0f ;                  //huftab2=0,1
                    huftabindex=huftab1*2+huftab2 ;
                    lptemp=lp+1 ;
                    ccount=0 ;
                    for(i=0;i<16;i++)
                    {
                        code_len_table[huftabindex][i]=(short)(*(lptemp++));
                        ccount+=code_len_table[huftabindex][i];
                    }
                    ccount+=17 ;
                    j=0 ;
                    for(i=0;i<16;i++)
                    if(code_len_table[huftabindex][i]!=0)
                    {
                        k=0 ;
                        while(k<code_len_table[huftabindex][i])
                        {
                            code_value_table[huftabindex][k+j]=(short)(*(lptemp++));
                            k++;
                        }
                        j+=k ;
                    }
                    i=0 ;
                    while(code_len_table[huftabindex][i]==0)
                    i++;
                    for(j=0;j<i;j++)
                    {
                        huf_min_value[huftabindex][j]=0 ;
                        huf_max_value[huftabindex][j]=0 ;
                    }
                    huf_min_value[huftabindex][i]=0 ;
                    huf_max_value[huftabindex][i]=code_len_table[huftabindex][i]-1 ;
                    for(j=i+1;j<16;j++)
                    {
                        huf_min_value[huftabindex][j]=(huf_max_value[huftabindex][j-1]+1)<<1 ;
                        huf_max_value[huftabindex][j]=huf_min_value[huftabindex][j]+code_len_table[huftabindex][j]-1 ;
                    }
                    code_pos_table[huftabindex][0]=0 ;
                    for(j=1;j<16;j++)
                    code_pos_table[huftabindex][j]=code_len_table[huftabindex][j-1]+code_pos_table[huftabindex][j-1];
                    lp+=ccount ;
                    hf_table_index=*lp ;
                }
                //while
            }
            //else
            break ;
            case M_DRI :
            qDebug()<<"DRI";
            llength=MAKEWORD(*(lp+1),*lp);
            restart=MAKEWORD(*(lp+3),*(lp+2));
            lp+=llength ;
            break ;
            case M_SOS :
            qDebug()<<"SOS";
            llength=MAKEWORD(*(lp+1),*lp);
            comnum=*(lp+2);
            if(comnum!=comp_num)
            return FUNC_FORMAT_ERROR ;
            lptemp=lp+3 ;
            for(i=0;i<comp_num;i++)
            {
                if(*lptemp==comp_index[0])
                {
                    YDcIndex=(*(lptemp+1))>>4 ;                    //Y
                    YAcIndex=((*(lptemp+1))&0x0f)+2 ;
                }
                else
                {
                    UVDcIndex=(*(lptemp+1))>>4 ;                    //U,V
                    UVAcIndex=((*(lptemp+1))&0x0f)+2 ;
                }
                lptemp+=2 ;
            }
            lp+=llength ;
            finish=TRUE ;
            if(finish)qDebug()<<*lp;
            break ;
            case M_EOI :
            qDebug()<<"EOI";
            return FUNC_FORMAT_ERROR ;
            default :
            if((id&0xf0)!=0xd0)
            {
                llength=MAKEWORD(*(lp+1),*lp);
                lp+=llength ;
            }
            else lp+=2 ;
            break ;
        }//switch
    }//while
    return FUNC_OK ;
}

BYTE Widget::ReadByte(void)
{
   BYTE i ;

   i=*(lp++);
   if(i==0xff)
   lp++;
   BitPos=8 ;
   CurByte=i ;
   return i ;
}
int Widget::DecodeElement(void)
{
   int thiscode,tempcode ;
   unsigned short temp,valueex ;
   short codelen ;
   BYTE hufexbyte,runsize,tempsize,sign ;
   BYTE newbyte,lastbyte ;

   if(BitPos>=1)
   {
       BitPos--;
       thiscode=(BYTE)CurByte>>BitPos ;
       CurByte=CurByte&And[BitPos];
   }
   else
   {
       lastbyte=ReadByte();
       BitPos--;
       newbyte=CurByte&And[BitPos];
       thiscode=lastbyte>>7 ;
       CurByte=newbyte ;
   }
   codelen=1 ;
   while((thiscode<huf_min_value[HufTabIndex][codelen-1])||
   (code_len_table[HufTabIndex][codelen-1]==0)||
   (thiscode>huf_max_value[HufTabIndex][codelen-1]))
   {
       if(BitPos>=1)
       {
           BitPos--;
           tempcode=(BYTE)CurByte>>BitPos ;
           CurByte=CurByte&And[BitPos];
       }
       else
       {
           lastbyte=ReadByte();
           BitPos--;
           newbyte=CurByte&And[BitPos];
           tempcode=(BYTE)lastbyte>>7 ;
           CurByte=newbyte ;
       }
       thiscode=(thiscode<<1)+tempcode ;
       codelen++;
       if(codelen>16)
       return FUNC_FORMAT_ERROR ;
   }
   //while
   temp=thiscode-huf_min_value[HufTabIndex][codelen-1]+code_pos_table[HufTabIndex][codelen-1];
   hufexbyte=(BYTE)code_value_table[HufTabIndex][temp];
   rrun=(short)(hufexbyte>>4);
   runsize=hufexbyte&0x0f ;
   if(runsize==0)
   {
       vvalue=0 ;
       return FUNC_OK ;
   }
   tempsize=runsize ;
   if(BitPos>=runsize)
   {
       BitPos-=runsize ;
       valueex=(BYTE)CurByte>>BitPos ;
       CurByte=CurByte&And[BitPos];
   }
   else
   {
       valueex=CurByte ;
       tempsize-=BitPos ;
       while(tempsize>8)
       {
           lastbyte=ReadByte();
           valueex=(valueex<<8)+(BYTE)lastbyte ;
           tempsize-=8 ;
       }
       //while
       lastbyte=ReadByte();
       BitPos-=tempsize ;
       valueex=(valueex<<tempsize)+(lastbyte>>BitPos);
       CurByte=lastbyte&And[BitPos];
   }
   //else
   sign=valueex>>(runsize-1);
   if(sign)
   vvalue=valueex ;
   else
   {
       valueex=valueex^0xffff ;
       temp=0xffff<<runsize ;
       vvalue=-(short)(valueex^temp);
   }
   return FUNC_OK ;
}
int Widget::HufBlock(BYTE dchufindex,BYTE achufindex)
{
   short count=0 ;
   short i ;
   int funcret ;

   //dc
   HufTabIndex=dchufindex ;
   funcret=DecodeElement();
   if(funcret!=FUNC_OK)
   return funcret ;

   BlockBuffer[count++]=vvalue ;

   //ac
   HufTabIndex=achufindex ;
   while(count<64)
   {
       funcret=DecodeElement();
       if(funcret!=FUNC_OK)
       return funcret ;
       if((rrun==0)&&(vvalue==0))
       {
           for(i=count;i<64;i++)
           BlockBuffer[i]=0 ;
           count=64 ;
       }
       else
       {
           for(i=0;i<rrun;i++)
           BlockBuffer[count++]=0 ;
           BlockBuffer[count++]=vvalue ;
       }
   }
   return FUNC_OK ;
}
//////////////////////////////////////////////////////////////////////////////

void Widget::GetYUV(short flag)
{
   short H,VV ;
   short i,j,k,h ;
   int*buf ;
   int*pQtZzMCU ;

   switch(flag)
   {
       case 0 :
       H=SampRate_Y_H ;
       VV=SampRate_Y_V ;
       buf=Y ;
       pQtZzMCU=QtZzMCUBuffer ;
       break ;
       case 1 :
       H=SampRate_U_H ;
       VV=SampRate_U_V ;
       buf=U ;
       pQtZzMCU=QtZzMCUBuffer+Y_in_MCU*64 ;
       break ;
       case 2 :
       H=SampRate_V_H ;
       VV=SampRate_V_V ;
       buf=V ;
       pQtZzMCU=QtZzMCUBuffer+(Y_in_MCU+U_in_MCU)*64 ;
       break ;
   }
   for(i=0;i<VV;i++)
   for(j=0;j<H;j++)
   for(k=0;k<8;k++)
   for(h=0;h<8;h++)
   buf[(i*8+k)*SampRate_Y_H*8+j*8+h]=*pQtZzMCU++;
}
///////////////////////////////////////////////////////////////////////////////
void Widget::StoreBuffer(void)
{
   short i,j ;
   unsigned char *lpbmp ;
   unsigned char R,G,B ;
   int y,u,v,rr,gg,bb ;
   for(i=0;i<SampRate_Y_V*8;i++)
   {
       if((sizei+i)<ImgHeight)
       {
           lpbmp=((unsigned char*)hImgData+(DWORD)(ImgHeight-sizei-i-1)*LineBytes+sizej*3);
          //  qDebug()<<lpbmp<<sizei<<sizej;
           for(j=0;j<SampRate_Y_H*8;j++)
           {
               if((sizej+j)<ImgWidth)
               {
                   y=Y[i*8*SampRate_Y_H+j];
                   u=U[(i/V_YtoU)*8*SampRate_Y_H+j/H_YtoU];
                   v=V[(i/V_YtoV)*8*SampRate_Y_H+j/H_YtoV];
                   rr=((y<<8)+18*u+367*v)>>8 ;
                   gg=((y<<8)-159*u-220*v)>>8 ;
                   bb=((y<<8)+411*u-29*v)>>8 ;
                   R=(unsigned char)rr ;
                   G=(unsigned char)gg ;
                   B=(unsigned char)bb ;
                   if(rr&0xffffff00)
                   {
                      if(rr>255)    R=255 ;
                      else if(rr<0) R=0 ;
                   }
                   if(gg&0xffffff00)
                   {
                      if(gg>255)    G=255 ;
                      else if(gg<0) G=0 ;
                   }
                   if(bb&0xffffff00)
                   {
                      if(bb>255)    B=255 ;
                      else if(bb<0) B=0 ;
                   }
                   *lpbmp++=B ;
                   *lpbmp++=G ;
                   *lpbmp++=R ;

               }
               else break ;
           }
       }
       else break ;
   }

}
///////////////////////////////////////////////////////////////////////////////
int Widget::DecodeMCUBlock(void)
{
   short *lpMCUBuffer ;
   short i,j ;
   int funcret ;

   if(IntervalFlag)
   {
       lp+=2 ;
       ycoef=ucoef=vcoef=0 ;
       BitPos=0 ;
       CurByte=0 ;
   }
  // qDebug()<<comp_num;
   switch(comp_num)
   {
       case 3 :
       lpMCUBuffer=MCUBuffer ;
       //Y
       for(i=0;i<SampRate_Y_H*SampRate_Y_V;i++)
       {
           funcret=HufBlock(YDcIndex,YAcIndex);
           if(funcret!=FUNC_OK)
           return funcret ;
           BlockBuffer[0]=BlockBuffer[0]+ycoef ;
           ycoef=BlockBuffer[0];
           for(j=0;j<64;j++)
           *lpMCUBuffer++=BlockBuffer[j];
       }
       //U
       for(i=0;i<SampRate_U_H*SampRate_U_V;i++)
       {
           funcret=HufBlock(UVDcIndex,UVAcIndex);
           if(funcret!=FUNC_OK)
           return funcret ;
           BlockBuffer[0]=BlockBuffer[0]+ucoef ;
           ucoef=BlockBuffer[0];
           for(j=0;j<64;j++)
           *lpMCUBuffer++=BlockBuffer[j];
       }
       //V
       for(i=0;i<SampRate_V_H*SampRate_V_V;i++)
       {
           funcret=HufBlock(UVDcIndex,UVAcIndex);
           if(funcret!=FUNC_OK)
           return funcret ;
           BlockBuffer[0]=BlockBuffer[0]+vcoef ;
           vcoef=BlockBuffer[0];
           for(j=0;j<64;j++)
           *lpMCUBuffer++=BlockBuffer[j];
       }
       break ;
       case 1 :
       lpMCUBuffer=MCUBuffer ;
       funcret=HufBlock(YDcIndex,YAcIndex);
       if(funcret!=FUNC_OK)
       return funcret ;
       BlockBuffer[0]=BlockBuffer[0]+ycoef ;
       ycoef=BlockBuffer[0];
       for(j=0;j<64;j++)
       *lpMCUBuffer++=BlockBuffer[j];
       for(i=0;i<128;i++)
       *lpMCUBuffer++=0 ;
       break ;
       default :
       return FUNC_FORMAT_ERROR ;
   }
   return FUNC_OK ;
}
//////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
void Widget::IQtIZzMCUComponent(short flag)
{
   short H,VV ;
   short i,j ;
   int*pQtZzMCUBuffer ;
   short*pMCUBuffer ;

   switch(flag)
   {
       case 0 :
       H=SampRate_Y_H ;
       VV=SampRate_Y_V ;
       pMCUBuffer=MCUBuffer ;
       pQtZzMCUBuffer=QtZzMCUBuffer ;
       break ;
       case 1 :
       H=SampRate_U_H ;
       VV=SampRate_U_V ;
       pMCUBuffer=MCUBuffer+Y_in_MCU*64 ;
       pQtZzMCUBuffer=QtZzMCUBuffer+Y_in_MCU*64 ;
       break ;
       case 2 :
       H=SampRate_V_H ;
       VV=SampRate_V_V ;
       pMCUBuffer=MCUBuffer+(Y_in_MCU+U_in_MCU)*64 ;
       pQtZzMCUBuffer=QtZzMCUBuffer+(Y_in_MCU+U_in_MCU)*64 ;
       break ;
   }
   for(i=0;i<VV;i++)
   for(j=0;j<H;j++)
   IQtIZzBlock(pMCUBuffer+(i*H+j)*64,pQtZzMCUBuffer+(i*H+j)*64,flag);
}
//////////////////////////////////////////////////////////////////////////////////////////
int Widget::Decode()
{
   int funcret;
   Y_in_MCU=SampRate_Y_H*SampRate_Y_V ;
   U_in_MCU=SampRate_U_H*SampRate_U_V ;
   V_in_MCU=SampRate_V_H*SampRate_V_V ;
   H_YtoU=SampRate_Y_H/SampRate_U_H ;
   V_YtoU=SampRate_Y_V/SampRate_U_V ;
   H_YtoV=SampRate_Y_H/SampRate_V_H ;
   V_YtoV=SampRate_Y_V/SampRate_V_V ;
   Initialize_Fast_IDCT();
   while((funcret=DecodeMCUBlock())==FUNC_OK)
   {
       interval++;
       if((restart)&&(interval%restart==0))
       IntervalFlag=TRUE ;
       else
       IntervalFlag=FALSE ;
       if(IntervalFlag==TRUE)
           qDebug()<<"ERR!";
       IQtIZzMCUComponent(0);
       IQtIZzMCUComponent(1);
       IQtIZzMCUComponent(2);
       GetYUV(0);
       GetYUV(1);
       GetYUV(2);
       StoreBuffer();
       sizej+=SampRate_Y_H*8 ;
       if(sizej>=ImgWidth)
       {
           sizej=0 ;
           sizei+=SampRate_Y_V*8 ;
       }
       if((sizej==0)&&(sizei>=ImgHeight))
       break ;
   }
   qDebug()<<interval;
   return funcret ;
}
void Widget::LoadJpegFile()
{
    interval=0;
    ImgWidth=0;ImgHeight=0;
    DWORD ImgSize;
    int funcret ;
    lpJpegBuf=(unsigned char *)ba.data();

    InitTable();
    if((funcret=InitTag())!=FUNC_OK)
    {
        qDebug()<<"InitTag"<<" "<<funcret;
    }
    //create new bitmapfileheader and bitmapinfoheader
    memset((char*)&bf,0,sizeof(BITMAPFILEHEADER));
    memset((char*)&bi,0,sizeof(BITMAPINFOHEADER));

    bi.biSize=(DWORD)sizeof(BITMAPINFOHEADER);
    bi.biWidth=(LONG)(ImgWidth);
    bi.biHeight=(LONG)(ImgHeight);
    bi.biPlanes=1 ;
    bi.biBitCount=24 ;
    bi.biClrUsed=0 ;
    bi.biClrImportant=0 ;
    bi.biCompression=BI_RGB ;
    NumColors=0 ;
    qDebug()<<"ImgWidth is "<<bi.biWidth;
    qDebug()<<"bi.biBitCount is "<<bi.biBitCount;
    LineBytes=(unsigned long)WIDTHBYTES(bi.biWidth*bi.biBitCount);
    qDebug()<<"LineBytes is"<<LineBytes;
    qDebug()<<"ImgHeight is "<<bi.biHeight;
    ImgSize=(DWORD)LineBytes*bi.biHeight ;
    qDebug()<<"Image size is "<<ImgSize;
    bf.bfType=0x4d42 ;
    bf.bfSize=sizeof(BITMAPFILEHEADER)-2+sizeof(BITMAPINFOHEADER)+NumColors*sizeof(RGBQUAD)+ImgSize ;
    qDebug()<<"sizeof(BITMAPFILEHEADER) is "<<sizeof(BITMAPFILEHEADER)<<sizeof(bf);
    qDebug()<<bf.bfType<<bf.bfSize;
    qDebug()<<"sizeof(BITMAPINFOHEADER) is "<<sizeof(BITMAPINFOHEADER)<<sizeof(bi);

    bf.bfOffBits=(DWORD)(NumColors*sizeof(RGBQUAD)+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER))-2 ;
    qDebug()<<bf.bfOffBits;
    qDebug()<<SampRate_Y_H<<SampRate_Y_V;
    qDebug()<<ImgSize;
//    if((hImgData=(char *)malloc(sizeof(char)*ImgSize))==NULL)
 //       qDebug()<<"NO!!";
    res.resize(ImgSize);
    hImgData=(unsigned char*)res.data();
    qDebug()<<hImgData;
    funcret=Decode();

}

/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    tranok=false;
    ui->openbt->setEnabled(false);
    ui->sendbt->setEnabled(false);
    connect(ui->browerbt,SIGNAL(clicked()),this,SLOT(Browse()));
    connect(ui->quitbt,SIGNAL(clicked()),qApp,SLOT(quit()));
    connect(ui->openbt,SIGNAL(clicked()),this,SLOT(openSerial()));
    connect(ui->sendbt,SIGNAL(clicked()),this,SLOT(sendData()));
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
            QSerialPort serial;
            serial.setPort(info);
            if(serial.open(QIODevice::ReadWrite))
            {
                ui->comboBox->addItem(info.portName());
                serial.close();
            }
    //        ui->openbt->setEnabled(true);
    }
    setLayout(ui->horizontalLayout);
}

Widget::~Widget()
{
    delete ui;
}
void Widget::readData()
{

    QByteArray Txdata;
    Txdata=myport->readAll();
    cntpos=Txdata[0];
    qDebug()<<"Txdata OK!"<<cntpos;
    if(cntpos<8)sendData();
    else cntpos=0;
}

void Widget::openSerial()
{
    myport=new QSerialPort();
    myport->setPortName(ui->comboBox->currentText());
    myport->open(QIODevice::ReadWrite);
    myport->setBaudRate(115200);
    myport->setDataBits(QSerialPort::Data8);
    myport->setParity(QSerialPort::NoParity);
    myport->setStopBits(QSerialPort::OneStop);
    myport->setFlowControl(QSerialPort::NoFlowControl);
    ui->comboBox->setEnabled(false);
    if(tranok)ui->sendbt->setEnabled(true);
    connect(myport,SIGNAL(readyRead()),this,SLOT(readData()));
    qDebug()<<"Open Serial";
    ui->openbt->setEnabled(false);
}
void Widget::sendData()
{
    qDebug()<<"Send Data"<<subdata[cntpos].size()<<cntpos;
    myport->write(subdata[cntpos]);
}

void Widget::Browse()
{
    QString path=QFileDialog::getOpenFileName(0,tr("选择转换的图片文件"),".","JPG文件(*.jpg);;JPG文件(*.bmp)",0,0);
    ui->lineEdit->setText(path);
    QFileInfo info(path);
    QString filename=info.fileName();
    int index=filename.lastIndexOf(".");
    QString type=filename.mid(index);
    qDebug()<<type;
    QImage img(path);
    QPixmap pix;
    int i,j;
    unsigned char rr,gg,bb;
    if(type==".bmp")
    {
        QBuffer buf(&ba);
        img.save(&buf,"BMP",-1);
        pix.loadFromData(ba,"BMP",Qt::AutoColor);
        qDebug()<<"24真彩色"<<ba.size();
        j=0;
        for(i=0;i<54;i++)
            Rxdata[j++]=ba[i];
        for(i=54;i<ba.size();i+=3)
        {
            bb=(ba[i+0]>>3);
            gg=(ba[i+1]>>2);
            rr=(ba[i+2]>>3);
            Rxdata[j++]=((gg&0x07)<<5)|bb;
            Rxdata[j++]=(rr<<3)|((gg&0x38)>>3);

        }
    }
    else if(type==".jpg")
    {
        QBuffer buf_jpg(&ba);
        img.save(&buf_jpg,"JPG",-1);
        LoadJpegFile();
        QByteArray info;
        info.resize(54*sizeof(char));
        memcpy(info.data(),&bf.bfType,sizeof(short));
        memcpy(info.data()+2*sizeof(char),&bf.bfSize,sizeof(DWORD));
        memcpy(info.data()+6*sizeof(char),&bf.bfReserved1,sizeof(WORD));
        memcpy(info.data()+8*sizeof(char),&bf.bfReserved2,sizeof(WORD));
        memcpy(info.data()+10*sizeof(char),&bf.bfOffBits,sizeof(DWORD));

        memcpy(info.data()+14*sizeof(char),&bi,sizeof(tagBITMAPINFOHEADER));
        QByteArray cc;
        cc=info.toHex();
        QString str1(cc);
        QByteArray hh;
        hh=res.toHex();
        qDebug()<<hh.size();
        QString str2(hh);
        QString str;
        str=str1+str2;
        QByteArray data;
        data=info+res;
        pix.loadFromData(data,"BMP",Qt::AutoColor);
        j=0;
        for(i=0;i<54;i++)
            Rxdata[j++]=data[i];
        for(i=54;i<data.size();i+=3)
        {
            bb=(data[i+0]>>3);
            gg=(data[i+1]>>2);
            rr=(data[i+2]>>3);
            Rxdata[j++]=((gg&0x07)<<5)|bb;
            Rxdata[j++]=(rr<<3)|((gg&0x38)>>3);

        }

    }
    qDebug()<<"16位高彩色"<<Rxdata.size();
    ui->label->setPixmap(pix);
    QByteArray dataready=Rxdata.toHex();
    QString str(dataready);
    QFile file("data.txt");
    if (file.open(QIODevice::WriteOnly))
     {
        QTextStream out(&file);
        qDebug()<<str.size();
        out<<str.toLatin1();
        file.close();
     }
    tranok=true;
    workwithdata();
    ui->openbt->setEnabled(true);
}
void Widget::workwithdata()
{
    int i,j;
    unsigned char sum;
    unsigned char pos=0;
    unsigned char temp;
    for(i=0;i<8;i++)
    {
       sum=0;
       for(j=0;j<19200;j++)
       {
           temp=(unsigned char)Rxdata[54+19200*i+j];
           sum+=temp;
           subdata[i][j]=temp;
       }
       subdata[i][19200]=sum;
       subdata[i][19201]=pos++;
    }
    cntpos=0;
}
