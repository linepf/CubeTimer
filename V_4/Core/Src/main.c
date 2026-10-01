#include <main.h>
#include "stm32f1xx.h"
#include "utils.h"
#include "User_GPIO.h"
#include <stdio.h>
#include <stdbool.h>


uint8_t a; int vol=0;

Accel_xyz accel_xyz;
Time time;

uint8_t time2_flag = 0;
uint8_t time3_flag = 0;
uint8_t MPU6050_motion_flag_1 = 0;
uint8_t MPU6050_motion_flag_2 = 0;
uint8_t rotation_new = 0;
uint8_t rotation_old = 0;
uint8_t position = 0;
uint32_t motion_delay = 0;
uint8_t target_flag = 0;
uint8_t adc_flag = 0;
uint8_t buzzer_flag = 1;

int main(){

	Clodk_set();
	Uart1_set();
	I2c2_set();
	Timer2_set();
	Timer3_set();
	GPIO_set();
	Adc_set();


	printf("Start!! \n\r");
	delay(10);
	MPU6050_1byte_write(PWR_MGMT_1, 0x80);
	delay(110);
	MPU6050_Init();
	delay(10);

	accel_xyz.x_avr = 0;
	accel_xyz.y_avr = 0;
	accel_xyz.z_avr = 0;

	for (int i = 0; i < 10; i++) {
		accel_xyz.x_avr += MPU6050_Read_x();delay(1);
		accel_xyz.y_avr += MPU6050_Read_y();delay(1);
		accel_xyz.z_avr += MPU6050_Read_z();delay(1);
	}
	accel_xyz.x_avr = accel_xyz.x_avr / 10;
	accel_xyz.y_avr = accel_xyz.y_avr / 10;
	accel_xyz.z_avr = accel_xyz.z_avr / 10;
	Get_Current_Rotation(&time, accel_xyz.x_avr,accel_xyz.y_avr,accel_xyz.z_avr);
	printf("x: %5d | y: %5d | z: %5d | Rot: %d \r\n", accel_xyz.x_avr, accel_xyz.y_avr, accel_xyz.z_avr, rotation_new);

	MPU6050_Configure_MotionInt();

	while(1){


		if(adc_flag == 1){
			vol = Get_Battery_Percentage();
			if(buzzer_flag == 1){
				buzzer_flag = 0;
			}else{
				buzzer_flag = 1;
			}
			delay(600);
			adc_flag = 0;
		}


		if (time2_flag == 1) {

			if(time.mm >= time.target_mm && time.target_mm != 0 && target_flag != 1){
				time.target_ss = 0;
				time.mm = 0;
				time.ss = 0;
				target_flag = 1;
				Buzzer_Sound(buzzer_flag);
			}
			time2_flag = 0;
		}



		if(MPU6050_motion_flag_1 == 1){
			if(motion_delay <= 400){
				delay(1);
				++motion_delay;
			}else {
				motion_delay = 0;
				MPU6050_motion_flag_1 = 0;
				MPU6050_motion_flag_2 = 1;
			}
		}

		if (MPU6050_motion_flag_2 == 1) {

			int i2c_error_count = 0;
			while(1){

				if(MPU6050_Updata_xyz(&accel_xyz) == 0) break;
				else ++i2c_error_count;

				if(i2c_error_count >= 3){
					printf("i2c error\n\r");
					if(MPU6050_Recovery() == 0){
						printf("MPU6050 Recovery Success\n\r");
						break;
					}else{
						i2c_error_count = 2;
						printf("MPU6050 Recovery Failure\n\r");
					}
				}
			}


			Get_Current_Rotation(&time, accel_xyz.x_avr,accel_xyz.y_avr,accel_xyz.z_avr);
			printf("x: %5d | y: %5d | z: %5d | Rot: %d \r\n", accel_xyz.x_avr, accel_xyz.y_avr, accel_xyz.z_avr, rotation_new);
			MPU6050_motion_flag_2 = 0;

		}


		if (!adc_flag && !time2_flag && !MPU6050_motion_flag_1 && !MPU6050_motion_flag_2)
		__WFI();







	}
}


