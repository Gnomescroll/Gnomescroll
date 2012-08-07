#pragma once


void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE)
{
	static char t[256];
	static const char* prefix = "GS_ASSERT: ";

	//print_trace(); 
	//printf("GS_ASSERT: %s\n", assert_str);

	static const int len1 = strlen(prefix); //use size of
	int len2 = strlen(FILE);
	int len3 = strlen(FUNC);

	strncpy(t, prefix, len1);
	strncpy(t+len1, FILE, len2);
	t[t+len1+len2] = '\n';
	strncpy(t+len1+len2+1, FILE, len3);
	t[t+len1+len2+len3+1] = '\n';
	t[t+len1+len2+len3+2] = 0x00;

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
