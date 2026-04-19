#ifndef __TM1637_H
#define __TM1637_H

#include "stm32f1xx_hal.h"

// Cấu hình chân CLK và DIO
#define TM1637_CLK_GPIO   GPIOB
#define TM1637_CLK_PIN    GPIO_PIN_6
#define TM1637_DIO_GPIO   GPIOB
#define TM1637_DIO_PIN    GPIO_PIN_7

// Các lệnh TM1637
#define TM1637_CMD_DATA   0x40  // ghi dữ liệu, auto increment
#define TM1637_CMD_ADDR   0xC0  // địa chỉ bắt đầu
#define TM1637_CMD_CTRL   0x80  // điều khiển hiển thị

// Độ sáng (0–7)
#define TM1637_BRIGHTNESS_MAX 7

void TM1637_Init(void);
void TM1637_DisplayOn(uint8_t brightness);
void TM1637_DisplayOff(void);
void TM1637_WriteDigit(uint8_t pos, uint8_t data);
void TM1637_WriteDigits(uint8_t *data, uint8_t len);

#endif
