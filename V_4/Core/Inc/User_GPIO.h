#ifndef INC_USER_GPIO_H_
#define INC_USER_GPIO_H_

#include "utils.h"
extern uint8_t time_flag;
extern uint8_t MPU6050_motion_flag_1;
extern uint8_t rotation_new;
extern uint8_t rotation_old;
extern uint8_t target_flag;
extern uint8_t adc_flag;

void GPIO_set(void);
void Put_16bit(uint16_t data);
void print_time(const Time* time, uint8_t rotation, uint8_t position);
void Get_Current_Rotation(Time* time, int x, int y, int z);
void GPIO_BUZZER(uint8_t data);
void Buzzer_Sound(uint8_t data);
void GPIO_RCK(uint8_t data);
void GPIO_SER(uint8_t data);
void GPIO_SCK(uint8_t data);
#endif /* INC_USER_GPIO_H_ */
