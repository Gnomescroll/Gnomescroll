#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>

#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/cspray.hpp>

/****** fun with macros ******/

#define CREATE_THING(RETURN_TYPE, LIST, NAME, STATE)            \
    RETURN_TYPE C_create_##NAME() {                                 \
        return STATE::LIST.create();                      \
    }                                                      \

#define CREATE_THING_WITH_ID(RETURN_TYPE, LIST, NAME, STATE)    \
    RETURN_TYPE C_create_##NAME(int id) {                          \
        return STATE::LIST.create(id);                    \
    }                                                      \

#define GET_OR_CREATE_THING(RETURN_TYPE, LIST, NAME, STATE)     \
    RETURN_TYPE C_get_or_create_##NAME(int id) {                    \
    printf("GET O CREATE %d\n", id);\
        RETURN_TYPE a = STATE::LIST.get(id);                    \
        if (a==NULL) {                                    \
        printf("GET FAIL:: CREATING\n");\
            a = STATE::LIST.create(id);                    \
        }                                                  \
        return a;                                         \
    }                                                      \

#define GET_THING(RETURN_TYPE, LIST, NAME, STATE)               \
    RETURN_TYPE C_get_##NAME(int id) {                              \
    printf("GET %d\n", id);\
        return STATE::LIST.get(id);                       \
    }                                                      \

#define DELETE_THING(RETURN_TYPE, LIST, NAME, STATE)            \
    void C_delete_##NAME(int id) {                            \
        STATE::LIST.destroy(id);                           \
    }                                                       \

#define DRAW_THING(RETURN_TYPE, LIST, NAME, STATE)  \
    void C_draw_##NAME##s() {                            \
        STATE::LIST.draw();                         \
    }                                               \

#define FULL_WRAPPER(RETURN_TYPE, LIST, NAME, STATE) \
    CREATE_THING(RETURN_TYPE, LIST, NAME, STATE)   \
    CREATE_THING_WITH_ID(RETURN_TYPE, LIST, NAME, STATE)\
    GET_THING(RETURN_TYPE, LIST, NAME, STATE)\
    GET_OR_CREATE_THING(RETURN_TYPE, LIST, NAME, STATE)\
    DELETE_THING(RETURN_TYPE, LIST, NAME, STATE)\
    DRAW_THING(RETURN_TYPE, LIST, NAME, STATE)\

#ifdef DC_CLIENT
    #define STATE ClientState
#else
    #define STATE ServerState
#endif


/* Creates C methods wrapping *List CRUD operations
 * to be imported by Cython
 */
FULL_WRAPPER(Agent_state*, agent_list, agent, STATE)
FULL_WRAPPER(Cspray*, cspray_list, cspray, STATE)
//FULL_WRAPPER(Grenade*, grenade_list, grenade, STATE)
