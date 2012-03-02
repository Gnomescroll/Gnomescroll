#include "ui_element.hpp"



struct UI_Element_struct
{
	int xdim;
	int ydim;

	int xpos;
	int ypos;

	void* draw_method;
};
/*
	Store list of UI elements (order)
	Store xdim,ydim dimensions
	Store xpos,ypos

*/

template <class Derived>
class UI_Element 
{
    public:

        static void _draw(void* pointer) 
        {
            Derived x* = (Derived x*) pointer;  //allocated on stack
            x->draw(xpos, ypos);
        }

        virtual void draw(int x, int y) = 0;;

        static uint8_t message_id;
        static int size;
        int client_id; //id of the UDP client who sent message

        //flatten this
        void serialize(char* buff, int* buff_n) __attribute((always_inline))
        {
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }
        //flatten this
        inline void unserialize(char* buff, int* buff_n, int* size) __attribute((always_inline))
        {
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() 
        {
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_unreliable_message(nm);
        }
        
        //will overflow if more than 128 bytes
        int Size() { char buff[128];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s+1;}

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            x.client_id = _client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id(); //set size
            Derived::size = x.Size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> uint8_t FixedSizeNetPacketToServer<Derived>::message_id(255);
template <class Derived> int FixedSizeNetPacketToServer<Derived>::size(-1);
