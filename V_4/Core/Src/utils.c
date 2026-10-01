#include "utils.h"
#include "User_GPIO.h"

void Clodk_set(void){

	RCC->CR |= (1<<16);
	while(1){
		if((RCC->CR & (1 << 17))){
			break;
		}
	}

	RCC->CFGR |= (1<<16);


	FLASH->ACR |= 0x1;

	RCC->CR |= (1<<24);
	while(1){
		if ((RCC->CR & (1 << 25))) {
			break;
		}
	}

	RCC->CFGR |= 0x2;

	while(1){
		if ((RCC->CFGR & 0x0000000c) == 0x08) {
			break;
		}
	}

	SysTick->CTRL |= (1<<2)| (1);
	SysTick->LOAD = 0x7CFF;
}


void Uart1_set(void){
	RCC->APB2ENR |= (1<<14);
	RCC->APB2ENR |= 0x04;


	GPIOA->CRH &= ~(0xff0);
	GPIOA->CRH |= (0x8a<<4);

	USART1->CR1 |= (1<<13)|(1<<3)|(1<<2);
	USART1->BRR = 0x116;

}

int __io_putchar(int ch) {

	uint8_t count_ms = 0;

	while (!(USART1->SR & (1 << 7))){
		if(SysTick->CTRL & (1 << 16)) ++count_ms;
		if(count_ms >= 10) return -1;
	}

	USART1->DR = ch;
	return ch;
}

void Timer2_set(void){

	RCC->APB1ENR |= 0x01;
	TIM2->ARR = 0x3E7;
	TIM2->PSC = 0x7CFF;
	TIM2->DIER |= 0x01;
	NVIC->ISER[0] = (1<<28);
	NVIC->IP[28] = (0x40);
	TIM2->CR1 = 0x01;
}

void TIM2_IRQHandler_User(void){
	if(TIM2->SR & 0x01){
		time2_flag = 1;
		++time.ss;
		if (time.ss >= 60) {
			time.ss = 0;
			++time.mm;
		}
		TIM2->SR &= ~(0x01);
	}
}

void Timer3_set(void){
	RCC->APB1ENR |= 0x02;
	TIM3->ARR = 0xC7;
	TIM3->PSC = 0x9F;
	TIM3->DIER |= 0x01;
	NVIC->ISER[0] = (1<<29);
	NVIC->IP[29] = (0x60);
	TIM3->CR1 |= 0x01;
}

void TIM3_IRQHandler(void){
	if(TIM3->SR & 0x01){
		print_time(&time, rotation_new, ++position);
		if(position == 8) position = 0;
		TIM3->SR &= ~(0x01);
	}
}


void delay(int Time){
	uint32_t  time = Time;
	uint32_t  time_count=0;
	while(1){
		if(SysTick->CTRL & (1 << 16)) ++time_count;
		if(time <= time_count) return;
	}
}

void delay_us(uint32_t us){
    uint32_t start_val = SysTick->VAL;
    uint32_t load_val = SysTick->LOAD;
    uint32_t target_ticks = us * (SystemCoreClock / 1000000);

	while(1){
		int temp = (int)start_val - (int)SysTick->VAL;
		if(temp >= 0 && temp >= target_ticks) break;
		if(temp < 0 && (load_val+temp) >= target_ticks) break;
	}

}

void I2c2_set(void)
{

    RCC->APB2ENR |= (1 << 3);
    RCC->APB1ENR |= (1 << 22);

    I2C2->CR1 &= ~(1 << 0);

    GPIOB->CRH &= ~(0xFF00);
    GPIOB->CRH |=  (0x5500);
    GPIOB->BSRR = (1 << 10) | (1 << 11);

    while (!(GPIOB->IDR & (1 << 10)));
    while (!(GPIOB->IDR & (1 << 11)));

    GPIOB->BRR = (1 << 11);

    while (GPIOB->IDR & (1 << 11));

    GPIOB->BRR = (1 << 10);

    while (GPIOB->IDR & (1 << 10));


    GPIOB->BSRR = (1 << 10);

    while (!(GPIOB->IDR & (1 << 10)));

    GPIOB->BSRR = (1 << 11);

    while (!(GPIOB->IDR & (1 << 11)));

    GPIOB->CRH &= ~(0xFF00);
    GPIOB->CRH |=  (0xFF00);

    I2C2->CR1 |= (1 << 15);

    I2C2->CR1 &= ~(1 << 15);

    I2C2->CR2   = 32;
    I2C2->CCR   = 0x8035;
    I2C2->TRISE = 0x0A;
    I2C2->CR1 = (1 << 10) | (1 << 0);
}

