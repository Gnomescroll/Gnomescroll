#pragma once

#include <net_lib/net.hpp>

namespace Item
{

class container_action_alpha_CtoS: public FixedSizeReliableNetPacketToServer<container_action_alpha_CtoS>
{
    public:
        uint16_t event_id;
        uint8_t action;

        // container/slot location
        uint16_t container_id;
        uint8_t slot;

        // expected slot values
        uint8_t slot_type;
        uint8_t slot_stack;

        // expected hand values
        uint8_t hand_stack;
        uint8_t hand_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);
            
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);

            pack_u8(&slot_type, buff, buff_n, pack);
            pack_u8(&slot_stack, buff, buff_n, pack);
            
            pack_u8(&hand_type, buff, buff_n, pack);
            pack_u8(&hand_stack, buff, buff_n, pack);
        }
        inline void handle();
};

class container_action_beta_CtoS: public FixedSizeReliableNetPacketToServer<container_action_beta_CtoS>
{
    public:
        uint16_t event_id;
        uint8_t action;

        // container/slot location
        uint16_t container_id;
        uint8_t slot;

        // expected slot values
        uint8_t slot_type;
        uint8_t slot_stack;

        // expected hand values
        uint8_t hand_stack;
        uint8_t hand_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);
            
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);

            pack_u8(&slot_type, buff, buff_n, pack);
            pack_u8(&slot_stack, buff, buff_n, pack);
            
            pack_u8(&hand_type, buff, buff_n, pack);
            pack_u8(&hand_stack, buff, buff_n, pack);
        }
        inline void handle();
};

class nanite_container_action_alpha_CtoS: public FixedSizeReliableNetPacketToServer<nanite_container_action_alpha_CtoS>
{
    public:
        uint16_t event_id;
        uint8_t action;

        // container/slot location
        uint16_t container_id;
        uint8_t slot;

        // expected slot values
        uint8_t slot_type;
        uint8_t slot_stack;

        // expected hand values
        uint8_t hand_stack;
        uint8_t hand_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);
            
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);

            pack_u8(&slot_type, buff, buff_n, pack);
            pack_u8(&slot_stack, buff, buff_n, pack);
            
            pack_u8(&hand_type, buff, buff_n, pack);
            pack_u8(&hand_stack, buff, buff_n, pack);
        }
        inline void handle();
};

class nanite_container_action_beta_CtoS: public FixedSizeReliableNetPacketToServer<nanite_container_action_beta_CtoS>
{
    public:
        uint16_t event_id;
        uint8_t action;

        // container/slot location
        uint16_t container_id;
        uint8_t slot;

        // expected slot values
        uint8_t slot_type;
        uint8_t slot_stack;

        // expected hand values
        uint8_t hand_stack;
        uint8_t hand_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);
            
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);

            pack_u8(&slot_type, buff, buff_n, pack);
            pack_u8(&slot_stack, buff, buff_n, pack);
            
            pack_u8(&hand_type, buff, buff_n, pack);
            pack_u8(&hand_stack, buff, buff_n, pack);
        }
        inline void handle();
};

class craft_container_action_alpha_CtoS: public FixedSizeReliableNetPacketToServer<craft_container_action_alpha_CtoS>
{
    public:
        uint16_t event_id;
        uint8_t action;

        // container/slot location
        uint16_t container_id;
        uint8_t slot;

        // expected slot values
        uint8_t slot_type;
        uint8_t slot_stack;

        // expected hand values
        uint8_t hand_stack;
        uint8_t hand_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);
            
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);

            pack_u8(&slot_type, buff, buff_n, pack);
            pack_u8(&slot_stack, buff, buff_n, pack);
            
            pack_u8(&hand_type, buff, buff_n, pack);
            pack_u8(&hand_stack, buff, buff_n, pack);
        }
        inline void handle();
};

class craft_container_action_beta_CtoS: public FixedSizeReliableNetPacketToServer<craft_container_action_beta_CtoS>
{
    public:
        uint16_t event_id;
        uint8_t action;

        // container/slot location
        uint16_t container_id;
        uint8_t slot;

        // expected slot values
        uint8_t slot_type;
        uint8_t slot_stack;

        // expected hand values
        uint8_t hand_stack;
        uint8_t hand_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);
            
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);

            pack_u8(&slot_type, buff, buff_n, pack);
            pack_u8(&slot_stack, buff, buff_n, pack);
            
            pack_u8(&hand_type, buff, buff_n, pack);
            pack_u8(&hand_stack, buff, buff_n, pack);
        }
        inline void handle();
};

class craft_item_from_bench_action_CtoS: public FixedSizeReliableNetPacketToServer<craft_item_from_bench_action_CtoS>
{
    public:
        uint16_t event_id;

        // container/slot location
        uint16_t container_id;
        uint8_t slot;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);
        }
        inline void handle();
};

class no_container_action_alpha_CtoS: public FixedSizeReliableNetPacketToServer<no_container_action_alpha_CtoS>
{
    public:
        uint16_t event_id;
        uint8_t action;

        // expected hand values
        uint8_t hand_stack;
        uint8_t hand_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);

            pack_u8(&hand_type, buff, buff_n, pack);
            pack_u8(&hand_stack, buff, buff_n, pack);            
        }
        inline void handle();
};

class no_container_action_beta_CtoS: public FixedSizeReliableNetPacketToServer<no_container_action_beta_CtoS>
{
    public:
        uint16_t event_id;
        uint8_t action;

        // expected hand values
        uint8_t hand_stack;
        uint8_t hand_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);
            
            pack_u8(&hand_type, buff, buff_n, pack);
            pack_u8(&hand_stack, buff, buff_n, pack);
        }
        inline void handle();
};

class open_container_CtoS: public FixedSizeReliableNetPacketToServer<open_container_CtoS>
{
    public:
        uint16_t container_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
        }
        inline void handle();
};

class close_container_CtoS: public FixedSizeReliableNetPacketToServer<close_container_CtoS>
{
    public:
        uint16_t container_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
        }
        inline void handle();
};

}
