#include "main.h"
#include "mpu_6050.h"
#include "structs.h"
#include "calibracao.h"
#include <math.h>

#define RAD_TO_DEG 57.295779513f
#define DEG_TO_RAD 0.017453293f

// I2C handle
extern I2C_HandleTypeDef hi2c1;
#define MPU_I2C &hi2c1


// ============================================
// PRIVATE VARIABLES
// ============================================

static uint8_t mpu_type = 0;           // 0=unknown, 1=6050, 2=6500+
static float KalmanAngleRoll = 0;
static float KalmanAnglePitch = 0;
static float KalmanUncertaintyAngleRoll = 0;
static float KalmanUncertaintyAnglePitch = 0;

// ============================================
// I2C FUNCTIONS
// ============================================

static uint8_t mpu_write_byte(uint8_t reg, uint8_t data)
{
    return (HAL_I2C_Mem_Write(MPU_I2C, MPU_I2C_ADDR, reg, 1, &data, 1, 10) == HAL_OK) ? 0 : 1;
}

static uint8_t mpu_read_bytes(uint8_t reg, uint8_t* data, uint16_t len)
{
    return (HAL_I2C_Mem_Read(MPU_I2C, MPU_I2C_ADDR, reg, 1, data, len, 10) == HAL_OK) ? 0 : 1;
}

// ============================================
// CHIP DETECTION
// ============================================

static uint8_t mpu_detect_type(void)
{
    uint8_t who_am_i = 0;
    mpu_read_bytes(MPU_REG_WHO_AM_I, &who_am_i, 1);
    
    if (who_am_i == MPU6050_WHO_AM_I) return 1;       // MPU6050
    if (who_am_i == MPU6500_WHO_AM_I || 
        who_am_i == MPU9250_WHO_AM_I || 
        who_am_i == MPU9255_WHO_AM_I) return 2;       // MPU6500 family
    
    return 0;  // Unknown
}

// ============================================
// KALMAN FILTER (1D)
// ============================================

static void kalman_1d(float* KalmanState, float* KalmanUncertainty, float KalmanInput, float KalmanMeasurement, float dt)
{
    // Predict state
    *KalmanState = *KalmanState + dt * KalmanInput;
    *KalmanUncertainty = *KalmanUncertainty + dt * dt * 16.0f;  // 4*4
    
    // Calculate Kalman gain
    float KalmanGain = *KalmanUncertainty / (*KalmanUncertainty + 9.0f);  // 3*3
    
    // Update state
    *KalmanState = *KalmanState + KalmanGain * (KalmanMeasurement - *KalmanState);
    *KalmanUncertainty = (1.0f - KalmanGain) * (*KalmanUncertainty);
}

// ============================================
// AXIS MAPPING
// ============================================

// Global axis mapping variables (extern in original)
uint8_t X = 0, Y = 1, Z = 2;

void mpu_verifica_fixacao(void)
{
    // Map axes based on mounting position
    switch (posicao_mpu) {
        case 1: X = 1; Y = 2; Z = 0; break;
        case 2: X = 2; Y = 0; Z = 1; break;
        case 3: X = 0; Y = 1; Z = 2; break;
        case 4: X = 2; Y = 1; Z = 0; break;
        case 5: X = 0; Y = 2; Z = 1; break;
        case 6: X = 1; Y = 0; Z = 2; break;
        default: X = 0; Y = 1; Z = 2; break;
    }
}

// ============================================
// MPU INITIALIZATION
// ============================================