int I2c2_start(void){

	uint32_t timeout = 100;
	I2C2->CR1 |= (1 << 8);
	while (!(I2C2->SR1 & 0x01)) {
		delay_us(1);
		if (--timeout == 0) {
			return 1;
		}
	}
	return 0;

}

void I2c2_stop(void){
	I2C2->CR1 |= (1<<9);
}

int I2c2_Slave_ReadorWrite(uint8_t add, bool RW){
	uint32_t timeout = 100;

	if(RW == 1){
		I2C2->DR = (add << 1)|0x01;
		while(!(I2C2->SR1 & 0x02)){
			delay_us(1);
			if (--timeout == 0) {
				return 1;
			}
		}
		(void)I2C2->SR2;
		return 0;
	}

	if(RW == 0){
		I2C2->DR = (add << 1);
		while(!(I2C2->SR1 & 0x02)){
			delay_us(1);
			if (--timeout == 0) {
				return 1;
			}
		}
		(void)I2C2->SR2;
		return 0;
	}
	return 1;
}

int I2c2_Trance(uint8_t data){
	uint32_t timeout = 100;

	I2C2->DR = data;
	while(!(I2C2->SR1 & 0x04)){
		delay_us(1);
		if (--timeout == 0) {
			return 1;
		}
	}
	return 0;
}

int I2c2_Read(){
	uint32_t timeout = 100;

	while(!(I2C2->SR1 & (1<<6))){
		delay_us(1);
		if (--timeout == 0) {
			return 1;
		}
	}
	return (uint8_t)I2C2->DR;
}

void Adc_set(void){
	RCC->APB2ENR |= (1<<9);
	RCC->CFGR |= (1<<14);
	GPIOA->CRL &= ~(0x0f000000);
	ADC1->SMPR2 |= (0x180000);
	ADC1->SQR3 |= (0x06 << 0);
	ADC1->CR1 |= (1<<11);
	ADC1->CR2 |= (0x07 << 17) | (1 << 20) | (0x01);
	delay(1);
	ADC1->CR2 |= (1 << 3);
	while (ADC1->CR2 & (1 << 3));
	ADC1->CR2 |= (1 << 2);
	while (ADC1->CR2 & (1 << 2));
}

uint16_t Adc_read(void){
	uint16_t data;
	ADC1->CR2 |= (1<<22);

	while(!(ADC1->SR & 0x02));
	data = (ADC1->DR & 0x0fff);
	return data;
}

uint8_t Get_Battery_Percentage(void){
    float vol = (((float)Adc_read()/4095.0f)*3.3)*2;

    if (vol >= 4.20f) return 100;
    if (vol <= 3.20f) return 0;

    if (vol > 4.05f) return 90 + (uint8_t)((vol - 4.05f) / (4.20f - 4.05f) * 10);
    if (vol > 3.90f) return 80 + (uint8_t)((vol - 3.90f) / (4.05f - 3.90f) * 10);
    if (vol > 3.80f) return 60 + (uint8_t)((vol - 3.80f) / (3.90f - 3.80f) * 20);
    if (vol > 3.70f) return 30 + (uint8_t)((vol - 3.70f) / (3.80f - 3.70f) * 30);
    if (vol > 3.50f) return 10 + (uint8_t)((vol - 3.50f) / (3.70f - 3.50f) * 20);

    return (uint8_t)((vol - 3.20f) / (3.50f - 3.20f) * 10);
}

int MPU6050_Init(void){
    if(I2c2_start() == 1)return 1;
    if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 0)== 1)return 1;
    if(I2c2_Trance(PWR_MGMT_1) == 1)return 1;
    if(I2c2_Trance(0x00) == 1)return 1;
    I2c2_stop();
    delay(10);
    if(MPU6050_1byte_write(PWR_MGMT_2, 0x07) == 1)return 1;

    return 0;
}


int MPU6050_Configure_MotionInt(void){
	if(MPU6050_1byte_write(PWR_MGMT_1, 0x00) == 1)return 1;
	if(MPU6050_1byte_write(PWR_MGMT_2, 0x00)== 1)return 1;
	if(MPU6050_1byte_write(ACCEL_CONFIG, 0x00)== 1)return 1;
	if(MPU6050_1byte_write(CONFIG, 0x00)== 1)return 1;
	if(MPU6050_1byte_write(INT_ENABLE, 0x40)== 1)return 1;
	if(MPU6050_1byte_write(MOT_DUR, 0x01)== 1)return 1;
	if(MPU6050_1byte_write(MOT_THR, 0x1E)== 1)return 1;
	delay(2);
	if(MPU6050_1byte_write(ACCEL_CONFIG, 0x07)== 1)return 1;
	if(MPU6050_1byte_write(PWR_MGMT_2, 0x87)== 1)return 1;
	if(MPU6050_1byte_write(PWR_MGMT_1, 0x20)== 1)return 1;
	if(MPU6050_1byte_write(INT_PIN_CFG, 0x20)== 1)return 1;
	return 0;
}

