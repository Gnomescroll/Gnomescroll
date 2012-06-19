#pragma once

#include <entity/network/packets.hpp>

namespace Objects
{

//forward decl
class Object;

/* Network Delegates */

class CreatePacketDelegate
{
    public:
        virtual void sendToClient(Object* object, int client_id) = 0;
        virtual void broadcast(Object* object) = 0;
        virtual ~CreatePacketDelegate(){}
};

class StatePacketDelegate
{
    public:
        virtual void sendToClient(Object* object, int client_id) = 0;
        virtual void broadcast(Object* object) = 0;
        virtual ~StatePacketDelegate(){}
};


/* Null Objects */


class CreatePacketNone: public CreatePacketDelegate
{
    public:
        void sendToClient(Object* object, int client_id) {}
        void broadcast(Object* object) {}
};

class StatePacketNone: public StatePacketDelegate
{
    public:
        void sendToClient(Object* object, int client_id) {}
        void broadcast(Object* object) {}
};

/* Concrete Delegates */

class CreatePacket: public CreatePacketDelegate
{
    private:
        void message(Object* object, object_create_StoC* msg);
    
    public:
        void sendToClient(Object* object, int client_id)
        {
            object_create_StoC msg;
            this->message(object, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* object)
        {
            object_create_StoC msg;
            this->message(object, &msg);
            msg.broadcast();
        }
};

class CreatePacketOwner: public CreatePacketDelegate
{
    private:
        void message(Object* object, object_create_owner_StoC* msg);
    
    public:
        void sendToClient(Object* object, int client_id)
        {
            object_create_owner_StoC msg;
            this->message(object, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* object)
        {
            object_create_owner_StoC msg;
            this->message(object, &msg);
            msg.broadcast();
        }
};

class CreatePacketMomentum: public CreatePacketDelegate
{
    private:
        void message(Object* object, object_create_momentum_StoC* msg);
    
    public:
        void sendToClient(Object* object, int client_id)
        {
            object_create_momentum_StoC msg;
            this->message(object, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* object)
        {
            object_create_momentum_StoC msg;
            this->message(object, &msg);
            msg.broadcast();
        }
};

class CreatePacketMomentumAngles: public CreatePacketDelegate
{
    private:
        void message(Object* object, object_create_momentum_angles_StoC* msg);
    
    public:
        void sendToClient(Object* object, int client_id)
        {
            object_create_momentum_angles_StoC msg;
            this->message(object, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* object)
        {
            object_create_momentum_angles_StoC msg;
            this->message(object, &msg);
            msg.broadcast();
        }
};

class CreatePacketMomentumAnglesHealth: public CreatePacketDelegate
{
    private:
        void message(Object* object, object_create_momentum_angles_health_StoC* msg);
        void health_message(Object* object, object_state_health_StoC* msg);
    
    public:
        void sendToClient(Object* object, int client_id)
        {
            object_create_momentum_angles_health_StoC msg;
            this->message(object, &msg);
            msg.sendToClient(client_id);

            object_state_health_StoC health_msg;
            this->health_message(object, &health_msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* object)
        {
            object_create_momentum_angles_health_StoC msg;
            this->message(object, &msg);
            msg.broadcast();
            
            object_state_health_StoC health_msg;
            this->health_message(object, &health_msg);
            msg.broadcast();
        }
};


/* State Packet */

class StatePacket: public StatePacketDelegate
{
    private:
        void message(Object* object, object_state_StoC* msg);
    
    public:
        void sendToClient(Object* object, int client_id)
        {
            object_state_StoC msg;
            this->message(object, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* object)
        {
            object_state_StoC msg;
            this->message(object, &msg);
            msg.broadcast();
        }
};

class StatePacketMomentum: public StatePacketDelegate
{
    private:
        void message(Object* object, object_state_momentum_StoC* msg);
    
    public:
        void sendToClient(Object* object, int client_id)
        {
            object_state_momentum_StoC msg;
            this->message(object, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* object)
        {
            object_state_momentum_StoC msg;
            this->message(object, &msg);
            msg.broadcast();
        }
};

class StatePacketMomentumAngles: public StatePacketDelegate
{
    private:
        void message(Object* object, object_state_momentum_angles_StoC* msg);
    
    public:
        void sendToClient(Object* object, int client_id)
        {
            object_state_momentum_angles_StoC msg;
            this->message(object, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* object)
        {
            object_state_momentum_angles_StoC msg;
            this->message(object, &msg);
            msg.broadcast();
        }
};


} // Objects
