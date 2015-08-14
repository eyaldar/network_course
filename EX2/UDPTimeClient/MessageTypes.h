#pragma once

#define AVAILABLE_REQUESTS_NUM 11

// Messages types
#define GET_TIME								"GetTime"
#define GET_TIME_WITHOUT_DATE					"GetTimeWithoutDate"
#define GET_TIME_SINCE_EPOCH					"GetTimeSinceEpoch"
#define GET_CLIENT_TO_SERVER_DELAY_ESTIMATION	"GetClientToServerDelayEstimation"
#define MEASURE_RTT								"MeasureRTT"
#define	GET_TIME_WITHOUT_DATE_OR_SECONDS		"GetTimeWithoutDateOrSeconds"
#define GET_YEAR								"GetYear"
#define GET_MONTH_AND_DAY						"GetMonthAndDay"
#define GET_SECONDS_SINCE_BEGINING_OF_MONTH		"GetSecondsSinceBeginingOfMonth"
#define GET_DAY_OF_YEAR							"GetDayOfYear"
#define GET_DAYLIGHT_SAVINGS					"GetDaylightSavings"

enum METHOD_TYPE {
	GetTime = 0,
	GetTimeWithoutDate,
	GetTimeSinceEpoch,
	GetClientToServerDelayEstimation,
	MeasureRTT,
	GetTimeWithoutDateOrSeconds,
	GetYear,
	GetMonthAndDay,
	GetSecondsSinceBeginingOfMonth,
	GetDayOfYear,
	GetDaylightSavings,
	Error
};

METHOD_TYPE get_method_type(char* methodType);
char* get_method_string(METHOD_TYPE method_type);