int MPU6050_1byte_write(uint32_t add, uint8_t data){
	if(I2c2_start() == 1) return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 0) == 1) return 1;
	if(I2c2_Trance(add) == 1)  return 1;
	if(I2c2_Trance(data) == 1) return 1;
	I2c2_stop();
	return 0;
}

int MPU6050_Recovery(void){

	if(MPU6050_1byte_write(PWR_MGMT_1, 0x80) == 1) return 1;
	delay(120);
	if(MPU6050_Init() == 1) return 2;
	delay(20);
	if(MPU6050_Configure_MotionInt() == 1) return 3;
	delay(20);
	if(MPU6050_Updata_xyz(&accel_xyz) == 1) return 4;
	delay(20);
	return 0;
}

uint8_t MPU6050_1byte_read(uint32_t add){
	uint8_t data;
	if(I2c2_start() == 1)return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 0) == 1)return 1;
	if(I2c2_Trance(add) == 1)return 1;
	if(I2c2_start() == 1)return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 1) == 1)return 1;
	I2C2->CR1 &= ~(1 << 10);
	I2c2_stop();
	data = I2c2_Read();
	I2C2->CR1 |= (1 << 10);

	return data;
}

int16_t MPU6050_Read_x(void){
	uint8_t data1, data2;
	if(I2c2_start() == 1)return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 0) == 1)return 1;
	if(I2c2_Trance(ACCEL_XOUT_H) == 1)return 1;
	if(I2c2_start() == 1)return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 1) == 1)return 1;
	data1 = I2c2_Read();
	I2C2->CR1 &= ~(1 << 10);
	I2c2_stop();
	data2 = I2c2_Read();
	I2C2->CR1 |= (1 << 10);
	return ((int16_t)data1 << 8 | data2);
}

int16_t MPU6050_Read_y(void){
	uint8_t data1, data2;
	if(I2c2_start() == 1)return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 0) == 1)return 1;
	if(I2c2_Trance(ACCEL_YOUT_H) == 1)return 1;
	if(I2c2_start() == 1)return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 1) == 1)return 1;
	data1 = I2c2_Read();
	I2C2->CR1 &= ~(1 << 10);
	I2c2_stop();
	data2 = I2c2_Read();
	I2C2->CR1 |= (1 << 10);
	return ((int16_t)data1 << 8 | data2);
}

int16_t MPU6050_Read_z(void){
	uint8_t data1, data2;
	if(I2c2_start() == 1)return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 0) == 1)return 1;
	if(I2c2_Trance(ACCEL_ZOUT_H) == 1)return 1;
	if(I2c2_start() == 1)return 1;
	if(I2c2_Slave_ReadorWrite(MPU6050_ADDR, 1) == 1)return 1;
	data1 = I2c2_Read();
	I2C2->CR1 &= ~(1 << 10);
	I2c2_stop();
	data2 = I2c2_Read();
	I2C2->CR1 |= (1 << 10);
	return ((int16_t)data1 << 8 | data2);
}

int MPU6050_Updata_xyz(Accel_xyz* accel_xyz){
	MPU6050_1byte_read(INT_STATUS);
	if(MPU6050_1byte_write(PWR_MGMT_1, 0x00) == 1)return 1;

	int temp_x = 0;
	int temp_y = 0;
	int temp_z = 0;

	for (int i = 0; i < 10; i++) {
		temp_x += MPU6050_Read_x();delay(1);
		temp_y += MPU6050_Read_y();delay(1);
		temp_z += MPU6050_Read_z();delay(1);
	}
	accel_xyz->x_avr = temp_x / 10;
	accel_xyz->y_avr = temp_y / 10;
	accel_xyz->z_avr = temp_z / 10;

	if(MPU6050_1byte_write(ACCEL_CONFIG, 0x00) == 1)return 1;
	delay(2);
	if(MPU6050_1byte_write(ACCEL_CONFIG, 0x07) == 1)return 1;
	if(MPU6050_1byte_write(PWR_MGMT_2, 0x87) == 1)return 1;
	if(MPU6050_1byte_write(PWR_MGMT_1, 0x20) == 1)return 1;
	return 0;
}


