#include<stdint.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define SET_BIT(PORT,BIT) PORT|= (1<<BIT)
#define CLR_BIT(PORT,BIT) PORT&= ~(1<<BIT)
#define TOGGLE_BIT(PORT, BIT) PORT^=(1<<BIT)

struct
{
 volatile unsigned int FLAG_ENG:1;
}FLAG_BIT;

void pwm(int y)
{
  OCR0A=16;
  OCR0B=((16*y)/100);
  TCCR0A|=(1<<COM0A1)|(1<<COM0B1)|(1<<WGM01);
  TCCR0B|=(1<<CS00)|(1<<CS02);
  TIMSK0|=(1<<OCIE0B)|(1<<OCIE0A);
  sei();
}
  
int adc_init()
{
  ADMUX=0x00;
  ADCSRA|=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADIE);
  ADCSRA|=(1<<ADSC);
  while(ADCSRA & (1<<ADSC));  
  sei();
}  
void timer()
{
  OCR1A=15624;     // Timer is set for 1s
  TCCR1A=0;
  TCCR1B|=(1<<CS12)|(0<<CS11)|(1<<CS10); // prescalar value :1024
  TIMSK1|=(1<<OCIE1A)|(1<<TOIE1);   // Enable Output compareA interrupt 
  sei();
}
int main()
{
  Serial.begin(9600);
  DDRD|=(1<<PD5);
  DDRD&=~(1<<PD3);
  PORTD&=~(1<<PD5);
  EICRA|=(1<<ISC10);
  EICRA&=~(1<<ISC11);
  EIMSK|=(1<<INT1);//local interrupt enable
  sei();
  while(1)
  {
    if(FLAG_BIT.FLAG_ENG==0)
    {
      CLR_BIT(PORTD,PD5);
    }
  } 
}
ISR(INT1_vect)
{ cli();
 FLAG_BIT.FLAG_ENG=!FLAG_BIT.FLAG_ENG;
 timer();
 sei();
}
ISR(TIMER1_COMPA_vect)
{
  adc_init();
  TCNT1=0;
}
ISR(ADC_vect)
{
  Serial.println(ADC);
  if(ADC>=0 & ADC<200)  //less distance
  {
    Serial.println
    pwm(75);
  }
  else if(ADC>=200 & ADC<400) 
  {
    pwm(50);  
  }  
  else if(ADC>=400 & ADC<700)
  {
    pwm(25); 
  }
  else if(ADC>=700)
  {
    pwm(1);  
  }
}
ISR(TIMER0_COMPA_vect)
{
  PORTD=(1<<PD5);
}
ISR(TIMER0_COMPB_vect)
{
  PORTD=(0<<PD5);
}
ISR(TIMER1_OVF_vect)
{
  adc_init();
}

