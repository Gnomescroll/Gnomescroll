#pragma once

#include <c_lib/entity/network/packets.hpp>

namespace Objects
{

//forward decl
class Object;

/* Network Delegates */

class CreatePacketDelegate
{
    public:
        virtual void sendToClient(Object* obj, int client_id) = 0;
        virtual void broadcast(Object* obj) = 0;
        virtual ~CreatePacketDelegate(){}
};

class StatePacketDelegate
{
    public:
        virtual void sendToClient(Object* obj, int client_id) = 0;
        virtual void broadcast(Object* obj) = 0;
        virtual ~StatePacketDelegate(){}
};


/* Null Objects */


class CreatePacketNone: public CreatePacketDelegate
{
    public:
        void sendToClient(Object* obj, int client_id) {}
        void broadcast(Object* obj) {}
};

class StatePacketNone: public StatePacketDelegate
{
    public:
        void sendToClient(Object* obj, int client_id) {}
        void broadcast(Object* obj) {}
};

/* Concrete Delegates */

class CreatePacket: public CreatePacketDelegate
{
    private:
        void message(Object* obj, object_create_StoC* msg);
    
    public:
        void sendToClient(Object* obj, int client_id)
        {
            object_create_StoC msg;
            this->message(obj, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* obj)
        {
            object_create_StoC msg;
            this->message(obj, &msg);
            msg.broadcast();
        }
};

class CreatePacketOwnerTeam: public CreatePacketDelegate
{
    private:
        void message(Object* obj, object_create_owner_team_StoC* msg);
    
    public:
        void sendToClient(Object* obj, int client_id)
        {
            object_create_owner_team_StoC msg;
            this->message(obj, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* obj)
        {
            object_create_owner_team_StoC msg;
            this->message(obj, &msg);
            msg.broadcast();
        }
};

class CreatePacketOwnerTeamIndex: public CreatePacketDelegate
{
    private:
        void message(Object* obj, object_create_owner_team_index_StoC* msg);
    
    public:
        void sendToClient(Object* obj, int client_id)
        {
            object_create_owner_team_index_StoC msg;
            this->message(obj, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* obj)
        {
            object_create_owner_team_index_StoC msg;
            this->message(obj, &msg);
            msg.broadcast();
        }
};

class CreatePacketMomentum: public CreatePacketDelegate
{
    private:
        void message(Object* obj, object_create_momentum_StoC* msg);
    
    public:
        void sendToClient(Object* obj, int client_id)
        {
            object_create_momentum_StoC msg;
            this->message(obj, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* obj)
        {
            object_create_momentum_StoC msg;
            this->message(obj, &msg);
            msg.broadcast();
        }
};

class CreatePacketMomentumAngles: public CreatePacketDelegate
{
    private:
        void message(Object* obj, object_create_momentum_angles_StoC* msg);
    
    public:
        void sendToClient(Object* obj, int client_id)
        {
            object_create_momentum_angles_StoC msg;
            this->message(obj, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* obj)
        {
            object_create_momentum_angles_StoC msg;
            this->message(obj, &msg);
            msg.broadcast();
        }
};


/* State Packet */

class StatePacket: public StatePacketDelegate
{
    private:
        void message(Object* obj, object_state_StoC* msg);
    
    public:
        void sendToClient(Object* obj, int client_id)
        {
            object_state_StoC msg;
            this->message(obj, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* obj)
        {
            object_state_StoC msg;
            this->message(obj, &msg);
            msg.broadcast();
        }
};

class StatePacketMomentum: public StatePacketDelegate
{
    private:
        void message(Object* obj, object_state_momentum_StoC* msg);
    
    public:
        void sendToClient(Object* obj, int client_id)
        {
            object_state_momentum_StoC msg;
            this->message(obj, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* obj)
        {
            object_state_momentum_StoC msg;
            this->message(obj, &msg);
            msg.broadcast();
        }
};

class StatePacketMomentumAngles: public StatePacketDelegate
{
    private:
        void message(Object* obj, object_state_momentum_angles_StoC* msg);
    
    public:
        void sendToClient(Object* obj, int client_id)
        {
            object_state_momentum_angles_StoC msg;
            this->message(obj, &msg);
            msg.sendToClient(client_id);
        }

        void broadcast(Object* obj)
        {
            object_state_momentum_angles_StoC msg;
            this->message(obj, &msg);
            msg.broadcast();
        }
};


} // Objects
