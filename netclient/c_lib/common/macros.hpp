#pragma once

#define ASSERT_VALID_AGENT_ID(agent_id) assert(agent_id >= 0 && agent_id < AGENT_MAX)

//GS_ABORT()

//syntax: GS_ABORT();
#define GS_ABORT printf("Call to abort() in %s:%d %s\n", __FILE__, __LINE__, __FUNCTION__); abort


//GS_ASSERT()

#define GS_ASSERT_DISABLE 1

#if GS_ASSERT_DISABLE
  #define GS_ASSERT(conditition) ;
#else
  #define GS_ASSERT(conditition) \
  if(conditition ) \
  fprintf (stderr, "GS_ASSERT error: %s, line %d \n", __FILE__, __LINE__);
#endif
