#include "c8051f020.h" 
#include <intrins.h> 
void Delay1us(unsigned char us)//延时us
{
	while (us)
	{
	  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	  --us;
	}
}
void SYSCLK_Init (void)//系统时钟初始化
{
   int i;                              // delay counter
   OSCXCN = 0x67;                      // start external oscillator with
                                       // 18.432MHz crystal
   for (i=0; i < 256; i++) ;           // Wait for osc. to start up
   while (!(OSCXCN & 0x80)) ;          // Wait for crystal osc. to settle
   OSCICN = 0x88;                      // select external oscillator as SYSCLK
                                       // source and enable missing clock
                                       // detector
										//	OSCICN = 0x07;   //interal 16MHZ
}
#define PRT0CF P0MDOUT
#define PRT1CF P1MDOUT
#define PRT2CF P2MDOUT
void PORT_Init (void)                 //串口初始化
{
   XBR0    = 0x07;                     // Enable SMBus, SPI0, and UART0
   XBR1    = 0x00;
   XBR2    = 0x44;                     // Enable crossbar and weak pull-ups
   EMI0CF  = 0x27;
   EMI0TC  = 0x21;
   P74OUT  = 0xFF;
   P0MDOUT = 0x15;
 P1MDOUT |= 0x3C;  					//P1.2-P1.5推挽输出
   P1 &= 0xc3;							//P1.2-P1.5=0
}
void SPI0_Init (void)                  //spi0初始化
{
   SPI0CFG = 0x07;                     // data sampled on 1st SCK rising edge
                                       // 8-bit data words
   SPI0CFG|=0xC0;	//CKPOL =1;

   SPI0CN = 0x03;                      // Master mode; SPI enabled; flags
                                       // cleared
   SPI0CKR = SYSCLK/2/8000000-1;       // SPI clock <= 8MHz (limited by 
                                       // EEPROM spec.)
}
unsigned char Count1ms;
void Timer0_Init (void)                //计数器0初始化
{
   	CKCON|=0x8;
   	TMOD|=0x1;    	//16Bit
	Count1ms=10;
   	TR0 = 0;                         	// STOP Timer0
   	TH0 = (-SYSCLK/1000) >> 8;    		// set Timer0 to overflow in 1ms
   	TL0 = -SYSCLK/1000;
   	TR0 = 1;   	// START Timer0
   	IE|= 0x2; 
}
void Timer0_ISR (void) interrupt 1  //1ms  
{
	TH0 = (-SYSCLK/1000) >> 8;  
   	TL0 = -SYSCLK/1000;
	if (Count1ms) Count1ms--;
}
void Delay1ms(unsigned char T)        //延时1ms
{
	Count1ms=T;
	while (Count1ms);
}
void Delay1s(unsigned char T)        //延时1s
{
	while (T)
	{
		Delay1ms(200);
		Delay1ms(200);
		Delay1ms(200);
		Delay1ms(200);
		Delay1ms(200);
		T--;
	}
}
void TestLCD(void);//测试初始画面
char GetKeyValue(void);//获取键值
void init_adc(void);
unsigned int GetADCValue(char No);
void Display (char *DispBuf,char ShowDot,char x,char y);
void menulcd ();
void zuoyouyi();
void caiji();
void miaobiao();
void calc();
void main (void) 
{
	char i;
	unsigned int u;
	char Num;
	WDTCN = 0xde;
	WDTCN = 0xad;           //关看门狗
	SYSCLK_Init ();         //初始化时钟
	Timer0_Init();			//初始化定时器
	PORT_Init ();           //初始化IO口
	SPI0_Init ();           //初始化SPI0
    init_adc();
	CPT1CN|=0x80;	//使能比较器1
	REF0CN = 0x07;  //使能片内参考电压
	DAC0CN |= 0x80;	//使能DAC0
	DAC0H=0;	DAC0L=0;
    EA=1;					//开中断
    Num=0;
    menulcd ();
	u= GetADCValue(Num);//获取按键值
	for (;;)

	{
		i=GetKeyValue();
		if ((i>=0)&&(i<=5))
			Num=i;
		else
			Num=0;

		Delay1ms(100);
		if (Num==0)
		{
			menulcd ();//按0显示主菜单
		}
		else if(Num==1)
 		{
			TestLCD();//按1进入字符方式
		}
		else if(Num==2)
		{
			zuoyouyi();//按2图片方式
		}
		else if(Num==3)
		{
			caiji();//按3采集温度，电压，压力
		}
		else if(Num==4)
		{
			miaobiao();//按4时钟
		}
		else if(Num==5)
		{
			calc();//按5计算器
		}
		u = GetADCValue(Num);
	}
}