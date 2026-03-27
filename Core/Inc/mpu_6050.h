#ifndef MPU_6050_H
#define MPU_6050_H

#include <stdint.h>

// MPU I2C address
#define MPU_I2C_ADDR            0xD0

// WHO_AM_I values
#define MPU6050_WHO_AM_I        0x68
#define MPU6500_WHO_AM_I        0x70
#define MPU9250_WHO_AM_I        0x71
#define MPU9255_WHO_AM_I        0x73

// Registers
#define MPU_REG_WHO_AM_I        0x75
#define MPU_REG_PWR_MGMT_1      0x6B
#define MPU_REG_GYRO_CONFIG     0x1B
#define MPU_REG_ACCEL_CONFIG    0x1C
#define MPU_REG_ACCEL_CONFIG2   0x1D  // MPU6500 only
#define MPU_REG_CONFIG          0x1A
#define MPU_REG_SMPLRT_DIV      0x19

// Functions
void mpu_inicializa(void);
void le_mpu(void);
void zera_inclinometro(void);
void mpu_verifica_fixacao(void);

#endif // MPU_6050_H
