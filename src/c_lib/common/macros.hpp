#pragma once

#define ASSERT_VALID_AGENT_ID(agent_id) GS_ASSERT(agent_id >= 0 && agent_id < AGENT_MAX)

#define GS_ABORT() do {\
printf("GS_ABORT error: %s, line %d function: %s \n", __FILE__, __LINE__, __FUNCTION__);\
print_trace();\
exit(1);\
}while(0)

#define GS_ASSERT(conditition) \
if(! (conditition) ) { print_trace(); printf("GS_ASSERT error: %s, line %d function: %s \n", __FILE__, __LINE__, __FUNCTION__); }
