/*久坐提醒装置实现代码*/
static const unsigned char PROGMEM str_1[] =  
{   
0x00,0x08,0x00,0x08,0x7C,0x08,0x44,0x08,0x45,0xFE,0x44,0x08,0x44,0x08,0x7C,0x08,
0x44,0x88,0x44,0x48,0x44,0x48,0x44,0x08,0x7C,0x08,0x44,0x08,0x00,0x28,0x00,0x10

  }; //时
  static const unsigned char PROGMEM str_2[] =  
{   
0x20,0x00,0x13,0xFC,0x10,0x04,0x40,0x04,0x47,0xC4,0x44,0x44,0x44,0x44,0x44,0x44,
0x47,0xC4,0x44,0x44,0x44,0x44,0x44,0x44,0x47,0xC4,0x40,0x04,0x40,0x14,0x40,0x08

  }; //间
  /*
 * OLED显示
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET     4
Adafruit_SSD1306 display(128, 64, &Wire,OLED_RESET);
#define LOGO16_GLCD_HEIGHT 16     //定义显示高度
#define LOGO16_GLCD_WIDTH  16     //定义显示宽度

float      distance;
const int  echo=3;                          //echO接D3脚
const int  trig=2;                        //echO接D2脚
int time=0;                   //久坐计时  
int leave=0;                      //离座计时
int limitdistance=45.00;                //设定限制距离！！！！！！可自行调整！！！！！
int limittime=3600;                //设定报警时间！！！！！可自行调整！！！！！
void setup()    
{
  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.setTextColor(WHITE);      //开像素点发光
  display.display();
  delay(2000);
  
  Serial.begin(9600);                       //波特率9600
  pinMode(echo,INPUT);                       //设置echo为输入脚
  pinMode(trig,OUTPUT);                      //设置trig为输出脚
   pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  Serial.println("HC-SR04测距开始：");
}
void loop()
{
  display.clearDisplay();
  //显示中文
   display.setTextSize(1);//设置字体大小  
  
   display.drawBitmap(0,0 , str_1, 16, 16, 1);   //oled显示汉字
   display.drawBitmap(16,0 , str_2, 16, 16, 1);   //oled显示汉字
   display.setCursor(32,0);               //设置显示位置
   display.setTextSize(3);             //设置字体大小
   display.print(":");

   
   //显示英文
  display.setTextSize(2);         //设置字体大小  
   display.setCursor(50, 0);        //设置显示位置
  display.println(time);
  if(time>=limittime){
    
    display.setCursor(32,32);
   display.setTextSize(2);             //设置字体大小
   display.print("Stop");
   display.setCursor(32,48);
    display.print("sitting");        //久坐时间达到设定值后在oled上显示“Stopsitting”
    }
  display.display();
  delay(500);
digitalWrite(trig,LOW);
delayMicroseconds(20); 
digitalWrite(trig,HIGH);
delayMicroseconds(20);
digitalWrite(trig,LOW);                     //发一个20US的高脉冲去触发Trig
unsigned int dutyCycle;                     //占空比
distance  = pulseIn(echo,HIGH);             //计数接收高电平时间
distance  = distance*340/2/10000;           //计算距离 1：声速：340M/S  2：实际距离1/2声速距离 3：计数时钟为1US
  if(distance<limitdistance){
        time=time+1;
  }
      if (time<limittime) {                    //小于门限值
        dutyCycle = 0; 
        digitalWrite(11, LOW);
      }
    else{                                        //发光二极管亮度最大值，并启动声音报警
          dutyCycle = 255; analogWrite(11,200);
        } 
   if(distance>30.00){          //离开时计时
      leave=leave+1;
    }
   if ((leave>=10)&&(time>=limittime)){                //离开一定时间后关闭报警
    digitalWrite(11, LOW);    
    }
    if(leave>=60){
        time=0;               //离开一定时间后重置久坐时间
      }
    if (leave>=60||(leave>=10&distance<limitdistance)){                 //离开一定时间后重置离开时间
        leave=0;
      }
   
        
  analogWrite(10, dutyCycle);                 //控制发光二极管发光
  
Serial.print("距离: ");
Serial.print(distance);
Serial.print("cm");
Serial.print("     ");            
Serial.print("时间");
Serial.print(time);
Serial.print("s");
Serial.print("     ");
Serial.print("离开");
Serial.print(leave);
Serial.println("s");                    //在串口监视器中查看
delay(20);                                    //单次测离完成后加20mS的延时再进行下次测量。防止近距离测量时，测量到上次余波，导致测量不准确。
delay(500);                                  //500mS测量一次
}
