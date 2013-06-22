#pragma once

#define GET_COMPONENT(COMPONENT, OBJECT) \
    (Components::COMPONENT##Component*)OBJECT->get_component(COMPONENT_##COMPONENT);

// auto c = GET_COMPONENT(VoxelModel, entity);

#define GET_COMPONENT_INTERFACE(COMPONENT, OBJECT) \
    (Components::COMPONENT##Component*)OBJECT->get_component_interface(COMPONENT_INTERFACE_##COMPONENT);

// auto c = GET_COMPONENT_INTERFACE(Physics, entity);

#define ADD_COMPONENT(COMPONENT) \
    (Components::COMPONENT##Component*)entity_data->attach_component(type, COMPONENT_##COMPONENT);

// auto c = ADD_COMPONENT(SpawnChild);
// Note: expects to be calling in load_data() initializer function. variable "EntityType type" should be in scope
