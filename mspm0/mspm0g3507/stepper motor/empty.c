#include "ti_msp_dl_config.h"
#include <stdint.h>

#define MOTOR_STEP_ANGLE_X10  (18UL)   /* 1.8 degrees x 10 */
#define MICROSTEP             (256UL)
#define TIMER_MAX_LOAD        (65535UL)
#define TIMER_MIN_LOAD        (4UL)

/*
 * speed_deg_s：角速度，单位 °/s
 * 正数正转，负数反转，0 停止
 */
void Stepper_SetSpeed(int32_t speed_deg_s)
{
    uint32_t abs_speed;
    uint64_t pulse_frequency;
    uint32_t load_value;

    if (speed_deg_s == 0) {
        DL_TimerA_stopCounter(STEP_INST);
        return;
    }

    /* 改变方向前先停止脉冲 */
    DL_TimerA_stopCounter(STEP_INST);

    if (speed_deg_s > 0) {
        DL_GPIO_setPins(
            STEP_DIR_PORT,
            STEP_DIR_STEP_DIR_PIN_PIN
        );

        abs_speed = (uint32_t)speed_deg_s;
    } else {
        DL_GPIO_clearPins(
            STEP_DIR_PORT,
            STEP_DIR_STEP_DIR_PIN_PIN
        );

        /* Convert through int64_t so INT32_MIN is handled safely. */
        abs_speed = (uint32_t)(-(int64_t)speed_deg_s);
    }

    /*
     * pulse_frequency
     * = speed × MICROSTEP / 1.8
     * = speed × MICROSTEP × 10 / 18
     */
    pulse_frequency =
        (((uint64_t)abs_speed * MICROSTEP * 10UL) /
        MOTOR_STEP_ANGLE_X10);

    if (pulse_frequency == 0U) {
        return;
    }

    load_value = (uint32_t)(STEP_INST_CLK_FREQ / pulse_frequency);

    if (load_value > TIMER_MAX_LOAD) {
        load_value = TIMER_MAX_LOAD;
    }

    if (load_value < TIMER_MIN_LOAD) {
        load_value = TIMER_MIN_LOAD;
    }

    DL_Timer_setLoadValue(
        STEP_INST,
        load_value - 1U
    );

    DL_TimerA_setCaptureCompareValue(
        STEP_INST,
        load_value / 2U,
        GPIO_STEP_C0_IDX
    );

    DL_TimerA_startCounter(STEP_INST);
}


int main(void)
{
    SYSCFG_DL_init();
    Stepper_SetSpeed(36);

    while (1) {
        __WFI();
    }
}
