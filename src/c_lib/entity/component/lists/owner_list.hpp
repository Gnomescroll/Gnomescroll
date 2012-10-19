#pragma once

#include <entity/component/component_list.hpp>

namespace Components
{

/* Owner */
const int MAX_OWNER_COMPONENTS = 4096;

class OwnerComponentList: public ComponentList<OwnerComponent, COMPONENT_OWNER, MAX_OWNER_COMPONENTS>
{
    public:
        void transfer(AgentID owner_id, AgentID new_owner_id);

        #if DC_SERVER
        void revoke(AgentID owner_id);
        #endif

    ~OwnerComponentList(){}
    OwnerComponentList(){}
};

} // Components

