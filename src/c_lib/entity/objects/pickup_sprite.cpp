//#include "pickup_sprite.hpp"

//#include <entity/constants.hpp>
//#include <entity/object/object.hpp>
//#include <entity/components/physics/verlet.hpp>
//#if DC_CLIENT
//#include <entity/components/draw/billboard_sprite.hpp>
//#endif

//namespace Objects
//{

//void load_pickup_sprite_data(ObjectType type)
//{
    //#if DC_SERVER
    //const int n_components = 4;
    //#endif
    //#if DC_CLIENT
    //const int n_components = 5;
    //#endif

    //object_data->set_components(type, n_components);

    //object_data->attach_component(type, COMPONENT_VERLET);
    //object_data->attach_component(type, COMPONENT_PICKUP);
    //object_data->attach_component(type, COMPONENT_TTL);
    //object_data->attach_component(type, COMPONENT_STACKABLE);

    //#if DC_CLIENT
    //object_data->attach_component(type, COMPONENT_BILLBOARD_SPRITE);
    //#endif
//}

////// private
////static void set_pickup_sprite_properties(Object* object)
////{   // attach components

    ////#if DC_CLIENT
    ////using Components::BillboardSpriteComponent;
    ////BillboardSpriteComponent* sprite = (BillboardSpriteComponent*)add_component_to_object(object, COMPONENT_BILLBOARD_SPRITE);
    ////sprite->sprite_index = JADE_GEMSTONE_SPRITE_INDEX;
    ////sprite->scale = PICKUP_SPRITE_SCALE;
    ////#endif

    ////using Components::VerletPhysicsComponent;
    ////using Components::PickupComponent;
    ////using Components::TTLHealthComponent;
    ////using Components::StackableComponent;
    
    ////VerletPhysicsComponent* physics = (VerletPhysicsComponent*)add_component_to_object(object, COMPONENT_VERLET);
    ////physics->mass = GEMSTONE_MASS;
    ////physics->damp = GEMSTONE_DAMP;
    
    ////PickupComponent* pickup = (PickupComponent*)add_component_to_object(object, COMPONENT_PICKUP);
    ////pickup->pickup_radius = GEMSTONE_PICKUP_RADIUS;
    
    ////TTLHealthComponent* health = (TTLHealthComponent*)add_component_to_object(object, COMPONENT_TTL);
    ////health->ttl_max = GEMSTONE_TTL;

    ////StackableComponent* stack = (StackableComponent*)add_component_to_object(object, COMPONENT_STACKABLE);
    ////stack->max = GEMSTONE_STACK_MAX;

    ////object->tick = &tick_pickup_sprite;
    //////object->update = NULL;

    ////object->create = create_packet_momentum;
    ////object->state = state_packet_momentum;
////}

//Object* create_pickup_sprite(ObjectType type)
//{   // initialize object
    //Object* obj = object_list->create(type);
    //if (obj == NULL) return NULL;

    ///* TODO : A REAL DAT LOADER */

    //switch (type)
    //{
        //// refills
        ////case OBJECT_HEALTH_REFILL:
            ////create_health_refill(obj);
            ////break;
        ////case OBJECT_GRENADE_REFILL:
            ////create_grenade_refill(obj);
            ////break;
        ////case OBJECT_LASER_REFILL:
            ////create_laser_refill(obj);
            ////break;
///*
        //// gemstones
        //case OBJECT_GEMSTONE_MALACHITE:
            //create_malachite_gemstone(obj);
            //break;
        //case OBJECT_GEMSTONE_RUBY:
            //create_ruby_gemstone(obj);
            //break;
        //case OBJECT_GEMSTONE_TURQUOISE:
            //create_turquoise_gemstone(obj);
            //break;
        //case OBJECT_GEMSTONE_SILVER:
            //create_silver_gemstone(obj);
            //break;
        //case OBJECT_GEMSTONE_AMETHYST:
            //create_amethyst_gemstone(obj);
            //break;
        //case OBJECT_GEMSTONE_JADE:
            //create_jade_gemstone(obj);
            //break;
        //case OBJECT_GEMSTONE_ONYX:
            //create_onyx_gemstone(obj);
            //break;
//*/
        //default:
            //printf("create_pickup_sprite() -- type %d not handled\n", type);
            //GS_ASSERT(false);
            //break;
    //}
    //return obj;
//}

//void ready_pickup_sprite(Object* object)
//{
    //#if DC_SERVER
    //object->broadcastCreate();
    //#endif
//}

//void die_pickup_sprite(Object* object)
//{
    //#if DC_SERVER
    //using Components::PickupComponent;
    //PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    //if (pickup->picked_up) pickup->broadcast();
    //else object->broadcastDeath();
    //#endif
//}

//void tick_pickup_sprite(Object* object)
//{
    //using Components::VerletPhysicsComponent;
    //using Components::PickupComponent;
    //using Components::TTLHealthComponent;
    
    //// update for physics
    //VerletPhysicsComponent* verlet = (VerletPhysicsComponent*)object->get_component(COMPONENT_VERLET);
    //verlet->bounce();

    //#if DC_SERVER
    //PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    //pickup->tick();

    //TTLHealthComponent* ttl = (TTLHealthComponent*)object->get_component(COMPONENT_TTL);
    //ttl->tick();
    //#endif
//}

////void update_pickup_sprite(Object* object)
////{
    //// update for draw
////}

//} // Objects
