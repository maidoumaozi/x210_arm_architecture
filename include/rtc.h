#ifndef _RTC_H_
#define _RTC_H_

struct rtc_time_s
{
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int dayweek;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
};

void rtc_set_time(const struct rtc_time_s *rtc_time);
void rtc_get_time(struct rtc_time_s *rtc_time);
void rtc_alarm_init(void);
void rtc_time_tick_init(void);

#endif