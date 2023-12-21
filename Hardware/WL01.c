#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void WL01_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	GPIO_InitTypeDef WL01_AO;
	WL01_AO.GPIO_Mode=GPIO_Mode_AIN;
	WL01_AO.GPIO_Pin=GPIO_Pin_0;
	WL01_AO.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&WL01_AO);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	
	ADC_InitTypeDef WL01_AO_ADC;
	WL01_AO_ADC.ADC_ContinuousConvMode=DISABLE;
	WL01_AO_ADC.ADC_DataAlign=ADC_DataAlign_Right;
	WL01_AO_ADC.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	WL01_AO_ADC.ADC_Mode=ADC_Mode_Independent;
	WL01_AO_ADC.ADC_NbrOfChannel=1;
	WL01_AO_ADC.ADC_ScanConvMode=DISABLE;
	ADC_Init(ADC1,&WL01_AO_ADC);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);
	ADC_StartCalibration(ADC1);
	
	while(ADC_GetCalibrationStatus(ADC1)==SET);
}

uint16_t WL01_GetValue(void){
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
	
	return ADC_GetConversionValue(ADC1);
}
