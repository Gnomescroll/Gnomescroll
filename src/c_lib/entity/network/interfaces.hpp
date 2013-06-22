#pragma once

#include <entity/network/packets.hpp>

namespace Entities
{

class Entity;   // forward decl

/* Network Delegates */

class CreatePacketDelegate
{
    public:
    virtual void sendToClient(const Entity* entity, ClientID client_id) const = 0;
    virtual void broadcast(const Entity* entity) const = 0;
    virtual ~CreatePacketDelegate(){}
};

class StatePacketDelegate
{
    public:
    virtual void sendToClient(const Entity* entity, ClientID client_id) const= 0;
    virtual void broadcast(const Entity* entity) const= 0;
    virtual ~StatePacketDelegate(){}
};


/* Null Entities */


class CreatePacketNone: public CreatePacketDelegate
{
    public:
    void sendToClient(const Entity* entity, ClientID client_id) const {}
    void broadcast(const Entity* entity) const {}
};

class StatePacketNone: public StatePacketDelegate
{
    public:
    void sendToClient(const Entity* entity, ClientID client_id) const {}
    void broadcast(const Entity* entity) const {}
};

/* Concrete Delegates */

class CreatePacket: public CreatePacketDelegate
{
    private:
    void message(const Entity* entity, entity_create_StoC* msg) const;

    public:
    void sendToClient(const Entity* entity, ClientID client_id) const
    {
        entity_create_StoC msg;
        this->message(entity, &msg);
        msg.sendToClient(client_id);
    }

    void broadcast(const Entity* entity) const
    {
        entity_create_StoC msg;
        this->message(entity, &msg);
        msg.broadcast();
    }
};

class CreatePacketOwner: public CreatePacketDelegate
{
    private:
    void message(const Entity* entity, entity_create_owner_StoC* msg) const;

    public:
    void sendToClient(const Entity* entity, ClientID client_id) const
    {
        entity_create_owner_StoC msg;
        this->message(entity, &msg);
        msg.sendToClient(client_id);
    }

    void broadcast(const Entity* entity) const
    {
        entity_create_owner_StoC msg;
        this->message(entity, &msg);
        msg.broadcast();
    }
};

class CreatePacketMomentum: public CreatePacketDelegate
{
    private:
    void message(const Entity* entity, entity_create_momentum_StoC* msg) const;

    public:
    void sendToClient(const Entity* entity, ClientID client_id) const
    {
        entity_create_momentum_StoC msg;
        this->message(entity, &msg);
        msg.sendToClient(client_id);
    }

    void broadcast(const Entity* entity) const
    {
        entity_create_momentum_StoC msg;
        this->message(entity, &msg);
        msg.broadcast();
    }
};

class CreatePacketMomentumAngles: public CreatePacketDelegate
{
    private:
    void message(const Entity* entity, entity_create_momentum_angles_StoC* msg) const;

    public:
    void sendToClient(const Entity* entity, ClientID client_id) const
    {
        entity_create_momentum_angles_StoC msg;
        this->message(entity, &msg);
        msg.sendToClient(client_id);
    }

    void broadcast(const Entity* entity) const
    {
        entity_create_momentum_angles_StoC msg;
        this->message(entity, &msg);
        msg.broadcast();
    }
};

class CreatePacketMomentumAnglesHealth: public CreatePacketDelegate
{
    private:
    void message(const Entity* entity, entity_create_momentum_angles_health_StoC* msg) const;
    void health_message(const Entity* entity, entity_state_health_StoC* msg) const;

    public:
    void sendToClient(const Entity* entity, ClientID client_id) const
    {
        entity_create_momentum_angles_health_StoC msg;
        this->message(entity, &msg);
        msg.sendToClient(client_id);

        entity_state_health_StoC health_msg;
        this->health_message(entity, &health_msg);
        health_msg.sendToClient(client_id);
    }

    void broadcast(const Entity* entity) const
    {
        entity_create_momentum_angles_health_StoC msg;
        this->message(entity, &msg);
        msg.broadcast();

        entity_state_health_StoC health_msg;
        this->health_message(entity, &health_msg);
        health_msg.broadcast();
    }
};


/* State Packet */

class StatePacket: public StatePacketDelegate
{
    private:
    void message(const Entity* entity, entity_state_StoC* msg) const;

    public:
    void sendToClient(const Entity* entity, ClientID client_id) const
    {
        entity_state_StoC msg;
        this->message(entity, &msg);
        msg.sendToClient(client_id);
    }

    void broadcast(const Entity* entity) const
    {
        entity_state_StoC msg;
        this->message(entity, &msg);
        msg.broadcast();
    }
};

class StatePacketMomentum: public StatePacketDelegate
{
    private:
    void message(const Entity* entity, entity_state_momentum_StoC* msg) const;

    public:
    void sendToClient(const Entity* entity, ClientID client_id) const
    {
        entity_state_momentum_StoC msg;
        this->message(entity, &msg);
        msg.sendToClient(client_id);
    }

    void broadcast(const Entity* entity) const
    {
        entity_state_momentum_StoC msg;
        this->message(entity, &msg);
        msg.broadcast();
    }
};

class StatePacketMomentumAngles: public StatePacketDelegate
{
    private:
    void message(const Entity* entity, entity_state_momentum_angles_StoC* msg) const;

    public:
    void sendToClient(const Entity* entity, ClientID client_id) const
    {
        entity_state_momentum_angles_StoC msg;
        this->message(entity, &msg);
        msg.sendToClient(client_id);
    }

    void broadcast(const Entity* entity) const
    {
        entity_state_momentum_angles_StoC msg;
        this->message(entity, &msg);
        msg.broadcast();
    }
};

} // Entities
