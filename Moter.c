
#include "Moter.h"
#include "Delay.h"
#include "Led.h"

#define MOTER_SLEEP PC_ODR_ODR5
#define ResistanceEN PA_ODR_ODR2

void MoterInit(void) {
    //position
    PD_DDR_DDR3 = 0;
    PD_CR1_C13 = 0;
    PD_CR2_C23 = 0;
    //Encoder
    PA_DDR_DDR3 = 0;
    PA_CR1_C13 = 1;
    PA_CR2_C23 = 1;
    //Position en
    PA_DDR_DDR2 = 1;
    PA_CR1_C12 = 1;
    PA_CR2_C22 = 0;
    //moto in1 
    PC_DDR_DDR7 = 1;
    PC_CR1_C17 = 1;
    PC_CR2_C27 = 1;
    //moto in2
    PC_DDR_DDR6 = 1;
    PC_CR1_C16 = 1;
    PC_CR2_C26 = 1;
    //moto sleep
    PC_DDR_DDR5 = 1;
    PC_CR1_C15 = 1;
    PC_CR2_C25 = 0;
    //moto current
    PD_DDR_DDR2 = 0;
    PD_CR1_C12 = 0;
    PD_CR2_C22 = 0;
    //PWM INIT
    TIM1_EGR |= 0x01;   //���³�ʼ��TIM1 
	TIM1_CR1 = 0x00;   
	TIM1_RCR = 0x00;  
	TIM1_PSCRH =0;     //�趨Ԥ��ƵΪ,16��Ƶ 1M
	TIM1_PSCRL =0x80;  //PWM��ʱ�� Ӱ������
	
	TIM1_CCER1 = 0xFF; //CC2ER1����1,2,�ߵ�ƽ��Ч
	TIM1_CCER2 = 0xFF; //CC2ER1����1,2,�ߵ�ƽ��Ч
	
	TIM1_CCMR1 = 0x60; //PWMģʽ1,CC1���������
	TIM1_CCMR2 = 0x60; //PWMģʽ1,CC1���������
	
	TIM1_ARRH = 0;     //�趨��װ��ֵ
	TIM1_ARRL = 0xFF;  //PWM������ 
	
	TIM1_CCR1H = 0;
	TIM1_CCR1L = 0x00;  // ռ�ձ�ֵ
	
	TIM1_CCR2H = 0;
	TIM1_CCR2L = 0x00;  // ռ�ձ�ֵ
	
	TIM1_CR1 |= 0x01; //ʹ��TIM1������
	TIM1_BKR |= 0x80;
    
    ADC_CSR &= ~BIT(6);//��ģ�⹷
	ADC_CSR &= ~BIT(4);
    ADC_CR1 |= 0x70;
	ADC_CR1 |= BIT(1);//����ת��ģʽ
	ADC_CR2 |= BIT(1);//ʹ��ɨ��ģʽ
	ADC_TDRL = 0x01;
    
    EXTI_CR1 |= BIT(1);//PA INT
	EXTI_CR1 |= BIT(0); //0
    
    MOTER_SLEEP = 1;
    ResistanceEN = 0; //OPEN 
}

static u8 sleep_bit = 0;

void MoterSleep(void) {
    LedSet(1);
    MOTER_SLEEP = 0;
    ResistanceEN = 1;//clear
    sleep_bit = 1;
    DelayMs(100);
    MCUSLEEP
}

void MoterOpen(void) {
    if(sleep_bit == 1) {
        sleep_bit = 0;
        LedSet(0);
        MOTER_SLEEP = 1;
        ResistanceEN = 0;//clear
        DelayMs(900);
    }
}

u16 MoterGetAd(u8 poss) {
    u16 ad_value = 0;
    u8 ad_h = 0;
    u8 ad_l = 0;
	ADC_CSR = poss;
	ADC_CR1 = 0x01;
	DelayUs(30);
	ADC_CR1 = ADC_CR1 | 0x01; 
	while((ADC_CSR & 0x80) == 0); 
    ad_h = ADC_DRH;
    ad_l = ADC_DRL;
	ad_value = (ad_h<<8) + ad_l;
    return ad_value;
}

u16 MoterReadResistancePosition(void) {
    return MoterGetAd(4);
}
u16 MoterReadCurrent(void) {
    return MoterGetAd(3);
}

void MoterSpeed(u8 cmd, u8 speed) {
    switch(cmd) {
        case 0:
            MOTER_SLEEP = 0;//�Ƴ�˯��	
            TIM1_CCR1L = 0xff;//ռ�ձ�ֵ
            TIM1_CCR2L = 0xff;//ռ�ձ�ֵ
        break;
        case 1:
            MOTER_SLEEP = 1;//�Ƴ�˯��	
            TIM1_CCR1L = 0x00;//ռ�ձ�ֵ
            TIM1_CCR2L = speed;//ռ�ձ�ֵ
        break;
        case 2:
            MOTER_SLEEP = 1;//�Ƴ�˯��	
            TIM1_CCR1L = speed;  //ռ�ձ�ֵ
            TIM1_CCR2L = 0x00;//ռ�ձ�ֵ
        break;
        case 3:
            MOTER_SLEEP = 1;//�Ƴ�˯��	
            TIM1_CCR1L = 0x00;  //ռ�ձ�ֵ
            TIM1_CCR2L = 0x00;//ռ�ձ�ֵ
        break;
    }
}

static u16 coding_site = 0;

void MoterSetCodingSite(u8 cmd) {
    coding_site = cmd;
}

u16 MoterReadCodingSite(void) {
    return coding_site;
}

static u16 coding_sleep = 0;
void MoterSetSleep(u8 cmd) {
    coding_sleep = cmd;
}
u16 MoterGetSleep(void) {
    return coding_sleep;
}

#pragma vector=5
__interrupt void EXTI_PORTA_IRQHandler(void)
{
    INTOFF
    coding_site++;
    coding_sleep++;
    INTEN
}
