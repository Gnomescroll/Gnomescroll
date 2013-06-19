#pragma once

#define GET_COMPONENT(COMPONENT, OBJECT) \
    (Components::COMPONENT##Component*)OBJECT->get_component(COMPONENT_##COMPONENT);

// auto c = GET_COMPONENT(VoxelModel, object);

#define GET_COMPONENT_INTERFACE(COMPONENT, OBJECT) \
    (Components::COMPONENT##Component*)OBJECT->get_component_interface(COMPONENT_INTERFACE_##COMPONENT);

// auto c = GET_COMPONENT_INTERFACE(Physics, object);

#define ADD_COMPONENT(COMPONENT, OBJECT) \
    (Components::COMPONENT##Component*)add_component_to_object(OBJECT, COMPONENT_##COMPONENT);

// auto c = ADD_COMPONENT(SpawnChild, object);
