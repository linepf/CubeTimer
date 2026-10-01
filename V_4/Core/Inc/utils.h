#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <main.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MPU6050_ADDR 0x68
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define PWR_MGMT_1   0x6B
#define PWR_MGMT_2   0x6C
#define ACCEL_CONFIG 0x1C
#define INT_ENABLE   0x38
#define MOT_DUR      0x20
#define MOT_THR		 0x1F
#define CONFIG 		 0x1A
#define INT_STATUS   0x3A
#define INT_PIN_CFG  0x37


typedef struct {

	int x_avr;
	int y_avr;
	int z_avr;
} Accel_xyz;


typedef struct {
	uint8_t ss;
	uint8_t mm;
	uint8_t target_ss;
	uint8_t target_mm;
} Time;

extern uint8_t time2_flag;
extern uint8_t time3_flag;
extern Time time;
extern uint8_t position;
extern uint8_t target_flag;
extern Accel_xyz accel_xyz;

void Clodk_set(void);
void Uart1_set(void);
void Timer2_set(void);
void Timer3_set(void);
void delay(int Time);
void delay_us(uint32_t us);

void I2c2_set(void);
int I2c2_start(void);
void I2c2_stop(void);
int I2c2_Slave_ReadorWrite(uint8_t add, bool RW);
int I2c2_Trance(uint8_t data);
int I2c2_Read();
void I2c2_ACK();
void I2c2_NACK();

int MPU6050_Init(void);
int MPU6050_1byte_write(uint32_t add, uint8_t data);
int MPU6050_Recovery(void);
int16_t MPU6050_Read_x(void);
int16_t MPU6050_Read_y(void);
int16_t MPU6050_Read_z(void);
int MPU6050_Configure_MotionInt(void);
uint8_t MPU6050_1byte_read(uint32_t add);
int MPU6050_Updata_xyz(Accel_xyz* accel_xyz);

uint16_t Adc_read(void);
void Adc_set(void);
uint8_t Get_Battery_Percentage(void);

#endif /* INC_UTILS_H_ */
