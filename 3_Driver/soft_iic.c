#include "soft_iic.h"
#include "app_main.h"

#define SCL_HIGH(iic)  HAL_GPIO_WritePin((iic)->scl_port, (iic)->scl_pin, GPIO_PIN_SET)
#define SCL_LOW(iic)   HAL_GPIO_WritePin((iic)->scl_port, (iic)->scl_pin, GPIO_PIN_RESET)
#define SDA_HIGH(iic)  HAL_GPIO_WritePin((iic)->sda_port, (iic)->sda_pin, GPIO_PIN_SET)
#define SDA_LOW(iic)   HAL_GPIO_WritePin((iic)->sda_port, (iic)->sda_pin, GPIO_PIN_RESET)
#define READ_SDA(iic)  HAL_GPIO_ReadPin((iic)->sda_port, (iic)->sda_pin)

static void sda_in(soft_iic_t *iic)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = iic->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(iic->sda_port, &GPIO_InitStruct);
}

static void sda_out(soft_iic_t *iic)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = iic->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(iic->sda_port, &GPIO_InitStruct);
}

void soft_iic_init(soft_iic_t *iic, GPIO_TypeDef *scl_port, uint16_t scl_pin,
                   GPIO_TypeDef *sda_port, uint16_t sda_pin, uint8_t addr, uint32_t delay_us)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    iic->scl_port = scl_port;
    iic->scl_pin = scl_pin;
    iic->sda_port = sda_port;
    iic->sda_pin = sda_pin;
    iic->addr = addr;
    iic->delay_us = delay_us;

    if (scl_port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (scl_port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (scl_port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (scl_port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (scl_port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (scl_port == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();

    if (sda_port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (sda_port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (sda_port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (sda_port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (sda_port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (sda_port == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();

    GPIO_InitStruct.Pin = scl_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(scl_port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(scl_port, scl_pin, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(sda_port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(sda_port, sda_pin, GPIO_PIN_SET);
}

void soft_iic_start(soft_iic_t *iic)
{
    sda_out(iic);
    SCL_HIGH(iic);
    delay_us(iic->delay_us);
    SDA_HIGH(iic);
    delay_us(iic->delay_us);
    SDA_LOW(iic);
    delay_us(iic->delay_us);
    SCL_LOW(iic);
    delay_us(iic->delay_us);
}

void soft_iic_stop(soft_iic_t *iic)
{
    sda_out(iic);
    SCL_LOW(iic);
    delay_us(iic->delay_us);
    SDA_LOW(iic);
    delay_us(iic->delay_us);
    SCL_HIGH(iic);
    delay_us(iic->delay_us);
    SDA_HIGH(iic);
    delay_us(iic->delay_us);
}

uint8_t soft_iic_wait_ack(soft_iic_t *iic)
{
    uint8_t ucErrTime = 0;
    sda_in(iic);
    SCL_HIGH(iic);
    delay_us(iic->delay_us);
    while (READ_SDA(iic)) {
        ucErrTime++;
        if (ucErrTime > 250) {
            soft_iic_stop(iic);
            return 1;
        }
    }
    SCL_LOW(iic);
    delay_us(iic->delay_us);
    return 0;
}

void soft_iic_send_ack(soft_iic_t *iic, uint8_t ack)
{
    SCL_LOW(iic);
    delay_us(iic->delay_us);
    sda_out(iic);
    if (ack) SDA_HIGH(iic);
    else SDA_LOW(iic);
    delay_us(iic->delay_us);
    SCL_HIGH(iic);
    delay_us(iic->delay_us);
    SCL_LOW(iic);
    delay_us(iic->delay_us);
}

void soft_iic_send_byte(soft_iic_t *iic, uint8_t dat)
{
    uint8_t t;
    sda_out(iic);
    SCL_LOW(iic);
    delay_us(iic->delay_us);
    for (t = 0; t < 8; t++) {
        if (dat & 0x80) SDA_HIGH(iic);
        else SDA_LOW(iic);
        dat <<= 1;
        delay_us(iic->delay_us);
        SCL_HIGH(iic);
        delay_us(iic->delay_us);
        SCL_LOW(iic);
        delay_us(iic->delay_us);
    }
}

uint8_t soft_iic_read_byte(soft_iic_t *iic, uint8_t ack)
{
    uint8_t i, receive = 0;
    sda_in(iic);
    delay_us(iic->delay_us);  // 等待 SDA 引脚稳定
    for (i = 0; i < 8; i++) {
        SCL_LOW(iic);
        delay_us(iic->delay_us);
        SCL_HIGH(iic);
        delay_us(iic->delay_us);
        receive <<= 1;
        if (READ_SDA(iic)) receive++;
        delay_us(iic->delay_us);
    }
    SCL_LOW(iic);
    // ack=1 表示发送 ACK (继续读取), ack=0 表示发送 NACK (停止读取)
    if (ack) soft_iic_send_ack(iic, 0);  // 发送 ACK (拉低 SDA)
    else soft_iic_send_ack(iic, 1);      // 发送 NACK (拉高 SDA)
    delay_us(iic->delay_us);  // 等待 ACK/NACK 完成
    return receive;
}

void soft_iic_write_reg(soft_iic_t *iic, uint8_t reg, uint8_t dat)
{
    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) & 0xFE);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_send_byte(iic, reg);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_send_byte(iic, dat);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_stop(iic);
    delay_us(10);
}

uint8_t soft_iic_read_reg(soft_iic_t *iic, uint8_t reg)
{
    uint8_t val = 0;
    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) & 0xFE);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return 0xFF; }
    soft_iic_send_byte(iic, reg);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return 0xFF; }

    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) | 0x01);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return 0xFF; }
    val = soft_iic_read_byte(iic, 0);
    soft_iic_stop(iic);
    delay_us(10);
    return val;
}

uint16_t soft_iic_read_reg16(soft_iic_t *iic, uint8_t reg)
{
    uint16_t val;
    uint8_t high_byte, low_byte;
    
    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) & 0xFE);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return 0xFFFF; }
    soft_iic_send_byte(iic, reg);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return 0xFFFF; }

    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) | 0x01);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return 0xFFFF; }
    
    // 读取高字节，发送 ACK
    high_byte = soft_iic_read_byte(iic, 1);
    // 读取低字节，发送 NACK
    low_byte = soft_iic_read_byte(iic, 0);
    
    val = ((uint16_t)high_byte << 8) | low_byte;
    soft_iic_stop(iic);
    return val;
}

void soft_iic_write_reg16(soft_iic_t *iic, uint8_t reg, uint16_t dat)
{
    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) & 0xFE);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_send_byte(iic, reg);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_send_byte(iic, (dat >> 8) & 0xFF);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_send_byte(iic, dat & 0xFF);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_stop(iic);
}

void soft_iic_write_bytes(soft_iic_t *iic, uint8_t reg, const uint8_t *dat, uint16_t len)
{
    uint16_t i;
    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) & 0xFE);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_send_byte(iic, reg);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    for (i = 0; i < len; i++) {
        soft_iic_send_byte(iic, dat[i]);
        if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    }
    soft_iic_stop(iic);
}

void soft_iic_read_bytes(soft_iic_t *iic, uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) & 0xFE);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    soft_iic_send_byte(iic, reg);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }

    soft_iic_start(iic);
    soft_iic_send_byte(iic, (iic->addr << 1) | 0x01);
    if (soft_iic_wait_ack(iic)) { soft_iic_stop(iic); return; }
    for (i = 0; i < len; i++) {
        buf[i] = soft_iic_read_byte(iic, (i == len - 1) ? 0 : 1);
    }
    soft_iic_stop(iic);
}
