// Memsic MMC5983MA Library

#ifndef _MMC5983MA_H_
#define _MMC5983MA_H_

#include <stdint.h>
#include "mmc5983ma_defs.h"

enum MMC5983MA_MEAS_TYPES {
    MMC5983MA_MEAS_TEMP = 0,
    MMC5983MA_MEAS_MAG,
    MMC5983MA_MEAS_ALL
};

enum MMC5983MA_ERROR {
    MMC5983MA_ERR_CANTSTARTMEAS  = -7,
    MMC5983MA_ERR_PRODIDMISMATCH = -6,
    MMC5983MA_ERR_PARAM     = -5,
    MMC5983MA_ERR_CFG       = -4,
    MMC5983MA_ERR_NULLPTR   = -3,
    MMC5983MA_ERR_BUS       = -2,
    MMC5983MA_ERR_GENERIC   = -1,
    MMC5983MA_SUCCESS       = 0
};

struct mmc5983ma_config
{
    uint8_t inhibit_x;              // Disable X Channel
    uint8_t inhibit_yz;             // Disable Y and ZChannel
    uint8_t enable_int;             // Enable interrupt pin
    uint8_t enable_auto_sr;         // Enable Auto Set/Reset
    uint8_t enable_cm;              // Enable continuous mode
    uint8_t enable_prd_set;         // Enable periodic Set
    uint8_t enable_3_wire_spi;      // Enable 3-wire SPI
    enum MMC5983MA_BW filter_bw;    // Decimation Filter Bandwidth
    enum MMC5983MA_CM_FREQ cm_freq; // Continuous Mode Frequency
    enum MMC5983MA_PRD_SET set_period; // Period for periodic Set operation when enabled
};

typedef int8_t (*mmc5983ma_read_fptr_t)(uint8_t reg_addr, uint8_t *data, uint32_t len);
typedef int8_t (*mmc5983ma_write_fptr_t)(uint8_t reg_addr, const uint8_t *data, uint32_t len);
typedef void (*mmc5983ma_delay_us_fptr_t)(uint32_t period);
struct mmc5983ma_dev
{
    uint8_t shadow_reg[4];              // Do not touch these
    mmc5983ma_read_fptr_t read;         // read function pointer
    mmc5983ma_write_fptr_t write;       // write function pointer
    mmc5983ma_delay_us_fptr_t delay_us; // delay function pointer
};

#ifdef __cplusplus
extern "C" {
#endif

int8_t mmc5983ma_set_reg_bm(const uint8_t reg_addr, const uint8_t bm, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_clear_reg_bm(const uint8_t reg_addr, const uint8_t bm, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_write_reg(const uint8_t reg_addr, uint8_t* val, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_read_reg(const uint8_t reg_addr, uint8_t* val, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_get_product_id(uint8_t *out, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_reset(struct mmc5983ma_dev *dev);

int8_t mmc5983ma_start_measurement(enum MMC5983MA_MEAS_TYPES type, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_is_measurement_done(enum MMC5983MA_MEAS_TYPES type, uint8_t *out, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_clear_interrupt(enum MMC5983MA_MEAS_TYPES type, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_get_temperature_raw(int8_t *out, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_get_temperature(float *out, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_get_x(uint32_t *x, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_get_y(uint32_t *y, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_get_z(uint32_t *z, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_get_xyz(uint32_t *x, uint32_t *y, uint32_t *z, struct mmc5983ma_dev *dev);

int8_t mmc5983ma_init(struct mmc5983ma_config* cfg, struct mmc5983ma_dev* dev);

#ifdef __cplusplus
}
#endif

#endif // _MMC5983MA_H_
