#include "MessageTypes.h"

#include <string.h>

char *accepted_requests[] = {
	GET_TIME,
	GET_TIME_WITHOUT_DATE,
	GET_TIME_SINCE_EPOCH,
	GET_CLIENT_TO_SERVER_DELAY_ESTIMATION,
	MEASURE_RTT,
	GET_TIME_WITHOUT_DATE_OR_SECONDS,
	GET_YEAR,
	GET_MONTH_AND_DAY,
	GET_SECONDS_SINCE_BEGINING_OF_MONTH,
	GET_DAY_OF_YEAR,
	GET_DAYLIGHT_SAVINGS,
};

METHOD_TYPE get_method_type(char* request)
{
	int i;
	int error = 12;

	for (i = 0; i < AVAILABLE_REQUESTS_NUM; i++)
	{
		if (!strcmp(request, accepted_requests[i]))
		{
			return (METHOD_TYPE)i;
		}
	}

	return (METHOD_TYPE)error;
}

char* get_method_string(METHOD_TYPE method_type)
{
	if (method_type > Error)
	{
		return 0;
	}

	return accepted_requests[method_type];
}