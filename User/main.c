#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "WL01.h"
int almost(int num,int range){
	if((num<=(range+2))&&(num>=(range-2)))
		return 1;
	return 0;
}
int flag=1,sum2=0;
void BUTTON_INIT(void);
void EXTI15_10_IRQHandler(void);
int main(){
	WL01_Init();
	OLED_Init();
	BUTTON_INIT();
	OLED_Clear();
	double sum=0;
	int n=0,a=0,k=1660,minimum=10000,maximum=0,delta=0;
	OLED_ShowString(2,6,"dV");
	while(1){
		OLED_ShowNum(1,8,flag,2);
		OLED_ShowString(1,1,flag%2==1?"A MODE":"B MODE");
		if(n==300){
			n=0;
			sum/=300;
			sum2++;
			k=sum;
			
			OLED_ShowNum(4,7,sum2,2);
			if(k<=1700&&1600<=k&&(maximum-minimum<=60)) delta=k-1671;
			sum=0;
			k-=delta;
			OLED_ShowSignedNum(2,8,delta,4);
			OLED_ShowNum(4,1,k,4);
			if(maximum-minimum>=60) OLED_ShowString(3,1,"SCANNING    ");
			
			if(flag%2==1&&(maximum-minimum<=60)){
				if(k<=1710&&1600<=k) OLED_ShowString(3,1,"EMPTY       ");
				else if(k<=1400&&k>=1320) OLED_ShowString(3,1,"1 YUAN      ");
				else if(k<=1485&&k>1400) OLED_ShowString(3,1,"5 JIAO      ");
				else if(k<=1600&&k>1485) OLED_ShowString(3,1,"1 JIAO      ");
				else OLED_ShowString(3,1,"UNKNOWN    ");
			}else if(flag%2==0&&(maximum-minimum<=60)){
				if(k<=1710&&1600<=k) OLED_ShowString(3,1,"EMPTY       ");
				
				else if(k>=1300&&k<=1363) OLED_ShowString(3,1,"1YUAN>1YUAN");
				else if(k>=1491&&k<=1520) OLED_ShowString(3,1,"1JIAO>1JIAO");
				else if(k>=1401&&k<=1450) OLED_ShowString(3,1,"5JIAO>5JIAO");
				
				else if(k>=1364&&k<=1376) OLED_ShowString(3,1,"5JIAO>1YUAN");
				else if(k>=1377&&k<=1400) OLED_ShowString(3,1,"1YUAN>5JIAO");
				
				else if(k>=1451&&k<=1465) OLED_ShowString(3,1,"1JIAO>5JIAO");
				else if(k>=1466&&k<=1490) OLED_ShowString(3,1,"5JIAO>1JIAO");
				
				
				
				else OLED_ShowString(3,1,"UNKNOWN    ");
			}
			minimum=10000;maximum=0;
		}
		a=WL01_GetValue();
		//OLED_ShowString(1,1,"ADC Value");
		OLED_ShowNum(2,1,a,4);
		//Normally 1620<ADC<1670,which can be considered as empty condition.
		
		if(a>maximum) maximum=a;
		if(a<minimum) minimum=a;
		
		sum+=a;
		n++;
	}
	/*
	1元 1359 1362 1363 1355 1357 1356 1357 1358 1358 1358 1358 1361 1359 1359 （1358+-5）
	1角  1528  1528 1530 1528 1529 1529 1529 1529 1528 （1528+-5）
	5角  1454  1455  1455 1454 1456 1454 （1455+-3）
	
	A类取+-30保证参数稳定
	 
	1元+1元 1355 1355 1355 1356 1357 1358 1359 1357 1361 1360
	//上1角下1元 1363 1363 1364 1365 1366 1363
	//下1角上1元 1390 1391 1391 1388 1383 1384      1395 1396 1397 1398 1399 1398 1399 1393 1393
	1角+1角 1503 1504 1505 1505 1503 1504 
	
	上5角下1元 1375 1372 1373 1372 1373 1380 1381 1376
	下5角上1元 1391 1392 1393 1396 1391 1392 1393  
	5角+5角 1450 1449 1449 1447 1450 1451 
	
	统计认为下1角上1元下和5角上1元 的分界点在1393附近
	
	下5角上1角 1456 1457  1457 1459 1459 1460
	
	统计认为5角+5角和5角上1角 的分界点在1453左右
	
	下1角上5角 1465 1467 1467 1465 1463
	
	统计认为下1角上5角和5角上1角 在1461-1462左右

	*/
	
	
	
	
	
}
void BUTTON_INIT(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}
void EXTI15_10_IRQHandler(void){
	if (EXTI_GetITStatus(EXTI_Line14) == SET){
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0){
			//sum2++;
			sum2=0;
			flag++;
			Delay_ms(500);
		}
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}

