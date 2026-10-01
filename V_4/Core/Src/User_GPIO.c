#include "User_GPIO.h"

uint8_t segmentArr_f[15]={
  			0xFC,	// 0
			0x60,	// 1
			0xDA,	// 2
			0xF2,	// 3
			0x66,	// 4
			0xB6,	// 5
			0xBE,	// 6
			0xE0,	// 7
			0xFE,	// 8
			0xF6,	// 9
			0x01,	// dp
			0xB6,	// s
			0x1E,	// t
			0x3A,	// o
			0xCE,	// p
};

uint8_t segmentArr_r[15]={
  			0xFC,	// 0
			0x0C,	// 1
			0xDA,	// 2
			0x9E,	// 3
			0x2E,	// 4
			0xB6,	// 5
			0xF6,	// 6
			0x1C,	// 7
			0xFE,	// 8
			0xBE,	// 9
			0x01,	// dp
			0xB6,	// s
			0xE2,	// t
			0xC6,	// o
			0x7A,	// p
};

void GPIO_set(void){

	RCC->APB2ENR |= (1<<2)|(1<<3);

	GPIOA->CRL &= ~(0xF00FFF00);
	GPIOA->CRL |=  (0x20082800);
	GPIOA->ODR |=  (1<<4);

	GPIOB->CRL &= ~(0x000000FF);
	GPIOB->CRL |=  (0x00000022);

	RCC->APB2ENR |= (0x01);
	AFIO->EXTICR[1] &= ~(0x0F);
	EXTI->IMR |= (1<<4);
	EXTI->RTSR|= (1<<4);

	NVIC->ISER[0] |= (1<<10);

	AFIO->EXTICR[0] &= ~(0xF00);
	EXTI->IMR |= (1<<2);
	EXTI->RTSR|= (1<<2);

	NVIC->ISER[0] |= (1<<8);
	NVIC->IP[8]  |= (0x30);

}


void EXTI2_IRQHandler(){
	if(EXTI->PR & (1 << 2)){
		MPU6050_motion_flag_1 = 1;
		EXTI->PR = (1 << 2);
	}
}


void EXTI4_IRQHandler(){
	if(EXTI->PR & (1 << 4)){
		adc_flag = 1;
		EXTI->PR = (1 << 4);
	}
}


void GPIO_RCK(uint8_t data){
	if(data == 1)
		GPIOB->BSRR = (1<<0);
	if(data == 0)
		GPIOB->BSRR = (1<<16);
}


void GPIO_SER(uint8_t data){
	if(data == 1)
		GPIOB->BSRR = (1<<1);
	if(data == 0)
		GPIOB->BSRR = (1<<17);
}


void GPIO_SCK(uint8_t data){
	if(data == 1)
		GPIOA->BSRR = (1<<7);
	if(data == 0)
		GPIOA->BSRR = (1<<23);
}


void GPIO_BUZZER(uint8_t data){
	if(data == 1)
		GPIOA->BSRR = (1<<3);
	if(data == 0)
		GPIOA->BSRR = (1<<19);
}

void Buzzer_Sound(uint8_t data){
	if (data == 0) return;
	else {
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				GPIO_BUZZER(1);
				delay(60);
				GPIO_BUZZER(0);
				delay(60);
			}
			delay(500);
		}
	}
}

void Put_16bit(uint16_t data){
	GPIO_RCK(0);
	GPIO_SCK(0);

	for(int i=0; i<=15; i++){
		GPIO_SER(data>>i & 0x01);
		GPIO_SCK(1);
		GPIO_SCK(0);
	}


	GPIO_RCK(1);
}

