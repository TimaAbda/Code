//LCD module connections
sbit LCD_RS at Rd0_bit;
sbit LCD_EN at Rd1_bit;
sbit LCD_D4 at Rd2_bit;
sbit LCD_D5 at Rd3_bit;
sbit LCD_D6 at Rd4_bit;
sbit LCD_D7 at Rd5_bit;
sbit LCD_RS_Direction at TRISd0_bit;
sbit LCD_EN_Direction at TRISd1_bit;
sbit LCD_D4_Direction at TRISd2_bit;
sbit LCD_D5_Direction at TRISd3_bit;
sbit LCD_D6_Direction at TRISd4_bit;
sbit LCD_D7_Direction at TRISd5_bit;
// End LCD module connections

unsigned int k;
int temp;
unsigned char temp1;
char temper[7];
void CCPPWM_init(void);
unsigned char  myscaledVoltage;
unsigned char myspeed;
unsigned int READ_temp(void) ;
void data_converstion(void);
void motor(unsigned char);
void PWM(unsigned char);
unsigned int angle;
unsigned int Dcntr;
unsigned int Mcntr;
unsigned char cntr;
unsigned char HL;//High Low

void myDelay(unsigned int x);

void CCPPWM_init(void){ //Configure CCP1 at 2ms period with 50% duty cycle
  T2CON = 0x27;//enable Timer2 at Fosc/4 with 1:16 prescaler (8 uS percount 2000uS to count 250 counts)
  CCP2CON = 0x0C;//enable PWM for CCP2
  PR2 = 250;// 250 counts =8uS *250 =2ms period
  CCPR2c= 125;
}

void interrupt(void){

   if(INTCON&0x04){// will get here every 1ms   //timer0 interrupt
    TMR0=248;
    Mcntr++;
    Dcntr++;
    if(Dcntr==500){//after 500 ms
      Dcntr=0;
    }
  INTCON = INTCON & 0xFB; //clear T0IF
}
 if(PIR1&0x04){//CCP1 interrupt     //if bit3 on then make ccp1 int
   if(HL){ //high
     CCPR1H= angle >>8; //save the angle
     CCPR1L= angle;
     HL=0;//next time low
     CCP1CON=0x09;//next time Falling edge
     TMR1H=0;
     TMR1L=0;
   }
   else{  //low
     CCPR1H= (40000 - angle) >>8;
     CCPR1L= (40000 - angle);
     CCP1CON=0x08; //next time rising edge
     HL=1; //next time High
     TMR1H=0;
     TMR1L=0;

   }

 PIR1=PIR1&0xFB;
 }

unsigned int READ_temp(void)
{
temp = ADC_Read(0);
temp = ((temp * 5)/1023);          //from 0 to 1023
return temp;                                                                                                                                                                       temp = temp * 100;

}

void display1(void)
{
lcd_out(1,1,"TEMPERATURE=");
lcd_out(2,1, Ltrim(temper));
Lcd_Chr_Cp(0xdf);
Lcd_Chr_Cp('C');
Lcd_Chr_Cp(' ');

}

void data_converstion(void)
{
inttostr(temp,temper);
}
void motor(unsigned char s){
     //speed 0-250
     CCPR1Ldc=s;
}

void PWM(unsigned char k){

//case 0:
if(0<k<123){
            motor(0); // 0% motor speed.
}

//case 1:
if(124<k<255){

      motor(0); // 0% motor speed.
      angle=1200;
      if (temper != temper){
      angle=3500;
      delay_ms(2000); //
                          }
      PORTC = PORTC |  0x08; //RC3=1 window open
}

//case 2:
if(256<k<511)
             motor(63); // 25% motor speed.

//case 3:
if(512<k<767)
             motor(125); // 50% motor speed.

//case 4:
if(768<k<1022)
              motor((187)); //75% motor speed.

// case 5:
if(k=1023)
             motor(255); //100% motor speed.
}

void main() {
   //  set the directions
  TRISA = 0x01;  //  RA1 INPUT (TEMP)
  TRISB = 0x06;  //  RB2 input (motion sensor)
  TRISC =0x00;    //output
  PORTC=0x00;

  TMR1H=0;       //start count from 0
  TMR1L=0;      //start count from 0
  TMR0=248;     // 0-255
  HL=1; //start high   keep track from high and low

  CCPPWM_init();
  CCP1CON=0x08; //      set the output

  OPTION_REG = 0x87;//Fosc/4 with 256 prescaler => incremetn every 0.5us*256=128us ==> overflow 8count*128us=1ms to overflow
  T1CON=0x01;//TMR1 On Fosc/4 (inc 0.5uS) with 0 prescaler (TMR1 overflow after 0xFFFF counts ==65535)==> 32.767ms

  INTCON=0xF0;//enable TMR0 overflow, TMR1 overflow, External interrupts and peripheral interrupts;

  PIE1=PIE1|0x04;// Enable CCP2 interrupts   on each match interrupt
  CCPR1H=20000>>8;                     //higher 8 bit
  CCPR1L=20000;                        //lower 8 bit       10msecond

  lcd_Init();    //lcd init
  lcd_Cmd(_LCD_CLEAR);
  lcd_cmd(_LCD_CURSOR_OFF);

  while(1){
  if(PORTB == 0x04)  //motion detected
 {
       if(PORTB & 0x02){   // detecting dark
           PORTC =  0x60; // LEDs ON
           }

        else {
           PORTC =  0x00;// LEds OFF
           }

          temp1 = READ_temp(void);
          data_converstion();
          display1();
          pwm(temp1);

  }

  }
  }