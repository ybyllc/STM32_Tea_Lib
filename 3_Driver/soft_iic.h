#ifndef _SOFT_IIC_H
#define _SOFT_IIC_H

#include "common.h"

typedef struct {
    GPIO_TypeDef *scl_port;
    uint16_t scl_pin;
    GPIO_TypeDef *sda_port;
    uint16_t sda_pin;
    uint8_t addr;
    uint32_t delay_us;
} soft_iic_t;

void soft_iic_init(soft_iic_t *iic, GPIO_TypeDef *scl_port, uint16_t scl_pin,
                   GPIO_TypeDef *sda_port, uint16_t sda_pin, uint8_t addr, uint32_t delay_us);

void soft_iic_start(soft_iic_t *iic);
void soft_iic_stop(soft_iic_t *iic);
uint8_t soft_iic_wait_ack(soft_iic_t *iic);
void soft_iic_send_ack(soft_iic_t *iic, uint8_t ack);
void soft_iic_send_byte(soft_iic_t *iic, uint8_t dat);
uint8_t soft_iic_read_byte(soft_iic_t *iic, uint8_t ack);

void soft_iic_write_reg(soft_iic_t *iic, uint8_t reg, uint8_t dat);
uint8_t soft_iic_read_reg(soft_iic_t *iic, uint8_t reg);
uint16_t soft_iic_read_reg16(soft_iic_t *iic, uint8_t reg);
void soft_iic_write_reg16(soft_iic_t *iic, uint8_t reg, uint16_t dat);

void soft_iic_write_bytes(soft_iic_t *iic, uint8_t reg, const uint8_t *dat, uint16_t len);
void soft_iic_read_bytes(soft_iic_t *iic, uint8_t reg, uint8_t *buf, uint16_t len);

#endif
