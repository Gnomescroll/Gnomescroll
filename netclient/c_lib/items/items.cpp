#include "items.hpp"




void Flag_list::draw() {
    int i;
    for (i=0; i<N_FLAGS; i++) {
        if (a[i]==NULL) continue;
        a[i]->draw();
    }
}
