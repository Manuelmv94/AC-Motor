#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

unsigned char j=0;
unsigned int periodo;
unsigned int t1;
unsigned char adc_value;
unsigned int delay;
unsigned int t_ki;
unsigned int t_entre_periodos;//us
interrupt VectorNumber_Vkeyboard  void 	KBI_interrupt (void) 
{
	KBISC_KBACK=1;
	t_entre_periodos=TPMCNT-t_ki;
	//t_entre_periodos*32 lo que tarda en una vuelta, luego 60/ans
	t_ki=TPMCNT;
	
	
}

interrupt 5 void TPMC0_ISR (void) 
{
	(void) TPMC0SC;
    TPMC0SC_CH0F=0;
    if(j==0)
    {
    	PTBD_PTBD6=1;
    	TPMC0V=TPMCNT+30;
    	j++;
    }
    else
    {
        PTBD_PTBD6=0;
        TPMC0SC=0x14;//apagas interrupciones
        j=0;
    }
}

interrupt 6 void CH1_ISR (void)
{
	(void) TPMC1SC;
	TPMC1SC_CH1F=0;
	//al detectar el cruce por cero, empezar a contar X  tiempo para prender el pwm
	PTBD_PTBD6=0;
	t1=TPMC1V-t1;	//tenemos diferencial de tiempo
	periodo=t1;
	t1=TPMC1V;
	//velocidad
	TPMC0SC=0x54;//
	TPMC0V=TPMCNT+15+delay;
	
	
}

//VELOCIDAD MAXIMA 400 US
// VELOCIDAD MINIMA 5200 US



void main(void) {
  EnableInterrupts;
  /* include your code here */
  t_ki=0;
  PTBPE_PTBPE1=1;
  KBISC&=0xFD;// clear interrupt enable
  KBIES=0x00; // set rising edge
  PTBPE_PTBPE1=1; // pull up resistor
  KBIPE=0b00100000; //select pin to enable key interrupt
  KBISC_KBACK=1;//clear false interrupt
  KBISC_KBIE=1;
  t_entre_periodos=0;
  ADCSC1=0x21;
  PTBD_PTBD6=0;
  PTBDD_PTBDD6=1;
  TPMSC=0x0F;
  TPMC1SC=0x44;
  j=0;

  for(;;) {
	  __RESET_WATCHDOG();	/* feeds the dog */
	  if(ADCSC1_COCO==1)
	  {
		  //adc_value=ADCRL;
		  adc_value=0;
		  adc_value>>=1; 
		  delay=adc_value;
	  }
	  //saber velocidad
	  //delay*2350/8300
	  //3168 1220 revs
	  //44321  1353
	  //2194 1798
  } /* loop forever */
  /* please make sure that you never leave main */
}
