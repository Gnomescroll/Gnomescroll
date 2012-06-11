#pragma once

#if DC_CLIENT
dont include this file in client
#endif

#include <net_lib/global.hpp>

class SubscriberList
{
    static const unsigned int SUBSCRIBER_LIST_HARD_MAX = NetServer::HARD_MAX_CONNECTIONS;
    
    private:
        // grow by doubling max, capped at HARD_MAX
        bool grow()
        {
            if (this->max >= SUBSCRIBER_LIST_HARD_MAX) return false;
            this->max *= 2;
            if (this->max > SUBSCRIBER_LIST_HARD_MAX) this->max = SUBSCRIBER_LIST_HARD_MAX;
            this->subscribers = (int*)realloc(this->subscribers, this->max * sizeof(int));
            return true;
        }

        // grows to a specified size
        bool grow(unsigned int new_max)
        {
            GS_ASSERT(new_max > this->max);
            if (new_max <= this->max) return false;
            GS_ASSERT(new_max <= SUBSCRIBER_LIST_HARD_MAX);
            if (new_max > SUBSCRIBER_LIST_HARD_MAX) return false;
            
            this->max = new_max;
            this->subscribers = (int*)realloc(this->subscribers, this->max * sizeof(int));
            return true;
        }
        
    public:
        int* subscribers;
        unsigned int n;
        unsigned int max;

    // returns true is subscriber is added to the list
    // if subscriber was already in the list, or if the list is maxed out, returns false
    bool add(int client_id)
    {
        ASSERT_VALID_CLIENT_ID(client_id);
        // no duplicates
        for (unsigned int i=0; i<this->n; i++)
            if (this->subscribers[i] == client_id)
                return false;

        // resize
        if (this->n >= this->max && !this->grow()) return false;
        
        this->subscribers[this->n++] = client_id;
        return true;
    }

    // fills the subscriber list with all possible values
    void add_all()
    {
        if (this->max < SUBSCRIBER_LIST_HARD_MAX) this->grow(SUBSCRIBER_LIST_HARD_MAX);

        this->n = this->max;
        // no duplicates
        for (unsigned int i=0; i<this->n; i++)
            this->subscribers[i] = (int)i;
    }

    // returns true if subscriber was found in list
    bool remove(int client_id)
    {
        ASSERT_VALID_CLIENT_ID(client_id);
        for (unsigned int i=0; i<this->n; i++)
            if (this->subscribers[i] == client_id)
            {   // swap with highest, decrement n
                this->subscribers[i] = this->subscribers[this->n--];
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
        this->subscribers = (int*)malloc(initial_size * sizeof(int));
    }

    ~SubscriberList()
    {
        if (this->subscribers != NULL) free(this->subscribers);
    }
};
