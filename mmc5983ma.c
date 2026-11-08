// Memsic MMC5983MA Library

#include "mmc5983ma.h"
#include "mmc5983ma_defs.h"

#ifndef NULL
#define NULL        UINT8_C(0x00)
#endif

#define SUCCESS (MMC5983MA_SUCCESS)

static const struct mmc5983ma_config DEFAULT_CONFIG = {0};

// NULL Pointer Check
static int8_t null_ptr_check(const struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_us == NULL))
    {
        rslt = MMC5983MA_ERR_NULLPTR;
    }

    return rslt;
}

int8_t mmc5983ma_set_reg_bm(const uint8_t reg_addr, const uint8_t bm, struct mmc5983ma_dev *dev)
{
    int8_t rslt;
    uint8_t val;

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    // If its an internal control register
    if ((reg_addr >= MMC5983MA_REG_CTRL0) && (reg_addr <= MMC5983MA_REG_CTRL3)) {
        val = dev->shadow_reg[reg_addr - MMC5983MA_REG_CTRL0] | bm;
        rslt = dev->write(reg_addr, &val, 1);
        if (rslt != SUCCESS) return rslt;
        dev->shadow_reg[reg_addr - MMC5983MA_REG_CTRL0] = val;
    // For all other registers
    } else {
        rslt = dev->read(reg_addr, &val, 1);
        if (rslt != SUCCESS) return rslt;
        val |= bm; // Modify
        rslt = dev->write(reg_addr, &val, 1);
        if (rslt != SUCCESS) return rslt;
    }

    return rslt;
}

int8_t mmc5983ma_clear_reg_bm(const uint8_t reg_addr, const uint8_t bm, struct mmc5983ma_dev *dev)
{
    int8_t rslt;
    uint8_t val;

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    // If its an internal control register
    if ((reg_addr >= MMC5983MA_REG_CTRL0) && (reg_addr <= MMC5983MA_REG_CTRL3)) {
        val = dev->shadow_reg[reg_addr - MMC5983MA_REG_CTRL0] & ~bm;
        rslt = dev->write(reg_addr, &val, 1);
        if (rslt != SUCCESS) return rslt;
        dev->shadow_reg[reg_addr - MMC5983MA_REG_CTRL0] = val;
    // For all other registers
    } else {
        rslt = dev->read(reg_addr, &val, 1);
        if (rslt != SUCCESS) return rslt;
        val &= ~bm;
        rslt = dev->write(reg_addr, &val, 1);
        if (rslt != SUCCESS) return rslt;
    }

    return rslt;
}

int8_t mmc5983ma_write_reg(const uint8_t reg_addr, uint8_t* val, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t temp;

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;
    if (val == NULL) return MMC5983MA_ERR_NULLPTR;

    temp = *val;
    rslt = dev->write(reg_addr, &temp, 1);
    if (rslt != SUCCESS) return rslt;

    // If its a Internal Control register, update the host's copy
    if ((reg_addr >= MMC5983MA_REG_CTRL0) && (reg_addr <= MMC5983MA_REG_CTRL3)) {
        dev->shadow_reg[reg_addr - MMC5983MA_REG_CTRL0] = temp;
    }

    return rslt;
}

int8_t mmc5983ma_read_reg(const uint8_t reg_addr, uint8_t* val, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t temp;

    // If reading an Internal Control Register, use the value the host keeps
    if ((reg_addr >= MMC5983MA_REG_CTRL0) && (reg_addr <= MMC5983MA_REG_CTRL3)) {
        *val = dev->shadow_reg[reg_addr - MMC5983MA_REG_CTRL0];
    // Otherwise, read from device
    } else {
        rslt = dev->read(reg_addr, &temp, 1);
        if (rslt != SUCCESS) return rslt;
        *val = temp;
    }

    return rslt;
}

// Higher level functions

int8_t mmc5983ma_get_product_id(uint8_t *out, struct mmc5983ma_dev *dev)
{
    return mmc5983ma_read_reg(MMC5983MA_REG_PRODID, out, dev);
}

int8_t mmc5983ma_reset(struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;

    rslt = mmc5983ma_set_reg_bm(MMC5983MA_REG_CTRL1, MMC5983MA_CTRL1_SW_RST, dev);
    if (rslt != SUCCESS) return rslt;

    // Wait for device to reset
    dev->delay_us(MMC5983MA_RESET_DELAY_US);

    // Soft reset causes device to clear all registers
    // Manually clear the shadow bits
    dev->shadow_reg[0] = 0x00;
    dev->shadow_reg[1] = 0x00;
    dev->shadow_reg[2] = 0x00;
    dev->shadow_reg[3] = 0x00;

    return rslt;
}

