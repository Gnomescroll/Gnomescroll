//#pragma once


void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE)
{
	print_trace(); 
	printf("GS_ASSERT: %s\n", assert_str);
}



   

#define GS_ASSERT(condition) _GS_ASSERT_INTERNAL( __FILE__, __FUNCTION__, __LINE__);


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
