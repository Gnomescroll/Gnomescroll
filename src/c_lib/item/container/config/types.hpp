#pragma once

#include <item/common/enum.hpp>

typedef void (*send_decision) (ContainerActionType, int, int);
                                 // action, container, slot
#if DC_CLIENT
typedef ContainerActionType (*decision_tree) (int, int);
#endif                                 // container, slot
#if DC_SERVER
typedef ContainerActionType (*decision_tree) (int, int, int, int);
#endif                              // agent, client, container, slot


class ContainerAttributes
{
    public:
    
    bool loaded;

    ItemContainerType type;

    int xdim,ydim;
    //int xdimb,ydimb;
    bool attached_to_agent;

    decision_tree alpha_action;
    decision_tree beta_action;
    send_decision alpha_packet;
    send_decision beta_packet;

    int max_dim()
    {
        return this->xdim * this->ydim;
    }
    
    ContainerAttributes()
    {
        this->init();
    }
    
    void init()
    {
        this->loaded = false;
        this->type = CONTAINER_TYPE_NONE;
        this->xdim = 0;
        this->ydim = 0;
        this->attached_to_agent = false;        
        this->alpha_action = NULL;
        this->beta_action = NULL;
        this->alpha_packet = NULL;
        this->beta_packet = NULL;
    }  
};
