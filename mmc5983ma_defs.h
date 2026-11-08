// Memsic MMC5983MA Library

#ifndef _MMC5983MA_DEFS_H_
#define _MMC5983MA_DEFS_H_

// Constants definitions
#define MMC5983MA_RESET_DELAY_US        (12500) // microseconds. Datasheet says 10 milliseconds
#define MMC5983MA_I2C_ADDR              UINT8_C(0x30)
#define MMC5983MA_PROD_ID               UINT8_C(0x30)

// STATUS Register
#define MMC5983MA_STATUS_MEAS_M_DONE        (1 << 0)
#define MMC5983MA_STATUS_MEAS_T_DONE        (1 << 1)
#define MMC5983MA_STATUS_OTP_READ_DONE      (1 << 4)

// CONTROL 0 Register
#define MMC5983MA_CTRL0_TM_M                (1 << 0)
#define MMC5983MA_CTRL0_TM_T                (1 << 1)
#define MMC5983MA_CTRL0_INT_MEAS_DONE_EN    (1 << 2)
#define MMC5983MA_CTRL0_SET_OPERATION       (1 << 3)
#define MMC5983MA_CTRL0_RESET_OPERATION     (1 << 4)
#define MMC5983MA_CTRL0_AUTO_SR_EN          (1 << 5)
#define MMC5983MA_CTRL0_OTP_READ            (1 << 6)

// CONTROL 1 Register
#define MMC5983MA_CTRL1_BW0                 (1 << 0)
#define MMC5983MA_CTRL1_BW1                 (1 << 1)
#define MMC5983MA_CTRL1_X_INHIBIT           (1 << 2)
#define MMC5983MA_CTRL1_YZ_INHIBIT          (3 << 3) // TODO: could these be split up?
#define MMC5983MA_CTRL1_SW_RST              (1 << 7)

// CONTROL 2 Register
#define MMC5983MA_CTRL2_CM_FREQ_0           (1 << 0)
#define MMC5983MA_CTRL2_CM_FREQ_1           (1 << 1)
#define MMC5983MA_CTRL2_CM_FREQ_2           (1 << 2)
#define MMC5983MA_CTRL2_CMM_EN              (1 << 3)
#define MMC5983MA_CTRL2_PRD_SET_0           (1 << 4)
#define MMC5983MA_CTRL2_PRD_SET_1           (1 << 5)
#define MMC5983MA_CTRL2_PRD_SET_2           (1 << 6)
#define MMC5983MA_CTRL2_EN_PRD_SET          (1 << 7)

// CONTROL 3 Register
#define MMC5983MA_CTRL3_ST_ENP              (1 << 1)
#define MMC5983MA_CTRL3_ST_ENM              (1 << 2)
#define MMC5983MA_CTRL3_SPI_3W              (1 << 6)

// XYZOUT 2 Register
#define MMC5983MA_XYZOUT2_XOUT              (3 << 6)
#define MMC5983MA_XYZOUT2_YOUT              (3 << 4)
#define MMC5983MA_XYZOUT2_ZOUT              (3 << 2)

// Continuous Mode Frequency
enum MMC5983MA_CM_FREQ {
    MMC5983MA_CMFREQ_0HZ = 0,
    MMC5983MA_CMFREQ_1HZ,
    MMC5983MA_CMFREQ_10HZ,
    MMC5983MA_CMFREQ_20HZ,
    MMC5983MA_CMFREQ_50HZ,
    MMC5983MA_CMFREQ_100HZ,
    MMC5983MA_CMFREQ_200HZ,
    MMC5983MA_CMFREQ_1000HZ
};

enum MMC5983MA_PRD_SET {
    MMC5983MA_PRD_1 = 0,
    MMC5983MA_PRD_25,
    MMC5983MA_PRD_75,
    MMC5983MA_PRD_100,
    MMC5983MA_PRD_250,
    MMC5983MA_PRD_500,
    MMC5983MA_PRD_1000,
    MMC5983MA_PRD_2000
};

enum MMC5983MA_BW {
    MMC5983MA_BW_100HZ = 0, // 8ms measurement time
    MMC5983MA_BW_200HZ,     // 4ms measurement time
    MMC5983MA_BW_400HZ,     // 2ms measurement time
    MMC5983MA_BW_800HZ      // 0.5ms measurement time
};

enum MMC5983MA_REG_ADDR {
    MMC5983MA_REG_XOUT0 = 0,        // XOUT[17:10]
    MMC5983MA_REG_XOUT1,            // XOUT[9:2]
    MMC5983MA_REG_YOUT0,            // YOUT[17:10]
    MMC5983MA_REG_YOUT1,            // YOUT[9:2]
    MMC5983MA_REG_ZOUT0,            // ZOUT[17:10]
    MMC5983MA_REG_ZOUT1,            // ZOUT[9:2]
    MMC5983MA_REG_XYZOUT2,          // XOUT[1:0], YOUT[1:0], ZOUT[1:0]
    MMC5983MA_REG_TOUT,             // Temperature output
    MMC5983MA_REG_STATUS,           // Device Status
    MMC5983MA_REG_CTRL0,            // Control Register 0
    MMC5983MA_REG_CTRL1,            // Control Register 1
    MMC5983MA_REG_CTRL2,            // Control Register 2
    MMC5983MA_REG_CTRL3,            // Control Register 3
    MMC5983MA_REG_PRODID = 0x2F,    // Product ID
};

#endif // _MMC5983MA_DEFS_H_
