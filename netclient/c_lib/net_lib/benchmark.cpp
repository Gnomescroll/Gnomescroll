#include "benchmark.hpp"

#include <time/physics_timer.h>
#include <globals.hpp>

#include <compat_gl.h>

//static int _CtoSbenchmark_counter = 0;



void enet_host_handling()
{

    ENetEvent event;

    uint16_t client_id;
    uint16_t packet_number;
    uint16_t packet_size;

    int n1 = 0;
            
    /* Wait up to 1000 milliseconds for an event. */
    while (enet_host_service (enet_host, & event, 0) > 0)
    {
        switch (event.type)
        {

        case ENET_EVENT_TYPE_RECEIVE:
            

        /*    
            printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet -> dataLength,
                    event.packet -> data,
                    (char*) event.peer -> data,
                    event.channelID);
        */    
            n1 = 0;
            UNPACK_uint16_t(&client_id, (char*) event.packet -> data, &n1); //client id
            UNPACK_uint16_t(&packet_number, (char*) event.packet -> data, &n1); //packet_number
            UNPACK_uint16_t(&packet_size, (char*) event.packet -> data, &n1); //packet_number

            //printf("packet number %i \n", packet_number);

            if(event.packet->dataLength < packet_size)
            {
                printf("Packet data shortfall: expected %i, read %i \n", packet_size, event.packet->dataLength);
                return;
            }

            if(event.packet -> dataLength != packet_size)
            {
                printf("NETWORKING ARMAGEDDON!!! \n");
                printf("packet_data_length= %i, packet_size= %i \n", event.packet -> dataLength, packet_size);
            }

            process_packet_messages((char*) event.packet -> data, n1, packet_size, client_id);

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event.packet);
            
            break;
           
        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("client disconected from server.\n");

            /* Reset the peer's client information. */
            client.connected = 0;
            client.peer = NULL;
            event.peer -> data = NULL;

            //exit(0);
            break;
        /*
        case ENET_EVENT_TYPE_CONNECT:
            printf("receive_packets(): IMPOSSIBLE!!!\n");

            printf ("A new client connected from %x:%u.\n", 
                    event.peer -> address.host,
                    event.peer -> address.port);

            // Store any relevant client information here. 
            event.peer -> data = (char* )"Client information";

            break;
        */
        default:
            printf("receive_packets error: unhandled event \n");
        }
    }


}
void _net_test() {
    //return;

    _start_physics_timer(10);

    int start1 = _get_time();
    int start =  _get_time();
    int c = 0;
    while(1) 
    {
        c++;
        benchmarkCtoS a;
        a.id = 0;
        a.seq = 5;
        a.cs = 15;
        a.theta = 15.60;
        a.phi = 243.0;
        int i;
        for(i=0; i < 75; i++) a.send(); //70

        int ti= _get_time();
        if(ti - start > 5000) 
        {
            printf("%i packets in %i ms, %i per second \n", c, ti - start, c/5);
            start = ti;
            c = 0;
        }
        client.flush_to_net();
        
        enet_host_handling();
        //if(_tick_check() == 1) enet_host_handling();

        //enet_host_handling();

        if(ti -start1 > 20000) {
            exit(0);
            return;
        }
    }
}

/*
//static int _r_sent = 0;
static int _r_received = 0;

class benchmarkReliableCtoS: public FixedSizeReliableNetPacketToServer<benchmarkReliableCtoS>
{
    public:

        int id;
        int seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_u16(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            _r_received++;
            //if(_CtoSbenchmark_counter % 1000 == 0) printf("%i messages\n", _CtoSbenchmark_counter);
            printf("Received %i reliable messages\n", _r_received);
            return;
        }
};
*/