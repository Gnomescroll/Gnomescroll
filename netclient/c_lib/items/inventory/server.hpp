#pragma once

#if DC_SERVER

#include <c_lib/items/inventory/base.hpp>

class ServerInventory: public BaseInventory
{
    public:
    ServerInventory(int id)
    : BaseInventory(id) {}
};

#endif