void mpu_inicializa(void)
{
    uint8_t data_write;
    
    // Detect MPU type
    mpu_type = mpu_detect_type();
    
    if (mpu_type == 0) {
        bit_erro_MPU = 1;
				while(1)
				{
					HAL_Delay(1);
				}
        return;
    }
    
    // Reset device
    mpu_write_byte(MPU_REG_PWR_MGMT_1, 0x80);
    HAL_Delay(100);
    mpu_write_byte(MPU_REG_PWR_MGMT_1, 0x01);
    HAL_Delay(100);
    
    // Configure gyro (500dps)
    data_write = 0x08;  // 500dps
    mpu_write_byte(MPU_REG_GYRO_CONFIG, data_write);
    HAL_Delay(10);
    
    // Configure accelerometer (8g)
    data_write = 0x10;  // 8g
    mpu_write_byte(MPU_REG_ACCEL_CONFIG, data_write);
    HAL_Delay(10);
    
    // Configure DLPF
    if (mpu_type == 1) {  // MPU6050
        // Combined DLPF for gyro and accel
        data_write = 0x05;  // 44Hz
        mpu_write_byte(MPU_REG_CONFIG, data_write);
    } else {  // MPU6500 family
        // Gyro DLPF
        data_write = 0x05;  // 44Hz
        mpu_write_byte(MPU_REG_CONFIG, data_write);
        // Accel DLPF (separate register)
        data_write = 0x03;  // 45Hz
        mpu_write_byte(MPU_REG_ACCEL_CONFIG2, data_write);
    }
    HAL_Delay(10);
    
    // Set sample rate divider for 200Hz
    data_write = 4;  // 1000Hz / (1 + 4) = 200Hz
    mpu_write_byte(MPU_REG_SMPLRT_DIV, data_write);
    HAL_Delay(10);
    
    // Set mounting position from saved value
    // posicao_mpu should be loaded from flash here
    mpu_verifica_fixacao();
    
    // Load calibration from flash here
    // zero_eixo_x = val_buff_zeramento[1];
    // zero_eixo_y = val_buff_zeramento[2];
    
    status_mpu = 1;
    bit_erro_MPU = 0;
}

// ============================================
// READ GYRO AND ACCEL
// ============================================

static void gyro_signals(float* RateRoll, float* RatePitch, float* RateYaw, 
                         float* AccX, float* AccY, float* AccZ)
{
    uint8_t data_read[14];  // 6 accel + 6 gyro + 2 temp
    
    // Read all sensor data at once
    if (mpu_read_bytes(0x3B, data_read, 14) != 0) {
        *RateRoll = *RatePitch = *RateYaw = 0;
        *AccX = *AccY = *AccZ = 0;
        return;
    }
    
    // Convert raw data
    int16_t AccXLSB = (int16_t)(data_read[0] << 8 | data_read[1]);
    int16_t AccYLSB = (int16_t)(data_read[2] << 8 | data_read[3]);
    int16_t AccZLSB = (int16_t)(data_read[4] << 8 | data_read[5]);
    int16_t GyroX = (int16_t)(data_read[8] << 8 | data_read[9]);
    int16_t GyroY = (int16_t)(data_read[10] << 8 | data_read[11]);
    int16_t GyroZ = (int16_t)(data_read[12] << 8 | data_read[13]);
    
    // Apply axis mapping
    float accel_raw[3] = {(float)AccXLSB, (float)AccYLSB, (float)AccZLSB};
    float gyro_raw[3] = {(float)GyroX, (float)GyroY, (float)GyroZ};
    
    // Map to standard orientation
    float accel_mapped[3] = {accel_raw[X], accel_raw[Y], accel_raw[Z]};
    float gyro_mapped[3] = {gyro_raw[X], gyro_raw[Y], gyro_raw[Z]};
    
    // Convert to physical units (8g, 500dps)
    *AccX = accel_mapped[0] / 4096.0f;
    *AccY = accel_mapped[1] / 4096.0f;
    *AccZ = accel_mapped[2] / 4096.0f;
    
    *RateRoll = gyro_mapped[0] / 65.5f;
    *RatePitch = gyro_mapped[1] / 65.5f;
    *RateYaw = gyro_mapped[2] / 65.5f;
}

// ============================================
// CALIBRATION (ZERAGEM)
// ============================================

