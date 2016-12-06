/************************************************************
����˵����
���������к����gprsģ���ҵ��������źţ��ͻ�����ָ���ķ�������
1.���Լ���51��Ƭ���Ĵ���1���ӵ�GSM��232�ӿ�
2.�ҵ�������ǰ���#define��������˵���޸ĺ��Լ��ĵ�Ƭ������.
3.ʹ�����������ҳhttp://www.ip138.com/����ѯ�Լ�������ip��ַ��
4.�ڹ������ҵ� ���Թ���\socket tool.exe �򿪣�Э�����ͣ�tcp��������ip��ַ���ض˿�Ĭ�ϡ����������
5.���س���
6.����ģ��,�ȴ��źŵ���˸���������ģ����ֻ����������������������յ�ģ�鷢������Ϣ

*************************************************************/
#include <REG51.H>
#include <string.h>

#define uchar unsigned char
#define uint unsigned int

//���������51��Ƭ���ľ����С
#define FOSC_110592M
//#define FOSC_12M


//�������ڱ��浥Ƭ���յ�ģ�鷢����ATָ�ͨ����Щָ�Ƭ�������ж�ģ���״̬
uchar GsmRcv[30] = {0};
uchar GsmRcvAt[30] = {0}; 
uchar GsmRccAt[30] = {0}; 
uchar GsmRcvCnt = 0;   //�����ַ�����
uchar GsmAtFlag = 0;   //ǰ���ж��Ƿ����վ֮���з���
uchar GsmJiFlag = 0;   //��������ǰ��ν�����������ν�������
uchar GsmQiFlag = 0;   //������ʶ�ӵ�ǰ�ַ���ʼ��ȡ
uchar GsmLiFlag = 0;   //��������֮��ʼ�������ݽ���
uchar CanRead = 0;
uchar count = 0;
static uchar second = 0;   //����
unsigned int tcount;
   
//ע�⣬���۽��յ��źŻ��Ƿ������źţ�������жϷ�������
/*��ʼ�����򣨱���ʹ�ã������޷��շ������γ��򽫻�ʹ�ö�ʱ��1*/
void SerialInti()//��ʼ�����򣨱���ʹ�ã������޷��շ���
{
	TMOD=0x22;//��ʱ��1����ģʽ2:8λ�Զ����ض�ʱ��

#ifdef FOSC_12M		   //��������ݾ����С���ò�ͬ����ֵ��ʼ������
	TH1=0xf3;//װ���ֵ��������2400
	TL1=0xf3;	
#else 	
	TH1=0xfd;//װ���ֵ��������9600
	TL1=0xfd;
#endif //end of SOC_12M

	TH0=0x06;
    TL0=0x06;
	
	TR1=1;//�򿪶�ʱ��
	TR0=1;
	SM0=0;//���ô���ͨѶ����ģʽ����10Ϊһ�����ͣ������ʿɱ䣬�ɶ�ʱ��1������ʿ��ƣ�
	SM1=1;//(ͬ��)�ڴ�ģʽ�£���ʱ�����һ�ξͷ���һ��λ������
	REN=1;//���н�������λ��Ҫ������sm0sm1�ٿ���������
	ET0=1;
	EA=1;//�����ж�
	ES=1;//�����п��ж�	
}

void Uart1Send(uchar c)
{
	SBUF=c;
	while(!TI);//�ȴ���������źţ�TI=1������
	TI=0;	
}

//���п���������char�����飬������ֹ��/0��ֹͣ
void Uart1Sends(uchar *str)
{
	while(*str!='\0')
	{
		SBUF=*str;
		while(!TI);//�ȴ���������źţ�TI=1������
		TI=0;
		str++;
	}
}

//��ʱ���������1s�ӣ�������ʱ��Ļ���׼...
void DelaySec(int sec)
{
	uint i , j= 0;

	for(i=0; i<sec; i++)
	{
		for(j=0; j<65535; j++)
		{	
		}
	}
}

void t0(void) interrupt 1 using 0 
{	
	tcount++;

    if(tcount==4000)
	{
		tcount=0;
		if (GsmLiFlag == 1) 
		{
			second++;
			if(second == 1)
			{
				Uart1Sends("AT+CIPSEND\r\n");
			} 
			else if (second == 4) 
			{
				Uart1Sends("abc123\r\n");	//��ʱ�����Զ������ݣ�����������֮�ࣩ
			}
			else if (second == 5) 
			{
				Uart1Send(0x1a);//��0x1a����
			}
			else if (second == 8) 
			{
				second = 0;
			}
		}
	}
}