void print_time(const Time* time, uint8_t rotation, uint8_t position){

	if(adc_flag == 1){
		uint8_t vol = Get_Battery_Percentage();
		uint8_t vol_1 = vol%10;
		uint8_t vol_10 = (vol / 10)%10;
		uint8_t vol_100 = vol / 100;
		if(rotation == 1){
			if(position == 1)
				Put_16bit((segmentArr_f[0]<<8)|(0x80));
			if(2 <= position && position <= 4)
				Put_16bit((segmentArr_f[vol_100]<<8)|(0x40));
			if(5 <= position && position <= 7)
				Put_16bit((segmentArr_f[vol_10]<<8)|(0x20));
			if(position == 8)
				Put_16bit((segmentArr_f[vol_1]<<8)|(0x10));
		}
		if(rotation == 2){
			if(position == 1 || position == 5)
				Put_16bit((segmentArr_r[0]<<8)|(0x08));
			if(position == 2 || position == 6)
				Put_16bit((segmentArr_r[vol_100]<<8)|(0x04));
			if(position == 3 || position == 7)
				Put_16bit((segmentArr_r[vol_10]<<8)|(0x02));
			if(position == 4 || position == 8)
				Put_16bit((segmentArr_r[vol_1]<<8)|(0x01));
		}
		if(rotation == 4){
			if(position == 1 || position == 5)
				Put_16bit((segmentArr_f[0]<<8)|(0x01));
			if(position == 2 || position == 6)
				Put_16bit((segmentArr_f[vol_100]<<8)|(0x02));
			if(position == 3 || position == 7)
				Put_16bit((segmentArr_f[vol_10]<<8)|(0x04));
			if(position == 4 || position == 8)
				Put_16bit((segmentArr_f[vol_1]<<8)|(0x08));
		}
		if(rotation == 3){
			if(position == 1)
				Put_16bit((segmentArr_r[0]<<8)|(0x10));
			if(2 <= position && position <= 4)
				Put_16bit((segmentArr_r[vol_100]<<8)|(0x20));
			if(5 <= position && position <= 7)
				Put_16bit((segmentArr_r[vol_10]<<8)|(0x40));
			if(position == 8)
				Put_16bit((segmentArr_r[vol_1]<<8)|(0x80));
		}
		if(rotation == 0){
			if(position == 1)
				Put_16bit((segmentArr_r[0]<<8)|(0x10));
			if(2 <= position && position <= 4)
				Put_16bit((segmentArr_r[vol_100]<<8)|(0x20));
			if(5 <= position && position <= 7)
				Put_16bit((segmentArr_r[vol_10]<<8)|(0x40));
			if(position == 8)
				Put_16bit((segmentArr_r[vol_1]<<8)|(0x80));
		}
		return ;
	}


	if(target_flag == 1){
		if(rotation == 1){
			if(position == 1)
				Put_16bit((segmentArr_f[11]<<8)|(0x80));
			if(2 <= position && position <= 4)
				Put_16bit((segmentArr_f[12]<<8)|(0x40));
			if(5 <= position && position <= 7)
				Put_16bit((segmentArr_f[0]<<8)|(0x20));
			if(position == 8)
				Put_16bit((segmentArr_f[14]<<8)|(0x10));
		}
		if(rotation == 2){
			if(position == 1 || position == 5)
				Put_16bit((segmentArr_r[11]<<8)|(0x08));
			if(position == 2 || position == 6)
				Put_16bit((segmentArr_r[12]<<8)|(0x04));
			if(position == 3 || position == 7)
				Put_16bit((segmentArr_r[0]<<8)|(0x02));
			if(position == 4 || position == 8)
				Put_16bit((segmentArr_r[14]<<8)|(0x01));
		}
		if(rotation == 4){
			if(position == 1 || position == 5)
				Put_16bit((segmentArr_f[11]<<8)|(0x01));
			if(position == 2 || position == 6)
				Put_16bit((segmentArr_f[12]<<8)|(0x02));
			if(position == 3 || position == 7)
				Put_16bit((segmentArr_f[0]<<8)|(0x04));
			if(position == 4 || position == 8)
				Put_16bit((segmentArr_f[14]<<8)|(0x08));
		}
		if(rotation == 3){
			if(position == 1)
				Put_16bit((segmentArr_r[11]<<8)|(0x10));
			if(2 <= position && position <= 4)
				Put_16bit((segmentArr_r[12]<<8)|(0x20));
			if(5 <= position && position <= 7)
				Put_16bit((segmentArr_r[0]<<8)|(0x40));
			if(position == 8)
				Put_16bit((segmentArr_r[14]<<8)|(0x80));
		}
		if(rotation == 0){
			if(position == 1)
				Put_16bit((segmentArr_r[10]<<8)|(0x10));
			if(2 <= position && position <= 4)
				Put_16bit((segmentArr_r[10]<<8)|(0x20));
			if(5 <= position && position <= 7)
				Put_16bit((segmentArr_r[10]<<8)|(0x40));
			if(position == 8)
				Put_16bit((segmentArr_r[10]<<8)|(0x80));
		}
		return;
	}

	uint8_t s0 = time->ss%10;
	uint8_t s1 = time->ss/10;
	uint8_t m0 = time->mm%10;
	uint8_t m1 = time->mm/10;


	if(rotation == 1){
		if(position == 1)
			Put_16bit((segmentArr_f[m1]<<8)|(0x80));
		if(2 <= position && position <= 4)
			Put_16bit((segmentArr_f[m0]<<8)|(0x40));
		if(5 <= position && position <= 7)
			Put_16bit((segmentArr_f[s1]<<8)|(0x20));
		if(position == 8)
			Put_16bit((segmentArr_f[s0]<<8)|(0x10));
	}
	if(rotation == 2){
		if(position == 1 || position == 5)
			Put_16bit((segmentArr_r[m1]<<8)|(0x08));
		if(position == 2 || position == 6)
			Put_16bit((segmentArr_r[m0]<<8)|(0x04));
		if(position == 3 || position == 7)
			Put_16bit((segmentArr_r[s1]<<8)|(0x02));
		if(position == 4 || position == 8)
			Put_16bit((segmentArr_r[s0]<<8)|(0x01));
	}
	if(rotation == 4){
		if(position == 1 || position == 5)
			Put_16bit((segmentArr_f[m1]<<8)|(0x01));
		if(position == 2 || position == 6)
			Put_16bit((segmentArr_f[m0]<<8)|(0x02));
		if(position == 3 || position == 7)
			Put_16bit((segmentArr_f[s1]<<8)|(0x04));
		if(position == 4 || position == 8)
			Put_16bit((segmentArr_f[s0]<<8)|(0x08));
	}
	if(rotation == 3){
		if(position == 1)
			Put_16bit((segmentArr_r[m1]<<8)|(0x10));
		if(2 <= position && position <= 4)
			Put_16bit((segmentArr_r[m0]<<8)|(0x20));
		if(5 <= position && position <= 7)
			Put_16bit((segmentArr_r[s1]<<8)|(0x40));
		if(position == 8)
			Put_16bit((segmentArr_r[s0]<<8)|(0x80));
	}
	if(rotation == 0){
		if(position == 1)
			Put_16bit((segmentArr_r[10]<<8)|(0x10));
		if(2 <= position && position <= 4)
			Put_16bit((segmentArr_r[10]<<8)|(0x20));
		if(5 <= position && position <= 7)
			Put_16bit((segmentArr_r[10]<<8)|(0x40));
		if(position == 8)
			Put_16bit((segmentArr_r[10]<<8)|(0x80));
	}

}

void Get_Current_Rotation(Time* time, int x, int y, int z){
	int hysteresis = 1000;
	if(z < -(16500 - hysteresis) && z > -(16500 + hysteresis)){
		rotation_new = 1;
	}

	if(x < -(15500 - hysteresis) && x > -(16500 + hysteresis)){
		rotation_new = 2;
	}
	if(z > 16500 - hysteresis && z < 16500 + hysteresis){
		rotation_new = 3;
	}
	if(x > 16500 - hysteresis && x < 16500 + hysteresis){
		rotation_new = 4;
	}
	if(y < -(16500 - hysteresis) && y > -(16500 + hysteresis)){
		rotation_new = 0;
	}
	if(y > 16500 - hysteresis && y < 16500 + hysteresis){
		rotation_new = 0;
	}

	if(rotation_new == rotation_old){

		return;
	}


	time->ss = time->mm = 0;

	rotation_old = rotation_new;
	target_flag = 0;

	switch(rotation_new){
	case 1: time->target_mm = 5;
		break;
	case 2: time->target_mm = 60;
		break;
	case 3: time->target_mm = 30;
		break;
	case 4: time->target_mm = 15;
		break;
	case 0: time->target_mm = 0;
		break;
	}
}



