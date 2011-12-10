/* 
Due to certain issues in the designing of the sensor board you need to connect your sensors in the following way :
1st pin of sensorboard to PORTA,PIN1
2nd pin --> PORTA,PIN2
3rd pin --> PORTA,PIN0
4th pin --> PORTA,PIN3
Under normal circumstances you'd have connected the sensors to PORTA 0-3 in order.
We apologize for the inconvenience.
*/

#include<avr/io.h>
#include<stdlib.h>
#include <compat/deprecated.h>
#include <util/delay.h>

unsigned char min[7],max[7],threshold[7],sensorbyte=0,ps=0,u=0,flag=1,flag1=1,flag2=1,flag3=1;
long int mpos=0,control=0,i,node=0,a,b,pa,pb,c,d,pc,pd,stage=0,temp=0,countl=0,countr=0,count=0,m,n,r=1,s=1,t=1;

void leftforward()
{
 sbi(PORTC,4);
 cbi(PORTC,5);
 cbi(PORTC,6);
 cbi(PORTC,7);
}
void rightforward()
{
 cbi(PORTC,4);
 cbi(PORTC,5);
 sbi(PORTC,6);
 cbi(PORTC,7);
 }

void botforward()
{
 sbi(PORTC,4);
 cbi(PORTC,5);
 sbi(PORTC,6);
 cbi(PORTC,7);
}

void botstop()
{
 cbi(PORTC,4);
 cbi(PORTC,5);
 cbi(PORTC,6);
 cbi(PORTC,7);
}

void port_init(void)
{
 PORTA = 0xFF;
 DDRA  = 0x00;
 PORTB = 0xFF;  
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xFF;
 PORTD = 0xFF;
 DDRD  = 0x00;
}


void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0x00; //setup
 TCNT1L = 0x00;
 OCR1AH = 0x00;
 OCR1AL = 0x00;
 OCR1BH = 0x00;
 OCR1BL = 0x00;
 ICR1H  = 0x01;
 ICR1L  = 0x8F;
 TCCR1A = 0xA2;
 TCCR1B = 0x19; //start Timer
}


void adc_init(void)
{
 ADCSRA=0X00;
 ADMUX=0X60;//0x40 for 10 bits
 ADCSRA=0X87;
 ACSR=0X80;
}

unsigned char adc_start(unsigned char channel)
{
 unsigned char i;
 
     ADCH=0x00;

	 i=channel&0x07;
	 ADMUX=i|0x60;                //i|0x40 for 10 bits
	 ADCSRA|=1<<ADSC;
	   
		 while(ADCSRA & (1<<ADSC));       // wait for conv. to complete
		    unsigned char temp=ADCH;      //unsigned int temp=ADC;   for 10 bits
   
 return temp;
}


void delay(int x)
{
 unsigned char i,j;
 for(i=0;i<x;i++)
  for(j=0;j<4;j++)
   _delay_ms(250);
}


void checksensors(void)
{
sensorbyte=0;

unsigned char i,temp[8];

	 for(i=0;i<8;i++)
	 {
	 
	  temp[i]=adc_start(i);
	  if(temp[i]<threshold[i])
	  sensorbyte|=(1<<i);
	 
	 }
	 
}
 
 
 void calibrateblack(void)
{
	unsigned char j,i,temp[8];

	for(j=0;j<8;j++) 
	 {
		  max[j]=adc_start(j);
		  
		  for(i=0;i<10;i++)
		 {
			  temp[j]=adc_start(j);
			  
			  if(temp[j]<max[j])
			  {
			  max[j]=temp[j];
			  }
			  
		 }
	 
	}

}

void calibratewhite(void)
{

	unsigned char j,i,temp[8];
	 
	 for(j=0;j<8;j++) 
	 {
		  min[j]=adc_start(j);
		  
		  for(i=0;i<10;i++)
		 {
			  temp[j]=adc_start(j);
			  
			 if(temp[j]<min[j])
			  {
			  min[j]=temp[j];
			  }
		  
		 }
	 
	}

}


void setthreshold(void)
{

	unsigned char i;
	char diff;
	
	 for(i=0;i<8;i++)
	 {
	 
		 diff=max[i]-min[i];
		 threshold[i]=min[i]+(diff>>1);
		  
	 }
	
}

void flick (void)
{
unsigned int i=0;

	for(i=0;i<5;i++)
	{
		PORTC=0xff;
		_delay_ms(100);
		PORTC=0x00;
		_delay_ms(100);
	}

}


void line_track(void)
{

checksensors();

switch(sensorbyte)
{
case 0b00000110:botforward();
				 break;

case 0b00000011:OCR1A=340;
				leftforward(); 
                  break;

case 0b00001100:OCR1B=340;
				rightforward(); 
                 break;

case 0b00000001: OCR1A=240;
				 leftforward();
                  break;
case 0b00001000: OCR1B=240;
                 rightforward();
                  break;
case 0b00000111:OCR1A=200;
				leftforward(); 
                  break;
case 0b00001110:OCR1B=200;
				rightforward(); 
                  break;
//case 0b00000111:rightforward(); 
 //                 break;
//case 0b00000110:botforward(); 
 //                 break;
//case 0b00001100:botforward(); 
  //                break;
case 0b00000000:botstop(); 
                  break;
case 0b00001111:botstop(); 
                  break;
				  	      
default :botforward(); break;
}
}


 
void init_devices(void)
{
 timer1_init();
 port_init();
 adc_init();
}

void main(void)
{
init_devices();
while(1)
{

       if(bit_is_clear(PIND,0))              //pressing s1 blink1portpin will execute
		{
		calibratewhite();
		
		setthreshold();
		
		flick();
		}
		
		if(bit_is_clear(PIND,1))              //pressing s2 pattern1 will execute
		{
		calibrateblack();
		
		setthreshold();
		
		flick();
		}
		
		if(bit_is_clear(PIND,2))              //function to check the sensorbyte press s3
		{
		checksensors();
		
		PORTC=sensorbyte;
		
		}
		if(bit_is_clear(PIND,3))              //function to linetrack press s4
		{ 
		
		while(1)
		{
		
		line_track();
		}
		}
		}}
