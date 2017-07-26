/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup low_power_pwm_example_main main.c
 * @{
 * @ingroup low_power_pwm_example
 * @brief Low Power PWM Example Application main file.
 *
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "app_error.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "app_util_platform.h"
#include "low_power_pwm.h"
#include "nordic_common.h"
#include "math.h"

/*Timer initialization parameters*/
#define OP_QUEUES_SIZE          1
#define APP_TIMER_PRESCALER     0

static low_power_pwm_t low_power_pwm_0;
static float R;
const int n_intervals = 100;
static int16_t interval = 0;
static bool up = true;
/**
 * @brief Function to be called in timer interrupt.
 *
 * @param[in] p_context     General purpose pointer (unused).
 */
void pwm_handler(void * p_context)
{
    uint8_t new_duty_cycle;
    uint32_t err_code;
    UNUSED_PARAMETER(p_context);

    low_power_pwm_t * pwm_instance = (low_power_pwm_t*)p_context;

    if(pwm_instance->bit_mask == BSP_LED_0_MASK)
    {

        if (up)
        {
            if(interval < n_intervals)
            {
                new_duty_cycle = pow (2, (interval / R)) - 1;
                err_code = low_power_pwm_duty_set(pwm_instance, new_duty_cycle);
                APP_ERROR_CHECK(err_code);
                interval++;
            }
            else
            {
                up = false;
            }
        }
        else
        {
            if(interval > 0)
            {
                new_duty_cycle = pow (2, (interval / R)) - 1;
                err_code = low_power_pwm_duty_set(pwm_instance, new_duty_cycle);
                APP_ERROR_CHECK(err_code);
                interval--;
            }
            else
            {
                up = true;
            }
        }

    }
}
/**
 * @brief Function to initalize low_power_pwm instances.
 *
 */

void low_power_init(void)
{
    uint32_t err_code;
    low_power_pwm_config_t low_power_pwm_config;

    APP_TIMER_DEF(lpp_timer_0);
    low_power_pwm_config.active_high = false;
    low_power_pwm_config.period = 255;
    low_power_pwm_config.bit_mask = BSP_LED_0_MASK;
    low_power_pwm_config.p_timer_id = &lpp_timer_0;

    err_code = low_power_pwm_init((&low_power_pwm_0), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm_0, 25);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_start((&low_power_pwm_0), low_power_pwm_0.bit_mask);
    APP_ERROR_CHECK(err_code);

    R = (n_intervals * log10(2))/(log10(255));
}

static void lfclk_init(void)
{
    uint32_t err_code;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    lfclk_init();

    // Start APP_TIMER to generate timeouts.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, OP_QUEUES_SIZE, NULL);

    low_power_init();

}

/** @} */
