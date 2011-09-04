

static inline void set_flag(unsigned int* _flag, int flag, int value) {
    if(value) {
        _flag = *_flag | flag;
    } else {
        _flag = *_flag & ~flag;
    }
}

static inline int get_flag(unsigned int* _flag, int flag) {
    return *_flag & flag
}





struct control_state_local {
    unsigned int state;
    //int is_set;
    //int tick;
};

int control_state_counter=0;
struct control_state_local control_history[32];


//forward,backwards,left,right,jetpack,fire
void set_control_state(int* state) {

    control_state_counter++;
    struct control_state_local* cs;
    cs = control_history[control_state_counter%32];

    cs->is_set=1;
    set_flag(&cs->state, 1, state[0]);
    set_flag(&cs->state, 2, state[1]);
    set_flag(&cs->state, 4, state[2]);
    set_flag(&cs->state, 8, state[3]);
    set_flag(&cs->state, 16, state[4]);
    set_flag(&cs->state, 32, state[5]);
    set_flag(&cs->state, 64, state[6]);
}
