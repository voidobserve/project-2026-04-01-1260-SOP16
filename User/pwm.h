#ifndef _PWM_H
#define _PWM_H

#include "my_config.h"
#include "include.h"
#include <stdio.h>

#define MAX_PWM_DUTY (6000) // 100%占空比   (SYSCLK 4800 0000 /  8000  == 6000)
enum
{
    PWM_DUTY_100_PERCENT = 6000, // 100%占空比
    PWM_DUTY_80_PERCENT = 4800,  // 80%
    PWM_DUTY_75_PERCENT = 4500,  // 75%占空比
    PWM_DUTY_50_PERCENT = 3000,  // 50%占空比
    PWM_DUTY_30_PERCENT = 1800,  // 30%占空比
    PWM_DUTY_25_PERCENT = 1500,  // 25%占空比
};
#define PWM_DUTY_X_PERCENT(x) ((u16)((u32)x * MAX_PWM_DUTY / 100))

// 定时，限制占空比的时间（用于上电多久之后，限制最大的占空比），单位：ms
#define SCHEDULE_TIME_TO_LIMIT_PWM ((u32)1 * 60 * 60 * 1000) // 1h
// #define SCHEDULE_TIME_TO_LIMIT_PWM ((u32)1 * 60 * 1000) // USER_TO_DO  1min，只在测试时使用
// 时间到来之后，要限制到的最大占空比值
#define SCHEDULE_TIME_TO_LIMIT_PWM_VAL ((u16)PWM_DUTY_X_PERCENT(80))
// 限制占空比的时间到来后，历时多久限制到期望的占空比值，单位：ms
#define SCHEDULE_TIME_TO_LIMIT_PWM_DURATION ((u32)20 * 60 * 1000) // 20 min
// #define SCHEDULE_TIME_TO_LIMIT_PWM_DURATION ((u32)2 * 60 * 1000) // USER_TO_DO  2 min，只在测试时使用
/*
    限制占空比的时间到来后，在调节时间内，每次的占空比调整的量

    例如：
    20min内，将占空比从100%调节至80%
    要调节占空比的数值绝对值 为 PWM_DUTY_X_PERCENT(100) - PWM_DUTY_X_PERCENT(80)
                           = 6000 - 4800 = 1200
    20min == 1200 000 ms
    只看数值，1200 000 ms 大于 1200(占空比绝对值)
    调节时间大于占空比绝对值，每经过一段时间才调节1单位的占空比值
    那么 20min 内，每调节1单位占空比值，需要经过的时长为：1200 000 ms / 1200 = 1000ms
    每 1000ms 调节1单位的占空比值

    @attention SCHEDULE_TIME_TO_LIMIT_PWM_DURATION 不能小于 SCHEDULE_TIME_TO_LIMIT_PWM_VAL
    @attention PWM_DUTY_X_PERCENT(100) - SCHEDULE_TIME_TO_LIMIT_PWM_VAL 不能小于等于0
*/
#define SCHEDULE_TIME_TO_LIMIT_PWM_ADJUST_UNIT_IN_DURATION \
    ((u16)((u32)SCHEDULE_TIME_TO_LIMIT_PWM_DURATION /      \
           (PWM_DUTY_X_PERCENT(100) - SCHEDULE_TIME_TO_LIMIT_PWM_VAL)))

// 由温度限制的PWM占空比 （对所有PWM通道都生效）
extern volatile u16 limited_pwm_duty_due_to_temp;
// 由于发动机不稳定，而限制的可以调节到的占空比（对所有PWM通道都生效，默认为最大占空比）
extern volatile u16 limited_pwm_duty_due_to_unstable_engine;
// 由于风扇异常，限制的可以调节到的最大占空比（对所有PWM通道都生效，默认为最大占空比）
extern volatile u16 limited_pwm_duty_due_to_fan_err;

extern volatile u16 cur_pwm_channel_0_duty;           // 当前设置的、 pwm_channle_0 的占空比
extern volatile u16 expect_adjust_pwm_channel_0_duty; // 存放期望调节到的 pwm_channle_0 占空比
extern volatile u16 adjust_pwm_channel_0_duty;        // pwm_channle_0 要调整到的占空比

extern volatile u16 cur_pwm_channel_1_duty;           // 当前设置的第二路PWN的占空比
extern volatile u16 expect_adjust_pwm_channel_1_duty; // 存放期望调节到的 pwm_channle_1 占空比
extern volatile u16 adjust_pwm_channel_1_duty;        // pwm_channle_1 要调整到的占空比

extern volatile u8 flag_is_time_to_limit_pwm;
extern volatile u16 limit_pwm_duty_val_by_time;

void pwm_init(void);

// 电源电压低于170V-AC,启动低压保护，电源电压高于170V-AC，关闭低压保护
void according_pin9_to_adjust_pwm(void);
// void according_pin9_to_adjust_pin16(void); // 根据9脚的电压来设定16脚的电平

extern u8 get_pwm_channel_0_status(void); // 获取第一路PWM的运行状态
extern u8 get_pwm_channel_1_status(void); // 获取第二路PWM的运行状态

extern void pwm_channel_0_enable(void);
extern void pwm_channel_0_disable(void);

extern void pwm_channel_1_enable(void);
extern void pwm_channel_1_disable(void);

void set_pwm_channel_0_duty(u16 channel_duty);
void set_pwm_channel_1_duty(u16 channel_duty);

u16 get_pwm_channel_x_adjust_duty(u16 pwm_adjust_duty);

#endif