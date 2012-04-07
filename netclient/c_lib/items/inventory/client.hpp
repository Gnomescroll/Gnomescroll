#pragma once

#if DC_CLIENT

#include <c_lib/items/inventory/base.hpp>

class ClientInventory: public BaseInventory
{
    public:

    void attach_to_owner();

    ClientInventory(int id)
    : BaseInventory(id) {}
};

#endif
