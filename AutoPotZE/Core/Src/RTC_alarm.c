/**
 * @file RTC_alarm.c
 * @brief Functions related to RTC and RTC alarm
 * @author Paweł Chalimoniuk
 */

#include "RTC_alarm.h"
uint8_t ALARM_RTC_get_weekday_from_macro(uint8_t weekday_macro);
uint8_t ALARM_RTC_get_weekday_to_macro(uint8_t weekday);
uint8_t ALARM_RTC_validate_date(uint8_t weekday, uint8_t hours, uint8_t minutes);
uint8_t ALARM_RTC_init_rtc(ALARM_RTC_struct * alarm_struct) {
	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef sDate = { 0 };
	RTC_HandleTypeDef temp = *alarm_struct->hrtc;
//	alarm_struct->hrtc->Instance = RTC;
//	alarm_struct->hrtc->Init.HourFormat = RTC_HOURFORMAT_24;
//	alarm_struct->hrtc->Init.AsynchPrediv = 127;
//	alarm_struct->hrtc->Init.SynchPrediv = 255;
//	alarm_struct->hrtc->Init.OutPut = RTC_OUTPUT_DISABLE;
//	alarm_struct->hrtc->Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
//	alarm_struct->hrtc->Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	temp.Instance = RTC;
	temp.Init.HourFormat = RTC_HOURFORMAT_24;
	temp.Init.AsynchPrediv = 127;
	temp.Init.SynchPrediv = 255;
	temp.Init.OutPut = RTC_OUTPUT_ALARMA;
	temp.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	temp.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	*(alarm_struct->hrtc) = temp;
	if (HAL_RTC_Init(alarm_struct->hrtc) != HAL_OK) {
		return 0;
	}

	/* Initialize RTC and set the Time and Date*/

	sTime.Hours = 9;
	sTime.Minutes = 59;
	sTime.Seconds = 30;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(alarm_struct->hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 1;
	sDate.Year = 0;

	if (HAL_RTC_SetDate(alarm_struct->hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	return 1;
}
uint8_t ALARM_RTC_set_rtc(ALARM_RTC_struct * alarm_struct){

	if (!ALARM_RTC_validate_date(alarm_struct->weekday, alarm_struct->hours, alarm_struct->minutes)){
		return 0;
	}
	uint8_t weekday_macro = ALARM_RTC_get_weekday_to_macro(alarm_struct->weekday);

	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef sDate = { 0 };

	sTime.Hours = alarm_struct->hours;
	sTime.Minutes = alarm_struct->minutes;
	sTime.Seconds = 0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sDate.WeekDay = weekday_macro;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 1;
	sDate.Year = 0;
	if (HAL_RTC_SetTime(alarm_struct->hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	if (HAL_RTC_SetDate(alarm_struct->hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	return 1;
}

uint8_t ALARM_RTC_get_weekday_to_macro(uint8_t weekday) {
	switch (weekday) {
	case 1:
		return RTC_WEEKDAY_MONDAY;
	case 2:
		return RTC_WEEKDAY_TUESDAY;
	case 3:
		return RTC_WEEKDAY_WEDNESDAY;
	case 4:
		return RTC_WEEKDAY_THURSDAY;
	case 5:
		return RTC_WEEKDAY_FRIDAY;
	case 6:
		return RTC_WEEKDAY_SATURDAY;
	case 7:
		return RTC_WEEKDAY_SUNDAY;
	default:
		return RTC_WEEKDAY_MONDAY;
	}
	return RTC_WEEKDAY_MONDAY;
}
uint8_t ALARM_RTC_validate_date(uint8_t weekday, uint8_t hours, uint8_t minutes){
	if (weekday < 1 || weekday > 8) {
		return 0;
	}
	if (hours < 1 || hours > 23) {
		return 0;
	}
	if (minutes > 59) {
		return 0;
	}
	return 1;
}
uint8_t ALARM_RTC_get_weekday_from_macro(uint8_t weekday_macro) {
	switch (weekday_macro) {
	case RTC_WEEKDAY_MONDAY:
		return 1;
	case RTC_WEEKDAY_TUESDAY:
		return 2;
	case RTC_WEEKDAY_WEDNESDAY:
		return 3;
	case RTC_WEEKDAY_THURSDAY:
		return 4;
	case RTC_WEEKDAY_FRIDAY:
		return 5;
	case RTC_WEEKDAY_SATURDAY:
		return 6;
	case RTC_WEEKDAY_SUNDAY:
		return 7;
	default:
		return 1;
	}
	return 1;
}
uint8_t ALARM_RTC_set_alarm(ALARM_RTC_struct * alarm_struct) {
	RTC_TimeTypeDef currentTime = {0};
	RTC_DateTypeDef currentDate = {0};
	if(alarm_struct->wateringPeriod > 8){
		return 0;
	}
	if (HAL_RTC_GetTime(alarm_struct->hrtc, &currentTime, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	if (HAL_RTC_GetDate(alarm_struct->hrtc, &currentDate, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	uint8_t shift = ALARM_RTC_get_weekday_from_macro(currentDate.WeekDay) + alarm_struct->wateringPeriod;
	if(shift > 8){
		shift = shift%7;
	}
	RTC_AlarmTypeDef sAlarm = {0};
	sAlarm.AlarmTime.Hours = alarm_struct->alarmTimeHour;
	sAlarm.AlarmTime.Minutes = 0;
	sAlarm.AlarmTime.Seconds = 0;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	sAlarm.AlarmDateWeekDay = shift;
	sAlarm.Alarm = alarm_struct->alarmTypeX;
	//__HAL_RTC_
	if (HAL_RTC_SetTime(alarm_struct->hrtc, &currentTime, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	if (HAL_RTC_SetDate(alarm_struct->hrtc, &currentDate, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	if (HAL_RTC_SetAlarm_IT(alarm_struct->hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
		return 0;
	}
	return 1;

}
