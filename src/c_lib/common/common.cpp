#include "common.hpp"

#include <stdlib.h>
#include <ctype.h>
#include <time.h>

void address_from_string(char* ip, int address[4])
{   // parse IP address string

    int address_index = 0;

    char c;
    int i=0, j=0;
    char tmp[3+1];
    while ((c = ip[i++]) != '\0')
    {
        if (c == '.')
        {
            tmp[j] = '\0';
            address[address_index++] = atoi(tmp);
            j = 0;
            continue;
        }
        tmp[j++] = c;
    }
    tmp[j] = '\0';
    address[address_index] = atoi(tmp);

}

void address_from_uint32(uint32_t ip, uint8_t address[4])
{
    for (int i=0; i<4; i++)
        address[i] = (uint8_t) (ip >> i*8) & 0xFF;
}

static const char time_fmt[] = "%Y-%m-%d_%H:%M:%S";

static const int TIME_STR_LEN = 4 + 2 + 2 + 2 + 2 + 2 + 2 + 1 + 2 + 1;    // Y,m,d,H,M,S,-,_,:,\0
static char time_str[TIME_STR_LEN+1];
char* get_time_str()
{
    // get time data
    time_t now;
    time(&now);
    const struct tm* time_info = localtime(&now);

    // setup time formatter
    strftime(time_str, TIME_STR_LEN, time_fmt, time_info);
    return time_str;
}

time_t atott(const char* str)
{
    size_t s = sizeof(time_t);
    if (s == sizeof(int))
        return atoi(str);
    else
    if (s == sizeof(long))
        return atol(str);
    else
    if (s == sizeof(long long))
	{
		//return NULL;	
		//return atoll(str);
	}
    GS_ASSERT_LIMIT(false, 1);
    return 0;
}

time_t utc_now()
{
    time_t now;
    time(&now); // get current unix time
    return now;
}