int8_t mmc5983ma_start_measurement(enum MMC5983MA_MEAS_TYPES type, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t mask = 0;

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    switch (type) {
    case MMC5983MA_MEAS_TEMP:
        mask = MMC5983MA_CTRL0_TM_T;
        break;
    case MMC5983MA_MEAS_MAG:
        mask = MMC5983MA_CTRL0_TM_M;
        break;
    case MMC5983MA_MEAS_ALL:
        mask = MMC5983MA_CTRL0_TM_T | MMC5983MA_CTRL0_TM_M;
        break;
    default:
        return MMC5983MA_ERR_PARAM;
    }

    rslt = mmc5983ma_set_reg_bm(MMC5983MA_REG_CTRL0, mask, dev);

    return rslt;
}

int8_t mmc5983ma_is_measurement_done(enum MMC5983MA_MEAS_TYPES type, uint8_t *out, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t mask = 0;
    uint8_t val = 0;

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    switch (type) {
    case MMC5983MA_MEAS_TEMP:
        mask = MMC5983MA_STATUS_MEAS_T_DONE;
        break;
    case MMC5983MA_MEAS_MAG:
        mask = MMC5983MA_STATUS_MEAS_M_DONE;
        break;
    // TODO: Does it make sense to handle it this way?
    case MMC5983MA_MEAS_ALL:
        mask = MMC5983MA_STATUS_MEAS_T_DONE | MMC5983MA_STATUS_MEAS_M_DONE;
        break;
    default:
        return MMC5983MA_ERR_PARAM;
    }

    rslt = mmc5983ma_read_reg(MMC5983MA_REG_STATUS, &val, dev);
    if (rslt != SUCCESS) return rslt;

    *out = ((val & mask) > 0) ? 1 : 0;

    return rslt;
}

int8_t mmc5983ma_clear_interrupt(enum MMC5983MA_MEAS_TYPES type, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t mask = 0;

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    switch (type) {
    case MMC5983MA_MEAS_TEMP:
        mask = MMC5983MA_STATUS_MEAS_T_DONE;
        break;
    case MMC5983MA_MEAS_MAG:
        mask = MMC5983MA_STATUS_MEAS_M_DONE;
        break;
    case MMC5983MA_MEAS_ALL:
        mask = MMC5983MA_STATUS_MEAS_T_DONE | MMC5983MA_STATUS_MEAS_M_DONE;
        break;
    default:
        return MMC5983MA_ERR_PARAM;
    }

    return (mmc5983ma_set_reg_bm(MMC5983MA_REG_STATUS, mask, dev));
}

int8_t mmc5983ma_get_temperature_raw(int8_t *out, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t result = 0;

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    rslt = dev->read(MMC5983MA_REG_TOUT, &result, 1);
    if (rslt != SUCCESS) return rslt;

    *out = result;

    return rslt;
}

int8_t mmc5983ma_get_temperature(float *out, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t raw = 0;

    rslt = mmc5983ma_get_temperature_raw(&raw, dev);
    if (rslt != SUCCESS) return rslt;

    // On pg.14 of datasheet:
    // The range is -75~125°C, about 0.8°C/LSB, 00000000 stands for -75°C
    *out = -75.0f + ((float)(raw) * (200.0f / 255.0f));

    return rslt;
}

int8_t mmc5983ma_get_x(uint32_t *x, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t reg_vals[3] = {0};

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    rslt = (dev->read(MMC5983MA_REG_XOUT0, reg_vals, 2));
    if (rslt != SUCCESS) return rslt;
    rslt = (dev->read(MMC5983MA_REG_XYZOUT2, &reg_vals[2], 1));
    if (rslt != SUCCESS) return rslt;

    *x = reg_vals[0];                               // Xout[17:10] from XOUT0
    *x = (*x << 8) | reg_vals[1];                   // Xout[9:2] from XOUT1
    *x = (*x << 2) | (reg_vals[2] >> 6);            // Xout[1:0] from XYZOUT2

    return rslt;
}

int8_t mmc5983ma_get_y(uint32_t *y, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t reg_vals[3] = {0};

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    rslt = (dev->read(MMC5983MA_REG_YOUT0, reg_vals, 2));
    if (rslt != SUCCESS) return rslt;
    rslt = (dev->read(MMC5983MA_REG_XYZOUT2, &reg_vals[2], 1));
    if (rslt != SUCCESS) return rslt;

    *y = reg_vals[0];                               // Yout[17:10] from YOUT0
    *y = (*y << 8) | reg_vals[1];                   // Yout[9:2] from YOUT1
    *y = (*y << 2) | ((reg_vals[2] >> 4) & 0x03);   // Yout[1:0] from XYZOUT2

    return rslt;
}

int8_t mmc5983ma_get_z(uint32_t *z, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t reg_vals[3] = {0};

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    rslt = (dev->read(MMC5983MA_REG_ZOUT0, reg_vals, 3));
    if (rslt != SUCCESS) return rslt;

    *z = reg_vals[0];                               // Zout[17:10] from ZOUT0
    *z = (*z << 8) | reg_vals[1];                   // Zout[9:2] from ZOUT1
    *z = (*z << 2) | ((reg_vals[2] >> 2) & 0x03);   // Zout[1:0] from XYZOUT2

    return rslt;
}