/*����ͨѶ�жϣ��շ���ɽ�������ж�*/
void Serial_interrupt() interrupt 4 
{
	uchar i = 0;

	if(RI == 1)	//�յ���Ϣ
	{

		RI=0;//�����ж��ź����㣬��ʾ����������

		if (SBUF == '[') 
		{
			GsmQiFlag = 1;
		}

		if (GsmJiFlag == 0) 
		{
			GsmRcv[GsmRcvCnt] = SBUF;
			GsmRcvCnt++;
			//�յ���������ATָ�������ATָ������0x0a 0x0d��β�ġ��������жϣ��ڽ��յĹ������Ƿ��յ�0x0a 0x0d
			if( GsmRcvCnt >= 2 && (GsmRcv[GsmRcvCnt-2] == 0x0d && GsmRcv[GsmRcvCnt-1] == 0x0a) )
			{
				//һ���յ�0x0a 0x0d���ͽ����ݱ����������û����������жϡ�			
				for(i=0; i<GsmRcvCnt; i++)
				{
					GsmRcvAt[i] = GsmRcv[i];				 
					GsmRcv[i] = 0;	
				}
				GsmRcvCnt = 0;
				GsmAtFlag = 1;//�յ���������atָ�ͨ�������־λ��1��������������֪��ȥ�ж��ˡ�
			}
			else if(GsmRcvCnt >= 30)//��Ϊ�ڴ����ޣ��յ���50���ַ�����û�п���0x0a 0x0d�Ļ��������¿�ʼ���հɡ�
			{
				GsmRcvCnt = 0;
			}
		} 
		else
		{
			if (GsmQiFlag == 1) 
			{
				 GsmRcv[GsmRcvCnt] = SBUF;
				 GsmRcvCnt++;
				 if(GsmRcvCnt >= 2 && (GsmRcv[GsmRcvCnt-2] == '#' ||  GsmRcv[GsmRcvCnt-1] == '#'))
				 {
					for(i=0; i<GsmRcvCnt; i++)
					{
						GsmRccAt[i] = GsmRcv[i];
						GsmRcv[i] = 0;	
					}
					GsmRcvCnt = 0;
					GsmQiFlag = 0;
					CanRead = 1;	 //���տ���ָ�������������
				}
				else if(GsmRcvCnt >= 30)//��Ϊ�ڴ����ޣ��յ���50���ַ�����û�п���0x0a 0x0d�Ļ��������¿�ʼ���հɡ�
				{
					GsmRcvCnt = 0;
				}
			}
		}
		
	}
}



void main()
{
	uchar i = 0;
	SerialInti();

//�ж��Ƿ��������
	GsmAtFlag = 0;
	while(GsmAtFlag == 0)
	{
		Uart1Sends("ATI\r\n");//����sim300������
		DelaySec(1);//��ʱ1��
	}
	GsmAtFlag = 0;

	//����ź�
	while(1)
	{
		Uart1Sends("AT+COPS?\r\n");
		DelaySec(2);//��ʱ1��
		while(GsmAtFlag == 0);
		if(strstr(GsmRcvAt, "CHN-UNICOM") )//����Ƿ��յ� CHINA MOBILE ��������Ϣ������յ�֤���������������ˣ��й��ƶ���CHINA;�й���ͨ��CHN-UNICOM
		{
			Uart1Sends("AT+CIPCLOSE\r\n");	//���ڹر�TCP/UDP����
			DelaySec(1);
			Uart1Sends("AT+CIPRXGET=0\r\n");	 //�����Զ���������
			DelaySec(1);
			Uart1Sends("AT+CLPORT=\"TCP\",\"2022\"\r\n");//����ָ��ָ�����ض˿�
			DelaySec(1);
			Uart1Sends("AT+CIPSTART=\"TCP\",\"120.27.96.78\",\"8888\"\r\n");//�˴��޸��㽨����������IP���������˿ں�8080
			DelaySec(1);
			GsmJiFlag = 1;
			GsmLiFlag = 1;
			break;
		}		
	}

	while(1)
	{	
		 if (CanRead == 1) 	   //ʵʱ��Ӧ���յ��Ŀ���ָ��
		 {
			  CanRead = 0;
		 	  P1 = 0x00;
			  DelaySec(1);
			  P1 = 0xff;
		 }
	}
}
