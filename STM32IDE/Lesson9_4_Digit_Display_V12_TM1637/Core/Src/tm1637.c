#include "tm1637.h"

// Hàm delay nhỏ
static void TM1637_Delay(void) {
    for(volatile int i=0; i<200; i++);
}

static void TM1637_DIO_Input(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TM1637_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TM1637_DIO_GPIO, &GPIO_InitStruct);
}

static void TM1637_DIO_Output(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TM1637_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TM1637_DIO_GPIO, &GPIO_InitStruct);
}

// Set/Reset pin
static void TM1637_CLK_High(void){ HAL_GPIO_WritePin(TM1637_CLK_GPIO, TM1637_CLK_PIN, GPIO_PIN_SET); }
static void TM1637_CLK_Low(void){ HAL_GPIO_WritePin(TM1637_CLK_GPIO, TM1637_CLK_PIN, GPIO_PIN_RESET); }
void TM1637_DIO_High(void){ HAL_GPIO_WritePin(TM1637_DIO_GPIO, TM1637_DIO_PIN, GPIO_PIN_SET); }
static void TM1637_DIO_Low(void){ HAL_GPIO_WritePin(TM1637_DIO_GPIO, TM1637_DIO_PIN, GPIO_PIN_RESET); }

// Start/Stop
static void TM1637_Start(void){
    TM1637_CLK_High();
    TM1637_DIO_High();
    TM1637_Delay();
    TM1637_DIO_Low();
    TM1637_Delay();
    TM1637_CLK_Low();
}
static void TM1637_Stop(void){
    TM1637_CLK_Low();
    TM1637_DIO_Low();
    TM1637_Delay();
    TM1637_CLK_High();
    TM1637_Delay();
    TM1637_DIO_High();
}

// Ghi 1 byte
static uint8_t TM1637_WriteByte(uint8_t b){
    for(uint8_t i = 0; i < 8; i++){
        TM1637_CLK_Low();

        if(b & 0x01)
            TM1637_DIO_High();   // release (open-drain)
        else
            TM1637_DIO_Low();    // kéo xuống

        TM1637_Delay();

        TM1637_CLK_High();
        TM1637_Delay();

        b >>= 1;
    }

    // ===== ACK =====
    TM1637_CLK_Low();

    TM1637_DIO_High();   // 🔥 QUAN TRỌNG: chỉ thả bus, KHÔNG đổi mode
    TM1637_Delay();

    TM1637_CLK_High();
    TM1637_Delay();

    uint8_t ack = HAL_GPIO_ReadPin(TM1637_DIO_GPIO, TM1637_DIO_PIN);

    TM1637_CLK_Low();
    TM1637_Delay();

    return (ack == GPIO_PIN_RESET);
}

// API
void TM1637_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = TM1637_CLK_PIN | TM1637_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TM1637_CLK_GPIO, &GPIO_InitStruct);
    TM1637_DisplayOn(7);
}

void TM1637_DisplayOn(uint8_t brightness){
    if(brightness > TM1637_BRIGHTNESS_MAX) brightness = TM1637_BRIGHTNESS_MAX;
    TM1637_Start();
    TM1637_WriteByte(TM1637_CMD_CTRL | 0x08 | brightness);
    TM1637_Stop();
}

void TM1637_DisplayOff(void){
    TM1637_Start();
    TM1637_WriteByte(TM1637_CMD_CTRL);
    TM1637_Stop();
}

void TM1637_WriteDigit(uint8_t pos, uint8_t data){
    TM1637_Start();
    TM1637_WriteByte(TM1637_CMD_ADDR | pos);
    TM1637_WriteByte(data);
    TM1637_Stop();
}

void TM1637_WriteDigits(uint8_t *data, uint8_t len){
    TM1637_Start();
    TM1637_WriteByte(TM1637_CMD_DATA); // auto increment
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(TM1637_CMD_ADDR);
    for(uint8_t i=0; i<len; i++){
        TM1637_WriteByte(data[i]);
    }
    TM1637_Stop();
}
