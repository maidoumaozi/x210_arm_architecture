#ifndef _PWM_H_
#define _PWM_H_

void pwm_init(void);
void set_frequency(unsigned int fre);
void start_beep(void);
void stop_beep(void);
void timer_init(void);
void update_timer(void);

#endif

