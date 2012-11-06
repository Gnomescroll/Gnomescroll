#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <net_lib/global.hpp>
#include <net_lib/server.hpp>

template<typename T>
class SubscriberList
{    
    private:
        // grow by doubling max, capped at HARD_MAX
        bool grow()
        {
            if (this->max >= hard_max) return false;
            this->max *= 2;
            if (this->max > hard_max) this->max = hard_max;
            this->subscribers = (T*)realloc(this->subscribers, this->max * sizeof(T));
            return true;
        }

        // grows to a specified size
        bool grow(size_t new_max)
        {
            GS_ASSERT(new_max > this->max);
            if (new_max <= this->max) return false;
            GS_ASSERT(new_max <= hard_max);
            if (new_max > hard_max) return false;
            
            this->max = new_max;
            this->subscribers = (T*)realloc(this->subscribers, this->max * sizeof(T));
            return true;
        }
        
    public:
        T* subscribers;
        size_t count;
        size_t max;
        size_t hard_max;
        
    // returns true is subscriber is added to the list
    // if subscriber was already in the list, or if the list is maxed out, returns false
    bool add(T id)
    {
        // no duplicates
        for (size_t i=0; i<this->count; i++)
            if (this->subscribers[i] == id)
                return false;

        // resize
        if (this->count >= this->max && !this->grow()) return false;
        
        this->subscribers[this->count++] = id;
        return true;
    }

    // returns true if subscriber was found in list
    bool remove(T id)
    {
        for (size_t i=0; i<this->count; i++)
            if (this->subscribers[i] == id)
            {
                if (i < this->count-1) // swap with highest, decrement count
                    this->subscribers[i] = this->subscribers[this->count];
                this->count--;
                return true;
            }
        return false;
    }

    void remove_all()
    {
        this->count = 0;
    }

    SubscriberList<T>(size_t initial_size, size_t hard_max) :
        count(0), max(initial_size), hard_max(hard_max)
    {
        GS_ASSERT(initial_size > 0);
        GS_ASSERT(hard_max > 0);
        GS_ASSERT(hard_max >= initial_size);
        this->subscribers = (T*)malloc(initial_size * sizeof(T));
    }

    ~SubscriberList<T>()
    {
        if (this->subscribers != NULL) free(this->subscribers);
    }
};