void zera_inclinometro(void)
{
    float RateRoll, RatePitch, RateYaw;
    float AccX, AccY, AccZ;
    float AngleRoll, AnglePitch;
    
    uint32_t start_time = uwTick;
    
    // Collect samples for 2 seconds
    uint16_t samples = 0;
    float roll_sum = 0, pitch_sum = 0;
    
    while ((uwTick - start_time) < 2000) {
        gyro_signals(&RateRoll, &RatePitch, &RateYaw, &AccX, &AccY, &AccZ);
        
        // Calculate angles from accelerometer
        AngleRoll = atan2f(AccY, AccZ) * RAD_TO_DEG;
        AnglePitch = atan2f(-AccX, sqrtf(AccY*AccY + AccZ*AccZ)) * RAD_TO_DEG;
        
        // Update Kalman filter
        float dt = 0.005f;  // 200Hz
        kalman_1d(&KalmanAngleRoll, &KalmanUncertaintyAngleRoll, RateRoll, AngleRoll, dt);
        kalman_1d(&KalmanAnglePitch, &KalmanUncertaintyAnglePitch, RatePitch, AnglePitch, dt);
        
        roll_sum += KalmanAngleRoll;
        pitch_sum += KalmanAnglePitch;
        samples++;
        
        // Blink LED or other indicator
        leds_calibracao();
    }
    
    if (samples > 0) {
        // Store calibration values
        zero_eixo_x = roll_sum / samples;
        zero_eixo_y = pitch_sum / samples;
        
        // Detect mounting position from average accelerometer
        // This would need a few samples to determine which axis is pointing down
        // For simplicity, assume default position 3
        posicao_mpu = 3;
        mpu_verifica_fixacao();
        
        // Save to flash would go here
        // val_buff_zeramento[0] = posicao_mpu;
        // val_buff_zeramento[1] = zero_eixo_x;
        // val_buff_zeramento[2] = zero_eixo_y;
    }
}

// ============================================
// MAIN READING FUNCTION
// ============================================

void le_mpu(void)
{
    static uint32_t prev_time = 0;
    static float mpu_roll_ant = 0, mpu_pitch_ant = 0;
    
    float RateRoll, RatePitch, RateYaw;
    float AccX, AccY, AccZ;
    float AngleRoll, AnglePitch;
    float roll_loc, pitch_loc;
    
    // Read sensors
    gyro_signals(&RateRoll, &RatePitch, &RateYaw, &AccX, &AccY, &AccZ);
    
    // Calculate time delta
    uint32_t current_time = uwTick;
    float dt = (current_time - prev_time) / 1000.0f;  // Convert to seconds
    if (dt > 0.1f) dt = 0.005f;  // Cap at 200Hz max
    prev_time = current_time;
    
    // Calculate angles from accelerometer
    AngleRoll = atan2f(AccY, AccZ) * RAD_TO_DEG;
    AnglePitch = atan2f(-AccX, sqrtf(AccY*AccY + AccZ*AccZ)) * RAD_TO_DEG;
    
    // Update Kalman filter
    kalman_1d(&KalmanAngleRoll, &KalmanUncertaintyAngleRoll, RateRoll, AngleRoll, dt);
    kalman_1d(&KalmanAnglePitch, &KalmanUncertaintyAnglePitch, RatePitch, AnglePitch, dt);
    
    // Apply calibration offset
    roll_loc = KalmanAngleRoll - zero_eixo_x;
    pitch_loc = KalmanAnglePitch - zero_eixo_y;
    
    // Simple low-pass filter
    float roll_err = roll_loc - mpu_roll_ant;
    mpu_roll_ant = mpu_roll_ant + (roll_err * 0.008f);
    
    float pitch_err = pitch_loc - mpu_pitch_ant;
    mpu_pitch_ant = mpu_pitch_ant + (pitch_err * 0.008f);
    
		pitch = mpu_pitch_ant;
		roll = mpu_roll_ant;
		
    // Convert to output format (0.1° resolution)
    valor_eixo_y = (int16_t)(mpu_pitch_ant * 10.0f);
    valor_eixo_x = (int16_t)(mpu_roll_ant * 10.0f);
		
    // Character format (integer degrees)
    // val_char_x = (int8_t)mpu_pitch_ant;
    // val_char_y = (int8_t)mpu_roll_ant;
}
