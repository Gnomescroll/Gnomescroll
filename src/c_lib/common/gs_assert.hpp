#pragma once


char* _internal_itoa(int val, int base)
{	
	static char buf[32] = {0};	
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}

void _push_str(char* tstr, int* index, char* istr)
{
	int len = strlen(istr);
	strncpy(tstr+*index, istr, len);
	*index += len;
}

void _push_str(char* tstr, int* index, const char* const_str)
{	
	int len = strlen(const_str);
	strncpy(tstr+*index, const_str, len);
	*index += len;
}

void _push_char(char* tstr, int* index, char c)
{
	tstr[*index] = c;
	(*index)++;
}

void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE)
{
	static char t[256];

	int index = 0;
	char* LINE_STR = _internal_itoa(LINE, 10);

	_push_str(t,&index, "GS_ASSERT: ");
	_push_str(t,&index, FILE);
	_push_str(t,&index, " ");
	_push_str(t,&index, FUNC);
	_push_str(t,&index, " ");
	_push_str(t,&index, LINE_STR);
	_push_char(t,&index, '\n');

	printf("%s", t);
}



   

#define GS_ASSERT(condition) if(!(condition)) _GS_ASSERT_INTERNAL( __FILE__, __FUNCTION__, __LINE__);


//printf( __FILE__  );



/*
#define GS_ASSERT_LIMIT(COND, LIMIT) \
if(!(COND)) { \
    static int ct = 0; \
    if (ct++ < (LIMIT)) \
        {\
            print_trace();printf("GS_ASSERT error: %s, line %d function: %s \n", __FILE__ " " #__LINE__ " " #__FUNCTION__ );\
            if (ct == (LIMIT)) printf("GS_ASSERT_LIMIT reached max: %s, line %d, function: %s\n", __FILE__, __LINE__, __FUNCTION__);\
        }\
}
*/

/*
#if PRODUCTION
#define GS_ASSERT(condition) GS_ASSERT_LIMIT(condition, 15)
#else
#define GS_ASSERT(condition) if(! (condition) ) { print_trace(); printf("GS_ASSERT error: %s, line %d function: %s \n", __FILE__, __LINE__, __FUNCTION__); }
#endif
*/

#define GS_ASSERT_LIMIT(CONDITION, LIMIT) GS_ASSERT(CONDITION)

//#define GS_ASSERT(condition) if(! (condition) )  
//_GS_ASSERT_INTERNAL(__FILE__, __LINE__, __FUNCTION__); }




#if defined ( __MSVC__ )
#define __func__ __FUNCTION__
#endif
