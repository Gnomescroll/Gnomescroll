#pragma once

#include <stdint.h>
#include <physics/vec3i.hpp>
#include <t_map/common/constants.hpp>


/*
 *  A Hash Table for storing map nodes (Vec3i's)
 *  Supports add, remove and contains
 */

#pragma once


class Bucket
{
    public:

    int pop(const struct Vec3i& point)
    {
        for (size_t i=0; i<this->size; i++)
            if (is_equal(this->points[i], point))
            {
                this->size -= 1;
                int value = this->values[i];
                if (i != this->size)
                {   // swap end of array with current
                    this->points[i] = this->points[this->size];
                    this->values[i] = this->values[this->size];
                }
                return value;
            }
        return -1;
    }

    void set(const struct Vec3i& point, int value)
    {
        size_t index = this->_get_index(point);
        if (index == this->NULL_SIZE)
        {   // Point wasn't found, grow if needed and use the last point as
            // the index
            if (this->size >= this->max && !this->_grow())
                return;
            index = this->size++;
        }
        this->points[index] = point;
        this->values[index] = value;
    }

    int get(const struct Vec3i& point)
    {
        for (size_t i=0; i<this->size; i++)
            if (is_equal(this->points[i], point))
                return this->values[i];
        return -1;
    }

    bool contains(const struct Vec3i& point)
    {
        return (this->get(point) >= 0);
    }

    void erase()
    {
        this->size = 0;
    }

    Bucket() :
        size(0), max(Bucket::START_MAX)
    {
        this->points = (struct Vec3i*)malloc(this->START_MAX * sizeof(*this->points));
        this->values = (int*)malloc(this->START_MAX * sizeof(*this->values));
    }

    ~Bucket()
    {
        free(this->points);
        free(this->values);
    }

    private:
        const static size_t START_MAX = 4;
        const static size_t NULL_SIZE = (size_t) - 1;
        size_t size;
        size_t max;
        struct Vec3i* points;
        int* values;

    bool _grow()
    {   // returns true if grew, else false on failure
        struct Vec3i* _points = (struct Vec3i*)realloc(this->points, 2 * this->max * sizeof(*this->points));
        IF_ASSERT(_points == NULL) return false;
        this->points = _points;

        int* _values = (int*)realloc(this->values, 2 * this->max * sizeof(*this->values));
        IF_ASSERT(_values == NULL) return false;
        this->values = _values;

        this->max *= 2;
        return true;
    }

    size_t _get_index(const struct Vec3i& point)
    {
        for (size_t i=0; i<this->size; i++)
            if (is_equal(this->points[i], point))
                return i;
        return this->NULL_SIZE;
    }

};

class HashMap
{
    public:

    int pop(const struct Vec3i& point)
    {
        int h = this->hash(point);
        return this->buckets[h].pop(point);
    }

    void set(const struct Vec3i& point, int value)
    {
        int h = this->hash(point);
        this->buckets[h].set(point, value);
    }

    int get(const struct Vec3i& point)
    {
        int h = this->hash(point);
        return this->buckets[h].get(point);
    }

    bool contains(const struct Vec3i& point)
    {
        int h = this->hash(point);
        return this->buckets[h].contains(point);
    }

    void erase()
    {
        for (size_t i=0; i<this->SIZE; i++)
            this->buckets[i].erase();
    }

    HashMap()
    {
        this->buckets = new Bucket[this->SIZE];
    }

    ~HashMap()
    {
        delete[] this->buckets;
    }

    int hash(const struct Vec3i& point)
    {   // Adapted from: http://stackoverflow.com/a/12996028/2253921
        int n = (map_dim.x * (map_dim.y * point.z + point.y)) + point.x;
        n = ((n >> 16) ^ n) * 0x45D9F3B;
        n = ((n >> 16) ^ n) * 0x45D9F3B;
        n = ((n >> 16) ^ n);
        return n % this->SIZE;
    }

    private:
        const static size_t SIZE = 0xFFFF;
        Bucket* buckets;
};