int8_t mmc5983ma_get_xyz(uint32_t *x, uint32_t *y, uint32_t *z, struct mmc5983ma_dev *dev)
{
    int8_t rslt = SUCCESS;
    uint8_t reg_vals[7] = {0};

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    rslt = (dev->read(MMC5983MA_REG_XOUT0, reg_vals, 7));
    if (rslt != SUCCESS) return rslt;

    *x = reg_vals[0];                               // Xout[17:10] from XOUT0
    *x = (*x << 8) | reg_vals[1];                   // Xout[9:2] from XOUT1
    *x = (*x << 2) | (reg_vals[6] >> 6);            // Xout[1:0] from XYZOUT2

    *y = reg_vals[2];                               // Yout[17:10] from YOUT0
    *y = (*y << 8) | reg_vals[3];                   // Yout[9:2] from YOUT1
    *y = (*y << 2) | ((reg_vals[6] >> 4) & 0x03);   // Yout[1:0] from XYZOUT2

    *z = reg_vals[4];                               // Zout[17:10] from ZOUT0
    *z = (*z << 8) | reg_vals[5];                   // Zout[9:2] from ZOUT1
    *z = (*z << 2) | ((reg_vals[6] >> 2) & 0x03);   // Zout[1:0] from XYZOUT2

    return rslt;
}

int8_t mmc5983ma_init(struct mmc5983ma_config* cfg, struct mmc5983ma_dev* dev)
{
    int8_t rslt = SUCCESS;
    uint8_t temp = 0;

    rslt = null_ptr_check(dev);
    if (rslt != SUCCESS) return rslt;

    if (cfg == NULL) return MMC5983MA_ERR_NULLPTR;

    // Validate Config

    // Datasheet says that if Periodic Set is enabled, both
    // Continuous Mode and Automatic Set/Reset must both be enabled
    if (cfg->enable_prd_set && (!cfg->enable_auto_sr || !cfg->enable_cm)) {
        return MMC5983MA_ERR_CFG;
    }

    // If Continuous Mode is enabled
    if (cfg->enable_cm) {
        switch (cfg->cm_freq) {
        // Cannot have 0Hz set with continuous mode enabled
        case MMC5983MA_CMFREQ_0HZ:
            return MMC5983MA_ERR_CFG;
            break;
        // Filter Bandwidth must be high enough (400Hz or 800Hz)
        case MMC5983MA_CMFREQ_200HZ:
            if (cfg->filter_bw < MMC5983MA_BW_400HZ) {
                return MMC5983MA_ERR_CFG;
            }
            break;
        // Filter Bandwidth must be high enough (800Hz)
        case MMC5983MA_CMFREQ_1000HZ:
            if (cfg->filter_bw != MMC5983MA_BW_800HZ) {
                return MMC5983MA_ERR_CFG;
            }
            break;
        }
    }

    // Verify that the Product ID is correct as a first check
    rslt = mmc5983ma_get_product_id(&temp, dev);
    if (temp != MMC5983MA_PROD_ID) return MMC5983MA_ERR_PRODIDMISMATCH;

    // Reset device to reset configuration of internal registers
    rslt = mmc5983ma_reset(dev);
    if (rslt != SUCCESS) return rslt;

    // Internal Control 0 Register
    temp = 0;
    if (cfg->enable_int)        temp |= (1 << 2);
    if (cfg->enable_auto_sr)    temp |= (1 << 5);
    if (temp) {
        rslt = mmc5983ma_set_reg_bm(MMC5983MA_REG_CTRL0, temp, dev);
        if (rslt != SUCCESS) return rslt;
    }

    // Internal Control 1 register
    temp = 0;
    if (cfg->inhibit_x)         temp |= (1 << 2);
    if (cfg->inhibit_yz)        temp |= (3 << 3);
    temp |= cfg->filter_bw;
    if (temp) {
        rslt = mmc5983ma_set_reg_bm(MMC5983MA_REG_CTRL1, temp, dev);
        if (rslt != SUCCESS) return rslt;
    }

    // Internal Control 2 register
    temp = 0;
    if (cfg->enable_cm)         temp |= (1 << 3);
    if (cfg->enable_prd_set)    temp |= (1 << 7);
    temp |= cfg->cm_freq;
    temp |= (cfg->set_period << 4);
    if (temp) {
        rslt = mmc5983ma_set_reg_bm(MMC5983MA_REG_CTRL2, temp, dev);
        if (rslt != SUCCESS) return rslt;
    }

    // Internal Control 3 Register
    temp = 0;
    if (cfg->enable_3_wire_spi) temp |= (1 << 6);
    if (temp) {
        rslt = mmc5983ma_set_reg_bm(MMC5983MA_REG_CTRL3, temp, dev);
        if (rslt != SUCCESS) return rslt;
    }

    return rslt;
}
