#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <net_lib/global.hpp>
#include <net_lib/server.hpp>

class SubscriberList
{    
    private:
        // grow by doubling max, capped at HARD_MAX
        bool grow()
        {
            if (this->max >= hard_max) return false;
            this->max *= 2;
            if (this->max > hard_max) this->max = hard_max;
            this->subscribers = (ClientID*)realloc(this->subscribers, this->max * sizeof(ClientID));
            return true;
        }

        // grows to a specified size
        bool grow(unsigned int new_max)
        {
            GS_ASSERT(new_max > this->max);
            if (new_max <= this->max) return false;
            GS_ASSERT(new_max <= hard_max);
            if (new_max > hard_max) return false;
            
            this->max = new_max;
            this->subscribers = (ClientID*)realloc(this->subscribers, this->max * sizeof(ClientID));
            return true;
        }
        
    public:
        ClientID* subscribers;
        unsigned int n;
        unsigned int max;
        unsigned int hard_max;
        
    // returns true is subscriber is added to the list
    // if subscriber was already in the list, or if the list is maxed out, returns false
    bool add(ClientID client_id)
    {
        ASSERT_VALID_CLIENT_ID(client_id);
        IF_INVALID_CLIENT_ID(client_id) return false;
        
        // no duplicates
        for (unsigned int i=0; i<this->n; i++)
            if (this->subscribers[i] == client_id)
                return false;

        // resize
        if (this->n >= this->max && !this->grow()) return false;
        
        this->subscribers[this->n++] = client_id;
        return true;
    }

    // returns true if subscriber was found in list
    bool remove(ClientID client_id)
    {
        ASSERT_VALID_CLIENT_ID(client_id);
        IF_INVALID_CLIENT_ID(client_id) return false;

        for (unsigned int i=0; i<this->n; i++)
            if (this->subscribers[i] == client_id)
            {
                if (i < this->n-1) // swap with highest, decrement n
                    this->subscribers[i] = this->subscribers[this->n];
                this->n--;
                return true;
            }
        return false;
    }

    void remove_all()
    {
        this->n = 0;
    }

    SubscriberList(unsigned int initial_size)
    : n(0), max(initial_size)
    {
        GS_ASSERT(initial_size > 0);
        this->subscribers = (ClientID*)malloc(initial_size * sizeof(ClientID));
        this->hard_max = HARD_MAX_CONNECTIONS;
        GS_ASSERT(initial_size < this->hard_max);
    }

    SubscriberList(unsigned int initial_size, unsigned int hard_max)
    : n(0), max(initial_size)
    {
        GS_ASSERT(initial_size > 0);
        GS_ASSERT(hard_max > 0);
        GS_ASSERT(hard_max >= initial_size);
        this->subscribers = (ClientID*)malloc(initial_size * sizeof(ClientID));
        this->hard_max = hard_max;
    }

    ~SubscriberList()
    {
        if (this->subscribers != NULL) free(this->subscribers);
    }
};
