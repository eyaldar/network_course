#include "MessageTypes.h"

#include <string.h>
#include <stdlib.h>

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
	int method_type;
	METHOD_TYPE error = Error;

	method_type = atoi(request);

	if(method_type >= Error)
		return error;

	return (METHOD_TYPE)method_type;
}

char* get_method_string(METHOD_TYPE method_type)
{
	if (method_type < Error) 
	{
		return accepted_requests[method_type];
	}

	return 0;
}