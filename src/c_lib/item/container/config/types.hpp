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
    char* name;

    int xdim, ydim;
    int alt_xdim, alt_ydim;
    bool attached_to_agent;

    decision_tree alpha_action;
    decision_tree beta_action;
    send_decision alpha_packet;
    send_decision beta_packet;
    
    // alt actions (synth purchase, craft output etc)
    decision_tree alpha_action_alt;
    decision_tree beta_action_alt;
    send_decision alpha_packet_alt;
    send_decision beta_packet_alt;

    void set_name(const char* name)
    {
        size_t len = strlen(name);
        this->name = (char*)malloc((len+1)*sizeof(char));
        strcpy(this->name, name);
    }

    int max_dim()
    {
        return this->xdim * this->ydim;
    }
    
    int max_alt_dim()
    {
        return this->alt_xdim * this->alt_ydim;
    }
    
    ContainerAttributes()
    {
        this->init();
    }

    ~ContainerAttributes()
    {
        if (this->name != NULL) free(this->name);
    }
    
    void init()
    {
        this->loaded = false;
        this->type = CONTAINER_TYPE_NONE;
        this->name = NULL;
        this->xdim = 0;
        this->ydim = 0;
        this->alt_xdim = 0;
        this->alt_ydim = 0;
        this->attached_to_agent = false;        
        this->alpha_action = NULL;
        this->beta_action = NULL;
        this->alpha_packet = NULL;
        this->beta_packet = NULL;
        this->alpha_action_alt = NULL;
        this->beta_action_alt = NULL;
        this->alpha_packet_alt = NULL;
        this->beta_packet_alt = NULL;
    }  
};
